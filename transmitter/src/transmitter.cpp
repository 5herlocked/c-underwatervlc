//
// Simple GPIO memory-mapped example by Snarky (github.com/jwatte)

// Modified and maintained by Sherlocked (github.com/5herlocked)
// For MORSELab as part of his responsibilities

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
    transmit();
//    Configuration appConfig{};
//    parseArgs(argc, argv, appConfig);

    return 0;
}

void transmit() {
    long frequency = 1/4;
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
    void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_3 & ~pagemask));
    if (base == nullptr) {
        perror("mmap()");
        exit(1);
    }

    //  set up a pointer for convenient access -- this pointer is to the selected GPIO controller
    auto volatile *pin = (GPIO_mem volatile *)((char *)base + (GPIO_3 & pagemask));

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
    while (generatedRecords < 100) {
        generatedRecords++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        val = val ^ 0xff;
        pin->OUT[0] = val;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printf("Generated Records: %d. Time taken: %ld ms\n", generatedRecords, duration.count());

    munmap(base, pagesize);
    close(fd);
}

//void parseArgs(int argc, char **argv, Configuration& config) {
//    // TODO: make this not use getopt
//    int opt;
//    struct option long_options[] = {
//            {"help", no_argument, nullptr, 'h'},
//            {"state", required_argument, nullptr, 's'},
//            {"random", optional_argument, nullptr, 'r'},
//            {"frequency", optional_argument, nullptr, 'f'},
//            {"cycles", optional_argument, nullptr, 'c'},
//            {nullptr, 0, nullptr, 0}
//    };
//    int optionIdx = 0;
//    while ((opt = getopt_long(argc, argv, "hs:r:f:c:", long_options, &optionIdx)) != -1) {
//        switch (opt) {
//            case 0:
//                // TODO: No arguments, default options
//                break;
//            case 'h':
//                showUsage();
//                exit(0);
//            case 's':
//                // TODO: Configure the app to set the state of the Transmitter
//                break;
//            case 'r':
//                // TODO: Configure the app to set the random bits
//                break;
//            case 'f':
//                // TODO: Configure the app to set the frequency
//                break;
//            case 'c':
//                // TODO: Configure the app to set the cycles
//                break;
//            default:
//                cout << "Unknown option: <" << opt << endl;
//                break;
//        }
//    }
//}
