#include <Arduino.h>

int analogPin = A0;
uint8_t val = 0;

// FORCE 10k SPS
unsigned long wait_duration = 1.0/10'000 * 1'000'000;

//boolean connected = false;
//boolean newData = false;

//String receivedChars;
//double pollingRate = 1.0/10'000 * 1'000'000;

void setup() {
    Serial.begin(115200);
    pinMode(analogPin, INPUT);
    Serial.println("Ready");
}

//void recvPollingRate() {
//    if (Serial.available() > 0) {
//         receivedChars = Serial.readString();
//         Serial.println(receivedChars);
//
//        // 1/Hz is the amount of seconds it needs to sleep
//        // * 10,000 to get milliseconds to sleep
//        // The Microcontroller MAXES out at 10kSPS
//        // This means at MAX the sleep time will be 100 milliseconds
//        pollingRate = (1.0/strtol(receivedChars.c_str(), nullptr, 10)) * 1'000'000;
//        wait_duration = pollingRate;
//        if (pollingRate > 0.0) {
//            Serial.println(pollingRate);
//            newData = true;
//        }
//    }
//}
//
//bool readPacket(char value) {
//    char rc;
//    if (Serial.available() > 0) {
//        rc = Serial.read();
//        Serial.readString();
//
//        if (rc == value) {
//            return true;
//        }
//    }
//
//    return false;
//}

//    if (!connected) {
//        connected = readPacket('?');
//        if (connected) {
//            Serial.println("Connected");
//        }
//        return;
//    }
//
//    if (!newData) {
//        recvPollingRate();
//        if (newData) {
//            Serial.println("Polling Rate Acquired");
//            Serial.println(pollingRate);
//        }
//        return;
//    }
//
//    if (connected) {
//        connected = !readPacket('!');
//        return;
//    }

void loop() {
    auto next_clock = micros();

    val = analogRead(analogPin)>>2;
    Serial.write(val);

    auto sleep_time = wait_duration - ((micros() - next_clock));
    delayMicroseconds(sleep_time);
}
