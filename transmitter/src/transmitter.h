//
// Created by camv7 on 22/07/2021.
//

#ifndef TRANSMITTER_TRANSMITTER_H
#define TRANSMITTER_TRANSMITTER_H

#pragma once

#include <cstdlib>
#include <chrono>
#include <thread>
#include <optional>
#include <iostream>
#include <vector>
#include <cmath>
#include <csignal>
#include <fstream>
#include <sstream>

// Change this to move the gpio pin
// reference: https://www.jetsonhacks.com/nvidia-jetson-nano-2gb-j6-gpio-header-pinout/
// use the sysfs GPIO name but only the number
// confirm that the line being used is correct by running the command
// sudo gpiofind "<name_of_pin>"
#define OUT 79

using namespace std;

// Define enums for standardisation
enum APP_TYPE {
    STATE,
    RANDOM,
    TEST,
    // TODO: Just add elements in here as the app gets more complicated
};

// Explicitly stating their internal values
// Though implicitly they mean the same thing
// GPIO::OFF will be interpreted as 0 when directly referenced.
enum GPIO {
    OFF = 0,
    ON = 1,
};


// Main data holding struct to manage the entire configuration of the application
// Realistically there will only be one that'll be created
// but easier to just declare as a full struct
struct Configuration {
    optional<APP_TYPE> type{};
    optional<GPIO> state{};
    optional<int> bits{};
    optional<double> frequency{};
    optional<int> cycles{};
    optional<string> output{};
};

struct LogEntry {
    optional<chrono::duration<double>> deltaTime{};
    optional<int> transmittedBit{};
    optional<string> message{};
};

// function definitions
void parseArgs(int argc, char **argv, Configuration &config);

// These accept a reference to a pointer
void instantiateGPIO(gpiod_chip *&chip, gpiod_line *&pin);

void gpioCleanUp(gpiod_chip *&pChip, gpiod_line *&pLine);

void setState(const Configuration &config);

optional<vector<LogEntry>> transmit(const Configuration &config, const vector<int> &transmission);

void preciseSleep(double seconds);

void generateCSV(const vector<LogEntry>& logs, const Configuration &appConfig);

void showUsage();

void signalHandler(int signal);

optional<double> getFrequency(long frequency);

vector<int> generateRandomTransmission(const int &value);

optional<GPIO> toGPIO(const string &input);

// Test functions
[[maybe_unused]] Configuration getTestConfiguration();

[[maybe_unused]] vector<int> generateBitFlips(int size);

#endif //TRANSMITTER_TRANSMITTER_H
