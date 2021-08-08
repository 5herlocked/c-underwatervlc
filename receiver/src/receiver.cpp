//
// Created by sherlock on 12/05/2021.
//
#include "receiver.h"
#include "utils.h"

// The COM ports on windows are referenced with \\\\.\\COM<x>
// On linux as /dev/tty<x>

// Modify these constant globals to change internals
constexpr char SERIAL_END_CHAR = '\n';
constexpr int ADC_RESOLUTION = 1023;
constexpr double ADC_VOLTAGE = 5.00;
const string ADC_READY_STRING = "Ready";
const string ADC_CONNECTED_STRING = "Connected";

const vector<CLOption> PROGRAM_OPTIONS = {
        CLOption{
                "-h",
                "--help",
                "Prints the help menu",
                PosArg::NO_ARG,
        },
        CLOption{
                "-s",
                "--source",
                "Define the source of serial communication from the Arduino",
                PosArg::REQ_ARG,
        },
        CLOption {
            "-f",
            "--frequency",
            "Define the frequency of the arduino's polling rate",
            PosArg::REQ_ARG,
        },
        CLOption{
                "-o",
                "--output",
                "Define the file name of the receiver data",
                PosArg::OPT_ARG,
        },
        CLOption{
            "-t",
            "--test",
            "Starts the testing suite",
            PosArg::NO_ARG,
        }
};

int main(int argc, char *argv[]) {
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);

    // Setup the signal handler
    SetCtrlHandler();

    serialib serialDevice;
    int serialErr = serialDevice.openDevice(appConfig.arduinoSource.c_str(), 115200);

    optional<vector<LogEntry>> logs;

    char *serialInputBuffer = new char[32];

    if (serialErr == 1) {
        // Sleeps for 2 seconds to make sure the arduino has time to respond
        preciseSleep(2);

        if (serialDevice.readString(serialInputBuffer, SERIAL_END_CHAR, 32) > 0) {
            // WE HAVE A RESPONSE FROM THE ARDUINO
            // Serial device successfully opened
            logs = readSerialPort(serialDevice, appConfig);
        } else {
            // Response is invalid

        }
    } else {
        return serialErrorHandler(serialErr, appConfig);
    }

    if (logs.has_value()) {
        printf("Log size: %zu\n", logs->size());
        writeLogs(logs.value(), appConfig);
    } else {
        printf("Logs not generated\n");
    }

    // Successfully returns
    return 0;
}

void parseArgs(int argc, char **argv, Configuration &config) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            // HELP
            showUsage();
            exit(0);
        } else if ((arg == "-s") || (arg == "--source")) {
            // SERIAL PORT LOCATION
            config.arduinoSource = argv[++i];
        } else if ((arg == "-o") || (arg == "--output")) {
            // OUTPUT LOCATION
            config.output = argv[++i];
        } else if ((arg == "-f") || (arg == "--frequency")){
            // Polling Rate
            config.pollingRate = strtol(argv[++i], nullptr, 10);
        } else if ((arg == "-t") || (arg == "--test")) {
            // Test instance
            config = getTestConfig();
        } else {
                printf("Unknown Option %s\n", arg.c_str());
                return;
            }
        }
    }

vector<LogEntry> readSerialPort(serialib &serialPort, const Configuration &config) {
    using namespace chrono;

    auto pollTime = getFrequency(config.pollingRate);

    auto logs = vector<LogEntry>();

    char *serialInputBuffer = new char[32];

    const auto startTime = high_resolution_clock::now();
    while (!exit_app) {
        auto startClock = high_resolution_clock::now();
        if (serialPort.readString(serialInputBuffer, SERIAL_END_CHAR, 32) > 0) {
            int analogValue = strtol(serialInputBuffer, nullptr, 10);
            logs.push_back(
                    LogEntry{
                        high_resolution_clock::now() - startTime,
                        getVoltage(analogValue),
                        analogValue,
                        });
        }
        progressBar(logs.size());
        auto recordClock = high_resolution_clock::now();
        double sleepTime = pollTime - ((recordClock - startClock).count() / 1e9);
        if (sleepTime <= 0) {
            // Negative Sleep
            continue;
        }
        // Sleep for half the requested polling rate
        // So we're still checking more often than the values are likely to come in
        preciseSleep(sleepTime/2);
    }
    const auto endTime = high_resolution_clock::now();

    return logs;
}

/*
 * (ADC Reading * System Voltage)/ADC Resolution = Voltage Value
 */
double getVoltage(int analogVoltage) {
    return (analogVoltage * ADC_VOLTAGE)/ADC_RESOLUTION;
}

/*
 * Uses a combination of thread_sleep (longer time intervals)
 * and spinlock to get as accurate of a sleep time as we can get without overloading the CPU
 * Assumes thread::sleep_for() has very poor accuracy and compensates for it
 * Borrowed from: https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
 */
void preciseSleep(double seconds) {
    using namespace chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        this_thread::sleep_for(milliseconds(1));
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

void writeLogs(const std::vector<LogEntry> &logs, const Configuration &config) {
    fstream csvStream;
    if (config.output.has_value()) {
        csvStream.open(config.output.value() + ".csv", ios::out);
    } else {
        ostringstream csvName;
        // TODO: Put in the actual default name
        csvName << "photodiode_";

        csvStream.open(csvName.str(), ios::out);
    }

    csvStream << "deltaTime" << "," << "analogValue" << "," << "voltage" << "\n";

    // frameAverage is of type double[4], we need to destructure it
    for (const LogEntry &entry : logs) {
        csvStream << entry.deltaTime.count() << "," << entry.analogValue << "," << entry.deducedVoltage << "\n";
    }

    csvStream.close();
}

int serialErrorHandler(int serialErr, const Configuration &appConfig) {
    switch (serialErr) {
        case -1:
            printf("Device %s not found\n", appConfig.arduinoSource.c_str());
            return -1;
        case -2:
            printf("Error while opening device\n");
            return -2;
        case -3:
            printf("Error while reading port parameters\n");
            return -3;
        case -4:
            printf("Serial Speed unsupported\n");
            return -4;
        case -5:
            printf("Error while writing port parameters\n");
            return -5;
        case -6:
            printf("Error while writing timeout parameters\n");
            return -6;
        default:
            printf("Undefined behaviour while opening the serial device\n");
            return -1;
            break;
    }
}

void showUsage() {
    ostringstream helpBuilder;
    helpBuilder << "./receiver ";
    for (const CLOption& opt : PROGRAM_OPTIONS) {
        helpBuilder << opt.shortOpt << " ";
    }
    helpBuilder << endl;
    for (const CLOption& opt : PROGRAM_OPTIONS) {
        helpBuilder << opt.shortOpt << " or " << opt.longOpt << "\t: " << opt.description << endl;
    }

    printf("%s", helpBuilder.str().c_str());
}

Configuration getTestConfig() {
    Configuration testConfig{};

    testConfig.arduinoSource = R"(/dev/ttyUSB0)";
    testConfig.pollingRate = 10'000;
    testConfig.output = "testReceiver";

    return testConfig;
}

double getFrequency(long frequency) {
    return (1.0/frequency);
}
