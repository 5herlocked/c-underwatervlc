//
// Created by camv7 on 21/07/2021.
//
#include <string>
#include <optional>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cmath>

#include "serialib.h"

#ifndef RECEIVER_RECEIVER_H
#define RECEIVER_RECEIVER_H

using namespace std;

struct Configuration {
    string arduinoSource{};
    int pollingRate{};
    optional<string> output{};
};

struct LogEntry {
    chrono::duration<double> deltaTime{};
    double deducedVoltage{};
    int analogValue{};
};

enum PosArg {
    NO_ARG,
    OPT_ARG,
    REQ_ARG,
};

struct CLOption {
    string shortOpt{};
    string longOpt{};
    string description{};
    PosArg arguments{};
};

void parseArgs(int argc, char *argv[], Configuration &config);

void writeLogs(const std::vector<LogEntry> &logs, const Configuration &config);

vector<LogEntry> readSerialPort(serialib &serialPort, const Configuration &config);

double getVoltage(int analogVoltage);

void preciseSleep(double seconds);

int serialErrorHandler(int serialErr, const Configuration &appConfig);

void showUsage();

double getFrequency(long frequency);

Configuration getTestConfig();

#endif //RECEIVER_RECEIVER_H
