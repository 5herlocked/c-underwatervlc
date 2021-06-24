//
// Simple GPIO memory-mapped example by Snarky (github.com/jwatte)

// Modified and maintained by Sherlocked (github.com/5herlocked)
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

#include "transmitter.h"
#include "getopt.h"
#include "unistd.h"
#include "gpiod.h"

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
    optional<chrono::duration<double>> frequency{};
    optional<int> cycles{};
};

struct LogEntry {
    chrono::duration<double> deltaTime{};
    optional<int> transmittedBit{};
    optional<string> message{};
};

// function definitions
void parseArgs(int argc, char **argv, Configuration &config);

// [[maybe_unused]] void transmitRaw(const Configuration &config);

optional<vector<LogEntry>> transmit(const Configuration &config, const vector<int> &transmission);

void setState(const Configuration &config);

void showUsage();

optional<GPIO> toGPIO(const string &input);

vector<int> generateRandomTransmission(const int &value);

optional<chrono::duration<double>> getFrequency(long frequency);

int main(int argc, char *argv[]) {
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);

    optional<vector<LogEntry>> logs = vector<LogEntry>();

    if (appConfig.type.has_value()) {
        switch (appConfig.type.value()) {
            case STATE:
                setState(appConfig);
                return 0;
            case RANDOM:
                logs = transmit(appConfig, generateRandomTransmission(appConfig.bits.value()));
                break;
        }
    } else {
        // Improperly Configured
    }

    // Do logs

    return 0;
}

vector<int> generateRandomTransmission(const int &value) {
    vector<int> transmission = vector<int>();

    for (int i = 0; i < value; ++i) {
        transmission.push_back((int)(rand() % 2));
    }

    return transmission;
}

/*
 * Parses commandline arguments using the GNU C library function getopt_long
 * Reference: https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html
 */
void parseArgs(int argc, char **argv, Configuration &config) {
    // TODO: make this not use getopt
    int opt;
    struct option long_options[] = {
            {"help",      no_argument,       nullptr, 'h'},
            {"state",     required_argument, nullptr, 's'},
            {"random",    optional_argument, nullptr, 'r'},
            {"frequency", required_argument, nullptr, 'f'},
            {"cycles",    required_argument, nullptr, 'c'},
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
            case 'c':
                config.cycles = strtol(optarg, nullptr, 10);
                break;
            default:
                cout << "Unknown option: <" << opt << endl;
                break;
        }
    }
}

optional<chrono::duration<double>> getFrequency(long frequency) {
    auto frequencyTime = chrono::duration<double>(1/frequency);

    return frequencyTime;
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

void setState(const Configuration &config) {
    struct gpiod_chip *chip;
    struct gpiod_line *pin;

    chip = gpiod_chip_open_by_number(0);

    if (!chip) {
        cout << "GPIO chip failed to open" << endl;
        return;
    }

    // Referring to gpio79 as a part of chip0
    // guessing that this is gpio board pin 12
    pin = gpiod_chip_get_line(chip, OUT);

    if (!pin) {
        cout << "GPIO pin failed to open" << endl;
        return;
    }

    gpiod_line_request_output(pin, "transmitter_out", config.state.value());

    cout << "Holding state to " << config.state.value() << endl;
    cout << "Press Ctrl + C to exit and reset the GPIO pin" << endl;
}

/*
 * Transmits using the libgpiod method of gpio manipulation
 * Should be more than fast enough
 * Reference
 */
optional<vector<LogEntry>> transmit(const Configuration &config, const vector<int> &transmission) {
    auto currentEntry = LogEntry{};
    auto logs = vector<LogEntry>();

    struct gpiod_chip *chip;
    struct gpiod_line *pin;

    chip = gpiod_chip_open_by_number(0);

    if (!chip) {
        cout << "GPIO chip failed to open" << endl;
        return nullopt;
    }

    // Referring to gpio79 as a part of chip0
    // guessing that this is gpio board pin 12
    pin = gpiod_chip_get_line(chip, OUT);

    if (!pin) {
        cout << "GPIO pin failed to open" << endl;
        return nullopt;
    }

    gpiod_line_request_output(pin, "transmitter_out", 0);
    auto t_0 = chrono::high_resolution_clock::now();

    for (int i : transmission) {
        auto t_i = chrono::high_resolution_clock::now();

        gpiod_line_set_value(pin, i);

        // Manage Logs
        currentEntry.deltaTime = (t_0 - chrono::high_resolution_clock::now());
        currentEntry.transmittedBit = i;
        currentEntry.message = nullopt;
        logs.push_back(currentEntry);

        // sleep for dT
        //this_thread::sleep_for(config.frequency.value() - (chrono::high_resolution_clock::now() - t_i));
    }

    auto timeTaken = (chrono::high_resolution_clock::now() - t_0);
    cout << "Time taken: " << chrono::duration_cast<chrono::nanoseconds>(timeTaken).count() << " ns" << endl;
    return logs;
}

void showUsage() {
    cout << ""
}

// optional TODO: figure out memory manipulation
/*
[[maybe_unused]] void transmitRaw() {
    long frequency = 1 / 4;
    //  read physical memory (needs root)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root (for example with sudo)\n");
        exit(1);
    }

    //  map a particular physical address into our address space
    int pagesize = getpagesize();
    int pagemask = pagesize - 1;
    //  This page will actually contain all the GPIO controllers, because they are co-located
    void *base = mmap(nullptr, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_3 & ~pagemask));
    if (base == nullptr) {
        perror("mmap()");
        exit(1);
    }

    //  set up a pointer for convenient access -- this pointer is to the selected GPIO controller
    auto volatile *pin = (GPIO_mem volatile *) ((char *) base + (GPIO_3 & pagemask));

    int port = 0;

    cout << pin->CNF[port] << endl;
    cout << pin->OE[port] << endl;
    cout << pin->OUT[port] << endl;
    cout << pin->INT_ENB[port] << endl;

    pin->CNF[port] = 0x00ff;
    pin->OE[port] = 0xff;
    pin->OUT[port] = 0xff;
    //  pin->IN = 0x00; read only
    //  disable interrupts
    pin->INT_ENB[port] = 0x00;
    //  don't worry about these for now
    //pin->INT_STA[0] = 0x00;
    //pin->INT_LVL[0] = 0x000000;
    //pin->INT_CLR[0] = 0xffffff;

    fprintf(stderr, "press ctrl-C to stop\n");

    //  "blink" the output values
    uint8_t val = 0xff;
    int generatedRecords = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (generatedRecords < 10) {
        generatedRecords++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        val = val ^ 0xff;
        pin->OUT[port] = val;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Generated Records: %d. Time taken: %ld ms\n", generatedRecords, duration.count());

    munmap(base, pagesize);
    close(fd);
}
*/