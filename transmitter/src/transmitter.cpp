// Maintained by Sherlocked (github.com/5herlocked)
// For MORSELab as part of his responsibilities

/*
 * Uses the GPIO_BOARD 12 pin as included in the program
 * Has the ability to generate, transmit and log as configured by the user
 * It only relies on system libraries available on an nVidia Jetson Nano Developer Kit
 */

#include <cstdlib>
#include <chrono>
#include <thread>
#include <optional>
#include <iostream>
#include <vector>
#include <cmath>
#include <csignal>
#include <fstream>

#include "getopt.h"
#include "gpiod.h"
#include "utils.h"

// Change this to move the gpio pin
// reference: https://www.jetsonhacks.com/nvidia-jetson-nano-2gb-j6-gpio-header-pinout/
// use the sysfs GPIO name but only the number
// confirm that the line being used is correct by running the command
// sudo gpiofind "<name_of_pin>"
#define OUT 79

using namespace std;

// Define enums for standardisation
enum APP_TYPE {
    STATE,
    RANDOM,
    // TODO: Just add elements in here as the app gets more complicated
};

// Explicitly stating their internal values
// Though implicitly they mean the same thing
// GPIO::OFF will be interpreted as 0 when directly referenced.
enum GPIO {
    OFF = 0,
    ON = 1,
};


// Main data holding struct to manage the entire configuration of the application
// Realistically there will only be one that'll be created
// but easier to just declare as a full struct
struct Configuration {
    optional<APP_TYPE> type{};
    optional<GPIO> state{};
    optional<int> bits{};
    optional<double> frequency{};
    optional<int> cycles{};
    optional<string> output{};
};

struct LogEntry {
    optional<chrono::duration<double>> deltaTime{};
    optional<int> transmittedBit{};
    optional<string> message{};
};

// function definitions
void parseArgs(int argc, char **argv, Configuration &config);

// These accept a reference to a pointer
void instantiateGPIO(gpiod_chip *&chip, gpiod_line *&pin);

void gpioCleanUp(gpiod_chip *&pChip, gpiod_line *&pLine);

void setState(const Configuration &config);

optional<vector<LogEntry>> transmit(const Configuration &config, const vector<int> &transmission);

void preciseSleep(double seconds);

void generateCSV(const vector<LogEntry>& logs, const Configuration &appConfig);

void showUsage();

void signalHandler(int signal);

optional<double> getFrequency(long frequency);

vector<int> generateRandomTransmission(const int &value);

optional<GPIO> toGPIO(const string &input);

// Test functions
[[maybe_unused]] Configuration getTestConfiguration();

[[maybe_unused]] vector<int> generateBitFlips(int size);

// Runner
int main(int argc, char *argv[]) {
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);

    optional<vector<LogEntry>> logs = vector<LogEntry>();

    if (appConfig.type.has_value()) {
        switch (appConfig.type.value()) {
            case RANDOM:
                // Do logs
                logs = transmit(appConfig, generateRandomTransmission(appConfig.bits.value()));
            case STATE:
                setState(appConfig);
                return 0;
                break;
        }
    } else {
        // Improperly Configured
        printf("It seems the configuration was improperly generated. Please use the -h flag to get the help prompt\n");
        showUsage();
    }

    if (logs.has_value()) {
        // Logs successfully generated
        generateCSV(logs.value(), appConfig);
    } else {
        // Logs failed to generate
        printf("Logs did not generate\n");
    }
    return 0;
}

/*
 * Parses commandline arguments using the GNU C library function getopt_long
 * Reference: https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
 */
void parseArgs(int argc, char **argv, Configuration &config) {
    int opt;
    struct option long_options[] = {
            {"help",      no_argument,       nullptr, 'h'},
            {"state",     required_argument, nullptr, 's'},
            {"random",    optional_argument, nullptr, 'r'},
            {"frequency", required_argument, nullptr, 'f'},
            {"cycles",    required_argument, nullptr, 'c'},
            {"output",    optional_argument, nullptr, 'o'},
            {nullptr,     no_argument,       nullptr, 0}
    };
    int optionIdx = 0;
    while ((opt = getopt_long(argc, argv, "hs:r:f:c:", long_options, &optionIdx)) != -1) {
        switch (opt) {
            case 0:
                //TODO: No arguments, default config
                break;
            case 'h':
                showUsage();
                exit(0);
            case 's':
                if (config.type.has_value() && config.type.value() == APP_TYPE::RANDOM) {
                    cout << "The state flag takes precedence. Using the given state: " << optarg << endl;
                }

                config.type = APP_TYPE::STATE;
                config.state = toGPIO(optarg);
                break;
            case 'r':
                if (config.type.has_value() && config.type.value() == APP_TYPE::STATE) {
                    cout << "The state flag takes precedence. Ignoring the random bits flag" << endl;
                    break;
                }
                config.type = APP_TYPE::RANDOM;
                config.bits = strtol(optarg, nullptr, 10);
                break;
            case 'f':
                config.frequency = getFrequency(strtol(optarg, nullptr, 10));
                break;
            case 't':
                config.cycles = strtol(optarg, nullptr, 10);
                break;
            case 'o':
                config.output = optarg;
                break;
            default:
                printf("Unknown option %s with argument %s", long_options[opt].name, optarg);
                break;
        }
    }
}

void instantiateGPIO(gpiod_chip *&chip, gpiod_line *&pin) {
    chip = gpiod_chip_open_by_number(0);

    if (!chip) {
        // if gpio pin failed
        cout << "GPIO chip failed to open" << endl;
        return;
    }

    // Referring to gpio79 as a part of chip0
    // guessing that this is gpio board pin 12
    pin = gpiod_chip_get_line(chip, OUT);

    if (!pin) {
        // if opening pin failed
        cout << "GPIO pin failed to open" << endl;
        return;
    }
}

void gpioCleanUp(gpiod_chip *&pChip, gpiod_line *&pLine) {
    gpiod_line_release(pLine);
    gpiod_chip_close(pChip);
}

// Sets the GPIO state to either ON or OFF
void setState(const Configuration &config) {
    sigset_t set;
    struct gpiod_chip *chip = nullptr;
    struct gpiod_line *pin = nullptr;

    instantiateGPIO(chip, pin);

    int pinRequest = gpiod_line_request_output(pin, "transmitter_out", config.state.value());
    if (pinRequest) {
        // If pin request failed
        printf("Pin request failed\n");
    }

    int stateRequest = gpiod_line_set_value(pin, config.state.value());

    if (stateRequest) {
        // If set State Failed
        printf("Set state request failed\n");
    }

    cout << "Holding state to " << config.state.value() << endl;
    cout << "Press Ctrl + C to exit and reset the GPIO pin" << endl;

    sigemptyset(&set);

    if (sigwait(&set, reinterpret_cast<int *>(SIGINT)) == SIGINT) {
        gpioCleanUp(chip, pin);
    }
}

/*
 * Transmits using the libgpiod method of gpio manipulation
 * Should be more than fast enough
 * Reference
 */
optional<vector<LogEntry>> transmit(const Configuration &config, const vector<int> &transmission) {
    auto currentEntry = LogEntry{};
    auto logs = vector<LogEntry>();

    struct gpiod_chip *chip = nullptr;
    struct gpiod_line *pin = nullptr;

    instantiateGPIO(chip, pin);

    gpiod_line_request_output(pin, "transmitter_out", 0);

    double frequency = config.frequency.value();
    auto length = transmission.size() * config.cycles.value();
    int transmitted = 0, failed = 0;
    auto t_0 = chrono::high_resolution_clock::now();
    auto prevClock = chrono::high_resolution_clock::now();

    for (int count = 0; count < config.cycles; ++count) {
        for (int i : transmission) {
            auto nextClock = chrono::high_resolution_clock::now();

            int complete = gpiod_line_set_value(pin, i);

            if (complete != 0) {
                // Transmission failed
                failed += 1;
                currentEntry.deltaTime = (t_0 - chrono::high_resolution_clock::now());
                currentEntry.transmittedBit = nullopt;
                currentEntry.message = "Bit dropped";
                logs.push_back(currentEntry);
            } else {
                // Manage Logs
                transmitted += 1;
                currentEntry.deltaTime = (t_0 - chrono::high_resolution_clock::now());
                currentEntry.transmittedBit = i;
                currentEntry.message = nullopt;
                logs.push_back(currentEntry);
            }

            progressBar((float)(transmitted/length), 30, transmitted, failed);

            auto transmitClock = chrono::high_resolution_clock::now();

            double sleepTime = frequency - ((transmitClock - nextClock).count() / 1e9);

            // sleep for dT using a spinLock
            preciseSleep(sleepTime);
        }
    }

    printf("Transmitted: %i\t Failed: %i\n", transmitted, failed);
    gpiod_line_set_value(pin, 0);

    // Clean Up
    gpioCleanUp(chip, pin);

    return logs;
}

/*
 * Uses a combination of thread_sleep (longer time intervals)
 * and spinlocks to get as accurate of a sleep time as we can get without overloading the CPU
 * Assumes thread::sleep_for() has very poor accuracy and compensates for it
 * Borrowed from: https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
 */
void preciseSleep(double seconds) {
    using namespace std::chrono;

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

void generateCSV(const vector<LogEntry> &logs, const Configuration& config) {
    fstream csvStream;
    csvStream.open(config.output.value() + ".csv", ios::out);

    csvStream << "deltaTime" << "," << "bit" << "," << "message" << "\n";

    // frameAverage is of type double[4], we need to destructure it
    for (const LogEntry &entry : logs) {
        csvStream << entry.deltaTime->count() << "," << entry.transmittedBit.value() << "," << entry.message.value() << "\n";
    }

    csvStream.close();
}

void showUsage() {
    printf("./transmitter -s <state> -r <bits> -f <frequency> -c <cycles> -o <output_name>\n");
    printf("-s or --state\t: Set state of the transmitter to either ON or OFF\n");
    printf("-r or --random\t: Define the number of random bits to generate a transmission\n");
    printf("-f or --frequency\t: Define the frequency of the transmission\n");
    printf("-c or --cycles\t: Define the number of times the transmission is to be repeated\n");
    printf("-o or --output\t: Set the name of the logs\n");
}

void signalHandler(int signal) {
    // TODO: Create the Signal Handler and finish the code for that
    // Usually just gpio cleanup and such

}

// Helper functions
optional<double> getFrequency(long frequency) {
    return (1.0/frequency);
}

vector<int> generateRandomTransmission(const int &value) {
    vector<int> transmission = vector<int>();

    for (int i = 0; i < value; ++i) {
        transmission.push_back((int)(rand() % 2));
    }

    return transmission;
}

// Goes from string to enum GPIO
// returns nullopt if the parameter doesn't correspond to on OR off
optional<GPIO> toGPIO(const string &input) {
    if (input == "on" || input == "ON") {
        return GPIO::ON;
    } else if (input == "off" || input == "OFF") {
        return GPIO::OFF;
    }
    return nullopt;
}

// Modify this code to run validation tests
[[maybe_unused]] Configuration getTestConfiguration() {
    Configuration testConfig{};

    testConfig.type = APP_TYPE::RANDOM;
    testConfig.frequency = getFrequency(30'000);
    testConfig.bits = 100'000;
    testConfig.cycles = 1;

    return testConfig;
}

// Creates bit flips so that we can purely test the potency of the application
[[maybe_unused]] vector<int> generateBitFlips(int size) {
    vector<int> transmission = vector<int>();
    int val = 0;
    for (int i = 0; i < size; i++) {
        transmission.push_back(val);
        val = val ^ 1;
    }

    return transmission;
}
