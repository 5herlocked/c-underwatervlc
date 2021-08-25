#include <SPI.h>
#include <SD/src/SD.h>
#include <Arduino.h>

const int analogPin = A0;
uint16_t val = 0;
const int sdSelect = 10;

unsigned long start_time;

File outputFile;

// FORCE 10k SPS
unsigned long wait_duration = 1.0/10'000 * 1'000'000;

double pollingRate = 1.0/10'000 * 1'000'000;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ;
    }

    Serial.print("Initializing SD card...\n");
    if (!SD.begin(sdSelect)) {

        Serial.println("initialization failed. Things to check:");

        Serial.println("1. is a card inserted?");

        Serial.println("2. is your wiring correct?");

        Serial.println("3. did you change the chipSelect pin to match your shield or module?");

        Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");

        exit(-1);
    }
    Serial.print("Initialisation done.\n");
    pinMode(analogPin, INPUT);
    Serial.println("Ready");
}

void loop() {
    if (Serial.available() > 0) {
        // if incoming signal is 0x01 -> Make new file & Start recording
        // if incoming signal is 0x09 -> Stop recording & Close file
        byte r = Serial.read();
        byte name = Serial.read();
        switch (r) {
            case 0x01:
                start_time = micros();
                // outputFile = SD.open();
                Serial.write(outputFile.name()); // send back the file name
                Serial.write(0x01); // ACK
                break;
            case 0x09:
                outputFile.close();
                Serial.write(outputFile.name()); // send back the file name
                Serial.write(0x09); //ACK
                break;
            default:
                //do nothing
                break;
        }
    }
>>>>>>> Stashed changes

void loop() {
    auto next_clock = micros();

    val = analogRead(analogPin);

    // Write in csv to file
    outputFile.write(micros() - start_time);
    outputFile.write(',');
    outputFile.write(val);

    auto sleep_time = wait_duration - ((micros() - next_clock));

    if (sleep_time > 0) {
        delayMicroseconds(sleep_time);
    }
}
