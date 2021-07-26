//
// Created by sherlock on 12/05/2021.
//
#include "receiver.h"
#include "utils.h"

const char SERIAL_END_CHAR = '\n';

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
        CLOption{
            "-o",
            "--output",
            "Define the file name of the receiver data",
            PosArg::OPT_ARG,
        }
};

int main(int argc, char* argv[]) {
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);

    // Setup the signal handler
    SetCtrlHandler();

    auto serialDevice = serialib();
    char serialErr = serialDevice.openDevice(appConfig.arduinoSource.c_str(), 115200);

    optional<vector<LogEntry>> logs;

    char *serialInputBuffer;

    if (serialErr == 1) {
        // Serial device successfully opened
        serialDevice.writeString("?");

        if (serialDevice.readString(serialInputBuffer, SERIAL_END_CHAR, 32) > 0) {
            // WE HAVE A RESPONSE FROM THE ARDUINO
            serialDevice.writeString((to_string(appConfig.pollingRate) + "\n").c_str());
            logs = readSerialPort(serialDevice, appConfig);
        } else {
            // Response is invalid OR buffer is full
        }
    } else {
        return serialErrorHandler(serialErr, appConfig);
    }

    if (logs.has_value()) {
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
        } else {
            printf("Unknown Option %s\n", arg.c_str());
            return;
        }
    }
}

vector<LogEntry> readSerialPort(serialib &serialPort, const Configuration &config) {
    auto logs = vector<LogEntry>();

    while (!exit_app) {

    }

    return logs;
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

}
