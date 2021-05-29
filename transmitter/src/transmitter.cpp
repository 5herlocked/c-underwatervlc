//
// Simple GPIO memory-mapped example by Snarky (github.com/jwatte)

// Modified and maintained by Sherlocked (github.com/5herlocked)
// For MORSELab as part of his responsibilities

/* Tegra X1 SoC Technical Reference Manual, version 1.3
 *
 * See Chapter 9 "Multi-Purpose I/O Pins", section 9.13 "GPIO Registers"
 * (table 32: GPIO Register Address Map)
 *
 * The GPIO hardware shares PinMux with up to 4 Special Function I/O per
 * pin, and only one of those five functions (SFIO plus GPIO) can be routed to
 * a pin at a time, using the PixMux.
 *
 * In turn, the PinMux outputs signals to Pads using Pad Control Groups. Pad
 * control groups control things like "drive strength" and "slew rate," and
 * need to be reset after deep sleep. Also, different pads have different
 * voltage tolerance. Pads marked "CZ" can be configured to be 3.3V tolerant
 * and driving; and pads marked "DD" can be 3.3V tolerant when in open-drain
 * mode (only.)
 *
 * The CNF register selects GPIO or SFIO, so setting it to 1 forces the GPIO
 * function. This is convenient for those who have a different pinmux at boot.
 */

#include "transmitter.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <optional>
#include <getopt.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <iostream>

using namespace std;

// The only address we really need
#define GPIO_1     0x6000d000
#define GPIO_2     0x6000d100
#define GPIO_3     0x6000d200
#define GPIO_4     0x6000d300
#define GPIO_5     0x6000d400
#define GPIO_6     0x6000d500
#define GPIO_7     0x6000d600
#define GPIO_8     0x6000d700

//  layout based on the definitions above
//  Each GPIO controller has four ports, each port controls 8 pins, each
//  register is interleaved for the four ports, so
//  REGX: port0, port1, port2, port3
//  REGY: port0, port1, port2, port3
struct GPIO_mem {
    uint32_t CNF[4];
    uint32_t OE[4];
    uint32_t OUT[4];
    uint32_t IN[4];
    uint32_t INT_STA[4];
    uint32_t INT_ENB[4];
    uint32_t INT_LVL[4];
    uint32_t INT_CLR[4];
};

enum APP_TYPE {
    STATE,
    RANDOM,
    // TODO: Just add elements in here as the app gets more complicated
};

enum GPIO_STATE {
    OFF,
    ON,
};

struct Configuration {
    optional<APP_TYPE> type;
    optional<GPIO_STATE> state;
    optional<int> bits;
    optional<int> frequency;
    optional<int> cycles;
};

void parseArgs(int argc, char **argv, Configuration& config);
void transmit();
void showUsage();

int main(int argc, char* argv[])
{
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);


    return 0;
}

void transmit() {
    //  read physical memory (needs root)
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root (for example with sudo)\n");
        exit(1);
    }

    //  map a particular physical address into our address space
    int pagesize = getpagesize();
    int pagemask = pagesize-1;
    //  This page will actually contain all the GPIO controllers, because they are co-located
    void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_2 & ~pagemask));
    if (base == nullptr) {
        perror("mmap()");
        exit(1);
    }

    //  set up a pointer for convenient access -- this pointer is to the selected GPIO controller
    auto volatile *pin = (GPIO_mem volatile *)((char *)base + (GPIO_2 & pagemask));

    pin->CNF[0] = 0x00ff;
    pin->OE[0] = 0xff;
    pin->OUT[0] = 0xff;
    //  pin->IN = 0x00; read only
    //  disable interrupts
    pin->INT_ENB[0] = 0x00;
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
        std::this_thread::sleep_for(std::chrono::seconds(1/frequency));
        val = val ^ 0xff;
        pin->OUT[0] = val;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Generated Records: %d. Time taken: %ld ms\n", generatedRecords, duration.count());
}

void parseArgs(int argc, char **argv, Configuration& config) {
    int opt;
    struct option long_options[] = {
            {"help", no_argument, nullptr, 'h'},
            {"state", required_argument, nullptr, 's'},
            {"random", optional_argument, nullptr, 'r'},
            {"frequency", optional_argument, nullptr, 'f'},
            {"cycles", optional_argument, nullptr, 'c'},
            {nullptr, 0, nullptr, 0}
    };
    int optionIdx = 0;
    while ((opt = getopt_long(argc, argv, "hs:r:f:c:", long_options, &optionIdx)) != -1) {
        switch (opt) {
            case 0:
                // TODO: No arguments, default options
                break;
            case 'h':
                showUsage();
                exit(0);
            case 's':
                // TODO: Configure the app to set the state of the Transmitter
                break;
            case 'r':
                // TODO: Configure the app to set the random bits
                break;
            case 'f':
                // TODO: Configure the app to set the frequency
                break;
            case 'c':
                // TODO: Configure the app to set the cycles
                break;
            default:
                cout << "Unknown option: <" << opt << endl;
                break;
        }
    }
}
