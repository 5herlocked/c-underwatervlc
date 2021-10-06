//
// Created by camv7 on 23/09/2021.
//

#include "Packet.h"


Packet::Packet(std::string &payload) {
    m_payloadFill = payload.size();
    m_payload = new unsigned char[m_payloadFill];
    strncpy(reinterpret_cast<char *>(m_payload), payload.c_str(), m_payloadFill);

    m_parityBit = generateParityBit(payload);
}

unsigned char Packet::generateParityBit(const std::string &payload) {
    uint8_t parityStorage = 0;

    for(char i : payload) {
        parityStorage ^= i & 0x01;
    }

    return (parityStorage & 0x01);
}

// Converts the embedded payload into a working transmission including parity bits
std::vector<int> Packet::getTransmission() {
    std::vector<int> transmission = std::vector<int>();

    // append header
    for (unsigned char bits : header) {
        transmission.push_back(bits);
    }

    // append parity
    transmission.push_back(m_parityBit);

    // inserts all the transmission bits
    for (int i = 0; i < m_payloadFill; ++i) {
        for (int j = 7; j >= 0; --j) {
            // a j bit bitmask
            int bit = (m_payload[i] >> j) & 0x01;
            transmission.push_back(bit);
        }
    }

    for (unsigned char bit : terminate) {
        transmission.push_back(bit);
    }

    return transmission;
}

Packet::~Packet() {
    delete [] m_payload;
}
