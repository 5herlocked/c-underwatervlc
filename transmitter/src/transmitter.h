//
// Created by sherlock on 12/05/2021.
//

#ifndef C_UNDERWATERVLC_TRANSMITTER_H
#define C_UNDERWATERVLC_TRANSMITTER_H


#include <cstdint>

class transmitter {
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
    public: struct GPIO_mem {
        uint32_t CNF[4];
        uint32_t OE[4];
        uint32_t OUT[4];
        uint32_t IN[4];
        uint32_t INT_STA[4];
        uint32_t INT_ENB[4];
        uint32_t INT_LVL[4];
        uint32_t INT_CLR[4];
    };
};


#endif //C_UNDERWATERVLC_TRANSMITTER_H
