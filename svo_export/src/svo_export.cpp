//
// Created by sherlock on 12/05/2021.
//

#include <iostream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "utils.h"
#include <sl/Camera.hpp>
#include <optional>
#include <cstdlib>

#if __has_include(<filesystem>)
#include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
error "Missing the <filesystem> header."
#endif

using namespace sl;
using namespace std;

enum SOURCE_TYPE {
    SINGLE_VIDEO,
    FOLDER
};

enum APP_TYPE {
    VIDEO_ONLY,
    COMPLETE_DECOMPOSITION,
};

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    string location;
    optional<SOURCE_TYPE> source;
    optional<APP_TYPE> type;
    string genericOutput;
};

void print(const string& msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, const string& msg_suffix = "");
void parseArgs(int argc, char* argv[], Configuration& config);
void exportFolder(Configuration& config);
void exportVideo(Configuration& config, const optional<string>& filename = nullopt);
bool fileExists(const fs::path &path, fs::file_status status = fs::file_status{});
void showUsage();

int main(int argc, char* argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    // Because of the checks in parseArgs before we come down here, we know all config files are valid
    // So the stD::optional is entirely unnecessary anywhere except for parseArgs
    switch(config.source.value()) {
        case SINGLE_VIDEO:
            // make sure vid exists then send it off to be processed in the proper manner
            if (fileExists(config.location)) {
                exportVideo(config);
                return 0;
            } else {
                // The file does not exist
                cout << "The file " << config.location << " does not exist" << endl;
                return -1;
            }
        case FOLDER:
            // make sure we have access to the folder AND it has at least one .svo file in it
            // then create a list of all svo files in the vid and just export vid with proper config
            if (fileExists(config.location)) {
                exportFolder(config);
                return 0;
            } else {
                cout << "The folder " << config.location << " does not exist" << endl;
                return -1;
            }
    }
}

void print(const string& msg_prefix, ERROR_CODE err_code, const string& msg_suffix) {
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout<<" ";
    cout << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

void parseArgs(int argc, char* argv[], Configuration& app_config) {
    // TODO: Convert this to use getopt
    // It's included in stdlib.h just so we know
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            return;
        } else if ((arg == "-v") || (arg == "--video")) {
            if (!app_config.type.has_value()) {
                // If the type of the app hasn't already been set
                app_config.type = APP_TYPE::VIDEO_ONLY;
            } else {
                cout << "You have attempted to use 2 type flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-a") || (arg == "--all")) {
            if (!app_config.type.has_value()) {
                // If the type of app hasn't already been set
                app_config.type = APP_TYPE::COMPLETE_DECOMPOSITION;
            } else {
                cout << "You have attempted to use 2 type flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-d") || (arg == "--folder")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::FOLDER;
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-f") || (arg == "--file")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::FOLDER;
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-o") || (arg == "--output")) {

        } else {
            cout << "Unknown option: <" << argv[i] << endl;
        }
        // TODO: Add config checking to verify an appropriate configuration has been generated
    }
}

// Pre-condition: command line options are valid, the file exists and is of the type svo
// All this does is instantiates a video writer and writes out files based on the configuration
void exportVideo(Configuration &config, const optional<string>& filename) {
    if (filename.has_value()) {
        // This means we're passing a specific filename
        // We need to use the configuration options with the given filenames
    } else {
        // This means we're not passing a specific filename
        // We need to fetch it from the configuration
    }
}

// Pre-conditions: command line options are valid, the folder exists.
// All this does is opens the folder, retrieves a list of .svo files
// and then converts each video individually
void exportFolder(Configuration &config) {

}

void showUsage() {
    cout << "./svo_export -a -v -f <file_path> -o <output_name>" << endl;
    cout << "-a or -all\t: Use this if you want the video, png sequences and depth sequences" << endl;
    cout << "-v or --video\t: Use this if you want the video only" << endl;
    cout << "-f or --file\t: File path of the svo file to export" << endl;
    cout << "-o or --output\t: Generic output name for the generated files" << endl;
    cout << "-f or --folder\t: Path to a folder with svos to be exported" << endl;

    exit(-1);
}

bool fileExists(const fs::path &path, fs::file_status status) {
    if (fs::exists(path))
        return true;

    return false;
}
