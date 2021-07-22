//
// Created by sherlock on 12/05/2021.
//
#include "receiver.h"

const vector<CLOption> PROGRAM_OPTIONS = {
        CLOption{
            "-h",
            "--help",
            "Prints the help menu",
            PosArg::NO_ARG,
        },
        CLOption{
            "-s",
            "--source",
            "Define the source of serial communication from the Arduino",
            PosArg::REQ_ARG,
        },
        CLOption{
            "-o",
            "--output",
            "Define the file name of the receiver data",
            PosArg::OPT_ARG,
        }
};

int main(int argc, char* argv[]) {
    Configuration appConfig{};
    parseArgs(argc, argv, appConfig);

    auto serialDevice = serialib();
    char serialErr = serialDevice.openDevice(appConfig.arduinoSource.c_str(), 115200);
    if (serialErr == 1) {
        // Serial device successfully opened
    } else {
        return serialErrorHandler(serialErr, appConfig);
    }

    // Successfully returns
    return 0;
}

void parseArgs(int argc, char **argv, Configuration &config) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            // HELP
            showUsage();
            exit(0);
        } else if ((arg == "-s") || (arg == "--source")) {
            // SERIAL PORT LOCATION
        } else if ((arg == "-o") || (arg == "--output")) {
            // OUTPUT LOCATION
        } else {
            printf("Unknown Option %s\n", arg.c_str());
            return;
        }
    }
}

void writeLogs(const std::vector<LogEntry> &logs, const Configuration &config) {
    fstream csvStream;
    if (config.output.has_value()) {
        csvStream.open(config.output.value() + ".csv", ios::out);
    } else {
        ostringstream csvName;
        // TODO: Put in the actual default name
        csvName << "photodiode_";

        csvStream.open(csvName.str(), ios::out);
    }

    csvStream << "deltaTime" << "," << "analogValue" << "," << "voltage" << "\n";

    // frameAverage is of type double[4], we need to destructure it
    for (const LogEntry &entry : logs) {
        csvStream << entry.deltaTime.count() << "," << entry.analogValue << "," << entry.deducedVoltage << "\n";
    }

    csvStream.close();
}

int serialErrorHandler(int serialErr, const Configuration &appConfig) {
    switch (serialErr) {
        case -1:
            printf("Device %s not found\n", appConfig.arduinoSource.c_str());
            return -1;
        case -2:
            printf("Error while opening device\n");
            return -2;
        case -3:
            printf("Error while reading port parameters\n");
            return -3;
        case -4:
            printf("Serial Speed unsupported\n");
            return -4;
        case -5:
            printf("Error while writing port parameters\n");
            return -5;
        case -6:
            printf("Error while writing timeout parameters\n");
            return -6;
        default:
            printf("Undefined behaviour while opening the serial device\n");
            return -1;
            break;
    }
}

void showUsage() {

}
