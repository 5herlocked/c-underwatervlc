//
// Created by sherlock on 12/05/2021.
//

// Borrowed from YoungJin Suh (github.com/valentis)
// Modified for morselab by Shardul Vaidya

#ifndef C_UNDERWATERVLC_TRANSMITTER_H
#define C_UNDERWATERVLC_TRANSMITTER_H

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

#pragma once
#include <stdint.h>

// The only address we really need
// These are full controller addresses
// Don't need them but leaving them in here
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

/*
 * Referring to the pinmux configuration and nvidia's forum site below
 * one can deduce the address of the gpio pin and it's related offset
 * 
 * Specifically for this project we're referring to GPIO pin 12
 * that means GPIO03_PJ.07 -> 0x6000d204
 * but to manipulate the specific bit, we need to use GPIO_mem->OUT[4]
 * Since we need to manipulate bit 07
 */

class transmitter {

};


#endif //C_UNDERWATERVLC_TRANSMITTER_H
