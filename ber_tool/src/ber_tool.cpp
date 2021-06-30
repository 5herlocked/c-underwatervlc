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
#include <charconv>


#include "utils.h"
#include "CSVRow.h"

#if __has_include(<filesystem>)

#include <filesystem>
namespace fs = std::filesystem;

#elif __has_include(<experimental/filesystem>)

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#else
error "Missing the <filesystem> header."
#endif

using namespace std;

struct Configuration {
    optional<string> receiverFile;
    optional<string> transmitterFile;
    int transmitRate{};
    int receiveRate{};
};

void parseArgs(int argc, char *argv[], Configuration &config);

double getBer(fstream &transmitterFile, fstream &receiverFile, int recRatio);

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
    double berValue = getBer(transmitterCSV, receiverCSV, 0);
}

void parseArgs(int argc, char **argv, Configuration &config) {
    std::string_view values = "new stuff";
}

double getBer(fstream &transmitterFile, fstream &receiverFile, int recRatio) {
    CSVIterator<double> transIter(transmitterFile);
    CSVIterator<double> recIter(receiverFile);

    double ber = 0;

    while (transIter != CSVIterator<double>()) {
        int transmittedBit;
        CSVRow<double> curRow = (*transIter);

        auto result = from_chars(curRow[2].data(), curRow[2].data() + curRow[2].size(), transmittedBit);

        if (result.ec == std::errc::invalid_argument) {
            cout << "Could not convert" << endl;
            continue;
        }

        int deducedBits[recRatio];

        // populate the deduced bits
        for (int i = 0; i < recRatio; ++i) {
            CSVRow<double> recCurRow = (*recIter);

        }
    }

    for (CSVIterator<double> transLoop(transmitterFile); transLoop != CSVIterator<double>(); ++transLoop) {
        for (CSVIterator<double> recLoop(receiverFile); recLoop != CSVIterator<double>(); recLoop += recRatio) {
            int deducedBits[recRatio];
            transIter = *transLoop;
            for (int i = 0; i < recRatio; ++i) {
                auto result = from_chars(transIter[3].data(), transIter[3].data() + transIter[3].size(), deducedBit);

                if (result.ec == std::errc::invalid_argument) {
                    cout << "Could not convert" << endl;
                    continue;
                }
                transIter.readNextRow()
            }
        }
    }
    return ber;
}
