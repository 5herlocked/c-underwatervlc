//
// Simple GPIO memory-mapped example by Snarky (github.com/jwatte)
// build with:
//  g++ -O1 -g -o mem gpiomem.cpp -Wall -std=gnu++17
// run with:
//  sudo ./mem
//
//
#include "transmitter.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cerrno>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

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

int main()
{
    // change frequency here
    int frequency = 1;
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
    auto volatile *pin = (transmitter::GPIO_mem volatile *)((char *)base + (GPIO_2 & pagemask));

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
    while (true) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1/frequency));
        val = val ^ 0xff;
        pin->OUT[0] = val;
    }

    return 0 ;
}
