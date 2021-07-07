//
// Created by camv7 on 29/06/2021.
//

/*
 * Accepts 2 csv files through the commandline
 * Analyses their names to find the transmission and receiving rate
 * Decides the multiples of expected bits and analyses
 * Using the baseline: if half of the bits are wrong, the bit is considered in error
 * So:
 *  If transmitting at 25Hz and receiving at 100Hz:
 *      if 2 of the 4 bits received read incorrectly, the bit was received incorrectly
 */

#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>
#include <sstream>

#include "utils.h"
#include "csv.h"

#if __has_include(<filesystem>)

namespace fs = std::filesystem;

#elif __has_include(<experimental/filesystem>)

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#else
error "Missing the <filesystem> header."
#endif

using namespace std;

struct TransmitterLog {
    optional<chrono::duration<double>> deltaTime{};
    optional<int> transmittedBit{};
    optional<string> message{};
};

struct ReceiverLog {
    chrono::duration<double> deltaTime{};
    double colourVals[3]{};
    optional<int> deducedBit{};
};

struct Configuration {
    optional<string> receiverFile;
    optional<string> transmitterFile;
    int transmitRate{};
    int receiveRate{};
};

void parseArgs(int argc, char *argv[], Configuration &config);

double getBer(const Configuration &appConfig, fstream &transmitterFile, fstream &receiverFile);

vector<TransmitterLog> getTransmitterLogs(const string &fileName, fstream &transmitterLogs);

vector<ReceiverLog> getReceiverLogs(const string &fileName, fstream &receiverLogs);

long
getTransmissionStart(const vector<TransmitterLog> &transmitter, const vector<ReceiverLog> &receiver, int recRatio = 1);

void showUsage();

int main(int argc, char *argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    error_code fs_error;

    fstream transmitterCSV (config.transmitterFile.value());
    fstream receiverCSV (config.receiverFile.value());

    if (transmitterCSV.fail()) {
        // Transmitter file broke
        printf("Transmitter File not working %s\n", config.transmitterFile.value().c_str());
    }
    if (receiverCSV.fail()) {
        // Receiver file broke
        printf("Receiver File not working %s\n", config.receiverFile.value().c_str());
    }

    // Make sure the receiver file is actually there

    // Get BER value
    double berValue = getBer(config, transmitterCSV, receiverCSV);

    printf("BER Value: %.2lf%%\n", berValue);
}

void parseArgs(int argc, char **argv, Configuration &config) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            exit(0);
        } else if ((arg == "-r") || (arg == "--receiver")) {
            fs::path file_path = argv[++i];
            config.receiverFile = file_path.string();
        } else if ((arg == "-t") || (arg == "--transmitter")) {
            fs::path file_path = argv[++i];
            config.transmitterFile = file_path.string();
        } else if ((arg == "-tx") || (arg == "--trxrate")) {
            config.transmitRate = strtol(argv[++i], nullptr, 10);
        } else if ((arg == "-rx") || (arg == "--rxrate")) {
            config.receiveRate = strtol(argv[++i], nullptr, 10);
        } else {
            printf("Unknown options: %s, Unknown argument: %s", arg.c_str(), argv[++i]);
        }
    }
}

/*
 * Theory Crafting time:
 *  Ingest the log files so that they're stored in heap
 *  Access them through vecs so it's super easy to window the data
 */
double getBer(const Configuration &appConfig, fstream &transmitterFile, fstream &receiverFile) {
    vector<TransmitterLog> transmitterLogs = getTransmitterLogs(appConfig.transmitterFile.value(), transmitterFile);
    vector<ReceiverLog> receiverLogs = getReceiverLogs(appConfig.receiverFile.value(), receiverFile);

    int recRatio = appConfig.receiveRate/appConfig.transmitRate;

    // After we get valid and failed bits from above
    int receiverL = receiverLogs.size();
    int success = 0;
    int receiverStart = getTransmissionStart(transmitterLogs, receiverLogs, recRatio);
    int* receivedBits = new int[recRatio];
    for (auto & transmitterLog : transmitterLogs) {
        int tBit = transmitterLog.transmittedBit.value();
        int iSucc = 0;

        for (int r = 0; r < recRatio; ++r) {
            if (receiverStart + r < receiverL) {
                optional<int> rBit = receiverLogs[receiverStart + r].deducedBit;
                if (rBit.has_value()) {
                    if (rBit.value() == tBit) {
                        iSucc += 1;
                    }
                    receivedBits[r] = rBit.value();
                }
            } else {
                goto vecOverflow;
            }
        }

        receiverStart += recRatio;

        if (iSucc >= recRatio/2) {
            success += 1;
        }
    }
vecOverflow:
        return (double)success/transmitterLogs.size() * 100;
}

long getTransmissionStart(const vector<TransmitterLog> &transmitter, const vector<ReceiverLog> &receiver, int recRatio) {
    int trackingNum = 3;

    // Create a pattern to match for
    ostringstream stringConstructor;

    // Create the pattern
    for (int i = 0; i < trackingNum; i += recRatio) {
        for (int j = 0; j < recRatio; ++j) {
            stringConstructor << transmitter[i].transmittedBit.value();
        }
    }
    // Using this pattern to find the start
    string startPattern(stringConstructor.str());
    stringConstructor = ostringstream();

    for (int i = 0; i < trackingNum * recRatio * 100; ++i) {
        stringConstructor << receiver[i].deducedBit.value();
    }
    // Just find the tracking pattern
    string receiverString(stringConstructor.str());

    auto found = receiverString.find(startPattern);

    return found != string::npos ? found : -1;

}

vector<TransmitterLog> getTransmitterLogs(const string &fileName, fstream &transmitterLogs) {
    vector<TransmitterLog> transmitter = vector<TransmitterLog>();
    io::CSVReader<3> transmitterCSV(fileName, transmitterLogs);

    transmitterCSV.read_header(io::ignore_extra_column | io::ignore_missing_column, "deltaTime", "bit", "message");

    double deltaTime; int bit; string message;

    auto logRef = TransmitterLog{};

    while (transmitterCSV.read_row(deltaTime, bit, message)) {
        logRef.deltaTime = chrono::duration<double>(deltaTime);
        logRef.transmittedBit = bit;
        logRef.message = message;

        transmitter.push_back(logRef);
    }

    return transmitter;
}

vector<ReceiverLog> getReceiverLogs(const string &fileName, fstream &receiverLogs) {
    vector<ReceiverLog> receiver = vector<ReceiverLog>();
    io::CSVReader<5> receiverCSV(fileName, receiverLogs);

    receiverCSV.read_header(io::ignore_extra_column | io::ignore_missing_column, "Delta Time", "Blue", "Green", "Red", "Bit");

    double deltaTime; double blue; double green; double red; int bit;

    auto logRef = ReceiverLog{};

    while (receiverCSV.read_row(deltaTime, blue, green, red, bit)) {
        logRef.deltaTime = chrono::duration<double>(deltaTime);
        logRef.colourVals[0] = blue;
        logRef.colourVals[1] = green;
        logRef.colourVals[2] = blue;
        logRef.deducedBit = bit;

        receiver.push_back(logRef);
    }

    return receiver;
}

void showUsage() {

}


