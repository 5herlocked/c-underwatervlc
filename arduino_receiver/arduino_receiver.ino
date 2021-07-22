#include <Arduino.h>

int analogPin = A0;
int val = 0;

unsigned long wait_duration;

boolean newData = false;

const byte numChars = 32;
char receivedChars[numChars];

void setup() {
    Serial.begin(115200);
    pinMode(analogPin, INPUT);
    Serial.println("Ready");
}

void recvPollingRate() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    if (Serial.available() > 0) {
        rc = Serial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;

            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        } else {
            receivedChars[ndx] = '\0'; // terminate string
            ndx = 0;
            newData = true;
        }
    }

    // 1/Hz is the amount of seconds it needs to sleep
    // * 10,000 to get milliseconds to sleep
    // The Microcontroller MAXES out at 10kSPS
    // This means at MAX the sleep time will be 100 milliseconds
    pollingRate = (1.0/strtol(receivedChars, nullptr, 10)) * 10'000;
}

void loop() {
    if (!newData) {
        recvPollingRate();
        return;
    }

    auto next_clock = millis();

    val = analogRead(analogPin);
    Serial.write(val);

    auto sleep_time = wait_duration - ((millis() - next_clock));

    delay(sleep_time);
}
