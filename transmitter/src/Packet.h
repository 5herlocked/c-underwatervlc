//
// Created by camv7 on 23/09/2021.
//

#ifndef TRANSMITTER_PACKET_H
#define TRANSMITTER_PACKET_H

#pragma once
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#define PAYLOAD_SIZE 8

class Packet {
private:
    static constexpr unsigned char header[] = {1, 1, 1, 0, 0, 1, 0};
    static constexpr unsigned char terminate[] = {0, 0, 0, 0, 0, 0, 0, 0};
    static unsigned char generateParityBit(const std::string &payload);

    unsigned char* m_payload;
    int m_payloadFill;
    unsigned char m_parityBit;

public:
    virtual ~Packet();

    explicit Packet(std::string &payload);

    std::vector<int> getTransmission();
};


#endif //TRANSMITTER_PACKET_H
