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


#include "utils.h"
#include "csv.h"

#if __has_include(<filesystem>)

#include <filesystem>
#include <sstream>

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
    double deltaTime{};
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

int main(int argc, char *argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    error_code fs_error;

    fstream transmitterCSV (config.transmitterFile.value());
    fstream receiverCSV (config.receiverFile.value());

    if (transmitterCSV.fail()) {
        // Transmitter file broke
    }

    if (receiverCSV.fail()) {
        // Receiver file broke
    }

    // Make sure the receiver file is actually there

    // Get BER value
    double berValue = getBer(config, transmitterCSV, receiverCSV);
}

void parseArgs(int argc, char **argv, Configuration &config) {

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

    int startOfTransmission = getTransmissionStart(transmitterLogs, receiverLogs, recRatio);

    // After we get valid and failed bits from above
    double ber = 0;

    return ber;
}

long getTransmissionStart(const vector<TransmitterLog> &transmitter, const vector<ReceiverLog> &receiver, int recRatio) {
    int trackingNum = 3;

    // Create a pattern to match for
    std::ostringstream stringConstructor;

    // Create the pattern
    for (int i = 0; i < trackingNum; i += recRatio) {
        for (int j = 0; j < recRatio; ++j) {
            stringConstructor << transmitter[i].transmittedBit.value();
        }
    }
    // Using this pattern to find the start
    string startPattern(stringConstructor.str());
    stringConstructor.clear();

    for (int i = 0; i < trackingNum * recRatio * 100; ++i) {
        stringConstructor << receiver[i].deducedBit.value();
    }
    // Just find the tracking pattern
    string receiverString(stringConstructor.str());

    auto found = receiverString.find(startPattern);

    if (found != string::npos) {
        return found;
    }

    return -1;
}

vector<TransmitterLog> getTransmitterLogs(const string &fileName, fstream &transmitterLogs) {
    io::LineReader transmitterCSV(fileName, transmitterLogs);

    return vector<TransmitterLog>();
}

vector<ReceiverLog> getReceiverLogs(const string &fileName, fstream &receiverLogs) {
    io::LineReader receiverCSV(fileName, receiverLogs);

    return vector<ReceiverLog>();
}


