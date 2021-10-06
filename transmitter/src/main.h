//
// Created by camv7 on 22/07/2021.
//

#ifndef TRANSMITTER_MAIN_H
#define TRANSMITTER_MAIN_H

#pragma once

#include <cstdlib>
#include <chrono>
#include <cstring>
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
enum AppType {
    STATE,
    RANDOM,
    MESSAGE,
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
    optional<AppType> type{};
    optional<GPIO> state{};
    optional<int> bits = 300;
    optional<string> message{};
    optional<double> frequency = 25;
    optional<int> cycles = 1;
    optional<string> output{};
};

struct LogEntry {
    optional<chrono::duration<double>> deltaTime{};
    optional<int> transmittedBit{};
    optional<string> message{};
};

#endif //TRANSMITTER_MAIN_H
