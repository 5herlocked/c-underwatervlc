#include <Arduino.h>

int analogPin = A0;
int val = 0;
unsigned long prev_time;

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
