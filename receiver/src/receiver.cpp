//
// Created by sherlock on 12/05/2021.
//

#include <string>
#include <optional>
#include <vector>
#include "receiver.h"

using namespace std;

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    optional<std::string> location;
    optional<std::string> genericOutput;
};

struct Log {
	time_t time;
	// TODO: Find the appropriate integer size for the bit values
};

void parseArgs(int argc, char* argv[], Configuration& config);
void writeLogs(std::vector<Log> logs, std::string log_name);
void showUsage();

int main(int argc, char* argv[]) {

}