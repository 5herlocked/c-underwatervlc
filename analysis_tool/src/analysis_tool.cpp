//
// Created by sherlock on 28/05/2021.
//

#include <iostream>
#include <optional>
#include <cstdlib>
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

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    optional<std::string> location;
    optional<SOURCE_TYPE> source;
    optional<std::string> genericOutput;
};

[[maybe_unused]] void print(const string& msg_prefix, sl::ERROR_CODE err_code = sl::ERROR_CODE::SUCCESS, const string& msg_suffix = "");
void parseArgs(int argc, char* argv[], Configuration& config);
void analyseFolder(Configuration& config);
int analyseVideo(Configuration& config);
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

[[maybe_unused]] void print(const string& msg_prefix, sl::ERROR_CODE err_code, const string& msg_suffix) {
    if (err_code != sl::ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout<<" ";
    cout << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
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
                app_config.genericOutput = file_path.replace_extension().string();
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

    if (!video.isOpened()) {
        cout << "Cannot open the video file" << endl;
        exit(-1);
    }

    double fps = video.get(cv::CAP_PROP_FPS);
    // TODO: use fps
    cv::InputArray lower_blue = cv::InputArray(std::vector({110, 50, 50}));
    cv::InputArray upper_blue = cv::InputArray(std::vector({130, 255, 255}));


    while (true) {
        // Material frames we need
        cv::Mat frame;
        cv::Mat hsv_frame;
        cv::Mat frame_mask;
        cv::Mat blue_objects;

        bool readSuccess = video.read(frame);
        if (!readSuccess) {
            cout << "Found end of video" << endl;
            break;
        }

        cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);
        cv::inRange(hsv_frame, lower_blue, upper_blue, frame_mask);
        cv::bitwise_and(frame, frame, frame_mask);

        cv::imshow("frame", frame);
        cv::imshow("mask", frame_mask);
        cv::imshow("final", blue_objects);
    }

    return 0;
}

// Pre-conditions: command line options are valid, the folder exists.
// All this does is opens the folder, retrieves a list of .svo files
// and then converts each video individually
void analyseFolder(Configuration &config) {
    Configuration tempConfig = config;

    for (const auto& file : fs::directory_iterator(config.location.value().c_str())) {
        if (file.path().extension() == ".svo") {
            optional<std::string> temp = file.path().string();
            optional<std::string> output_val = file.path().filename().replace_extension().string();
            tempConfig.location = temp;
            tempConfig.genericOutput = output_val;
            analyseVideo(tempConfig);
        }
    }
}

void showUsage() {
    // TODO: Fill out help section
    exit(-1);
}