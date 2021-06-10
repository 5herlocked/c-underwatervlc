//
// Created by sherlock on 28/05/2021.
//

#include <iostream>
#include <fstream>
#include <optional>
#include "utils.h"

#include <opencv2/opencv.hpp>

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

enum SOURCE_TYPE {
    SINGLE_VIDEO,
    FOLDER
};

struct LogEntry {
    double deltaTime{};
    cv::Scalar frameAverage{};
    int deducedBit{};
};

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    optional<std::string> location;
    optional<SOURCE_TYPE> source;
    optional<std::string> genericOutput;
};

void parseArgs(int argc, char* argv[], Configuration& config);
void analyseFolder(Configuration& config);
int analyseVideo(Configuration& config);
void createCSV(const vector<LogEntry> &logs, const string &filename);
void showUsage();

int main(int argc, char* argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    // Because of the checks in parseArgs before we come down here, we know all config files are valid
    // So the std::optional is entirely unnecessary anywhere except for parseArgs
    error_code fs_error;

    switch(config.source.value()) {
        case SINGLE_VIDEO:
            // make sure vid exists then send it off to be processed in the proper manner
            if (fs::is_regular_file(config.location.value().c_str(), fs_error)) {
                analyseVideo(config);
                return 0;
            } else {
                // The file does not exist
                cout << config.location.value().c_str() << endl;
                cout << "Error accessing file: " << fs_error.message() << endl;
                return -1;
            }
        case FOLDER:
            // make sure we have access to the folder AND it has at least one .svo file in it
            // then create a list of all svo files in the vid and just export vid with proper config
            if (fs::is_directory(config.location.value().c_str(), fs_error)) {
                analyseFolder(config);
                return 0;
            } else {
                cout << "Error accessing folder: " << fs_error.message() << endl;
                return -1;
            }
    }
}

void parseArgs(int argc, char* argv[], Configuration& app_config) {
    // TODO: Complete parseArgs
    for (int i = 1; i < argc; ++i) {
        // Stores the option
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            exit(0);
        } else if ((arg == "-d") || (arg == "--folder")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::FOLDER;
                app_config.location = argv[++i];
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-f") || (arg == "--file")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::SINGLE_VIDEO;
                fs::path file_path = argv[++i];
                app_config.location = file_path.string();
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-o") || (arg == "--output")) {
            if (!app_config.genericOutput.has_value()) {
                // If the output value has not already been declared
                app_config.genericOutput = argv[++i];
            } else {
                cout << "You have attempted to set 2 values of output. Please make up your mind" << endl;
            }
        } else {
            cout << "Unknown option: <" << argv[i] << endl;
        }
    }
}

int analyseVideo(Configuration &config) {
    cv::VideoCapture video(config.location.value());
    auto frameMeans = vector<LogEntry>();

    if (!video.isOpened()) {
        cout << "Cannot open the video file" << endl;
        exit(-1);
    }

    // Material frames we need
    cv::Mat frame;

    bool readSuccess = video.read(frame);
    if (!readSuccess) {
        cout << "Unable to read the video" << endl;
        return EXIT_FAILURE;
    }

    cv::namedWindow("source vid", cv::WINDOW_AUTOSIZE);
    cv::imshow("source vid", frame);

    cv::Rect roi = cv::selectROI("source vid", frame, true, false);

    cv::Mat roiMask(frame, roi);

    cv::imshow("roi vid", roiMask);

    double fps = video.get(cv::CAP_PROP_FPS);
    double totalFrames = video.get(cv::CAP_PROP_FRAME_COUNT);

    int position = 0;

    while (true) {
        readSuccess = video.read(frame);

        if (!readSuccess) {
            cout << "Found end of video" << endl;
            break;
        }

        // increment position so we can keep track of where we are in dT
        position += 1;

        // This should be the ROI mat
        roiMask = frame(roi);
//        cv::imshow("roi vid", roiMask);

        // TODO: Make a vec of scalars that stores our values, then log them
        // This average will be more blue when the LED is on, and less blue when the LED is off
        cv::Scalar average = cv::mean(roiMask);
        double deltaTime = position/fps;

        // TODO: threshold to find the bit value
        frameMeans.push_back(LogEntry{
            deltaTime,
            average,
            1
        });
        progressBar((float)position / totalFrames, 30);
    }

    createCSV(frameMeans, config.genericOutput.value());

    return 0;
}

// Pre-conditions: command line options are valid, the folder exists.
// All this does is opens the folder, retrieves a list of .svo files
// and then converts each video individually
void analyseFolder(Configuration &config) {
    Configuration tempConfig = config;

    for (const auto& file : fs::directory_iterator(config.location.value().c_str())) {
        if (file.path().extension() == ".avi") {
            optional<std::string> temp = file.path().string();
            optional<std::string> output_val = file.path().filename().replace_extension().string();
            tempConfig.location = temp;
            tempConfig.genericOutput = output_val;
            analyseVideo(tempConfig);
        }
    }
}

void createCSV(const vector<LogEntry> &logs, const string &filename) {
    fstream csvStream;
    csvStream.open(filename, ios::out);

    csvStream << "Delta Time" << "," << "Blue" << "," << "Green" << ","<< "Red" << "," << "Bit" << "\n";

    // frameAverage is of type double[4], we need to destructure it
    for (const LogEntry& entry : logs) {
        csvStream << entry.deltaTime << "," << entry.frameAverage.val[0] << ","<< entry.frameAverage.val[1] << ","<< entry.frameAverage.val[2] << "," << entry.deducedBit << "\n";
    }

    csvStream.close();
}


void showUsage() {
    // TODO: Fill out help section
    cout << "./analysis_tool -f <file_path> -d <folder_path> -o <output_name>" << endl;
    cout << "-f or --file\t: File path of the avi file you want to analyse" << endl;
    cout << "-d or --folder\t: Path to a folder with svos to be analysed" << endl;
    cout << "-o or --output\t: Generic output name for the generated analysis files" << endl;
    exit(-1);
}
