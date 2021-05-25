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
    optional<String> location;
    optional<SOURCE_TYPE> source;
    optional<APP_TYPE> type;
    optional<String> genericOutput;
};

void print(const string& msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, const string& msg_suffix = "");
void parseArgs(int argc, char* argv[], Configuration& config);
void exportFolder(Configuration& config);
int exportVideo(Configuration& config);
void showUsage();

int main(int argc, char* argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    // Because of the checks in parseArgs before we come down here, we know all config files are valid
    // So the std::optional is entirely unnecessary anywhere except for parseArgs
    error_code fs_error;
    // Here, we have a definite configuration file
    switch(config.source.value()) {
        case SINGLE_VIDEO:
            // make sure vid exists then send it off to be processed in the proper manner
            if (fs::is_regular_file(config.location.value().c_str(), fs_error)) {
                exportVideo(config);
                return 0;
            } else {
                // The file does not exist
                cout << "Error accessing file: " << fs_error.message() << endl;
                return -1;
            }
        case FOLDER:
            // make sure we have access to the folder AND it has at least one .svo file in it
            // then create a list of all svo files in the vid and just export vid with proper config
            if (fs::is_directory(config.location.value().c_str(), fs_error)) {
                exportFolder(config);
                return 0;
            } else {
                cout << "Error accessing folder: " << fs_error.message() << endl;
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
    for (int i = 1; i < argc; ++i) {
        // Stores the option
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            exit(0);
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
                app_config.location = argv[i+1];
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-f") || (arg == "--file")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::SINGLE_VIDEO;
                app_config.location = argv[i+1];
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-o") || (arg == "--output")) {
            if (!app_config.genericOutput.has_value()) {
                // If the output value has not already been declared
                app_config.genericOutput = argv[i+1];
            } else {
                cout << "You have attempted to set 2 values of output. Please make up your mind" << endl;
            }
        } else {
            cout << "Unknown option: <" << argv[i] << endl;
        }
    }
}

// Pre-condition: command line options are valid, the file exists and is of the type svo
// All this does is instantiates a video writer and writes out files based on the configuration
int exportVideo(Configuration &config) {
    // Open Camera
    Camera zed;

    InitParameters init;
    init.input.setFromSVOFile(config.location.value());
    init.coordinate_system = COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;
    init.coordinate_units = UNIT::MILLIMETER;
    init.depth_mode = DEPTH_MODE::ULTRA;

    Resolution imageSize = zed.getCameraInformation().camera_configuration.resolution;

    Mat leftImage(imageSize, MAT_TYPE::U8_C4);
    cv::Mat leftImageOcv = slMat2cvMat(leftImage);

    Mat rightImage(imageSize, MAT_TYPE::U8_C4);
    cv::Mat rightImageOcv = slMat2cvMat(rightImage);

    Mat depthImage(imageSize, MAT_TYPE::F32_C1);
    cv::Mat depthImageOcv = slMat2cvMat(depthImage);

    cv::Mat imageSideByside = cv::Mat(imageSize.height, imageSize.width * 2, CV_8UC3);

    cv::VideoWriter videoWriter;
    int fourcc = cv::VideoWriter::fourcc('M', '4', 'S', '2'); // MP4 part 2 codec

    int frameRate = zed.getInitParameters().camera_fps;
    videoWriter.open(config.genericOutput.value().c_str(), fourcc, frameRate, cv::Size(imageSize.width * 2, imageSize.height));

    if (!videoWriter.isOpened()) {
        print("Error: OpenCV video writer cannot be opened. Please check the .avi file path and write permissions.");
        zed.close();
        return EXIT_FAILURE;
    }

    RuntimeParameters param;
    param.sensing_mode = SENSING_MODE::FILL;

    // Start SVO conversion to AVI/SEQUENCE
    print("Converting SVO... Use Ctrl-C to interrupt conversion.");
    cout << "Exporting: " << config.location.value() << endl;

    int numFrames = zed.getSVONumberOfFrames();
    int svoPos = 0;
    zed.setSVOPosition(svoPos);

    SetCtrlHandler();

    while(!exit_app) {
        sl::ERROR_CODE err = zed.grab(param);
        // The SVO grab was successful
        if (err == ERROR_CODE::SUCCESS) {
            svoPos = zed.getSVOPosition();

            zed.retrieveImage(leftImage, VIEW::LEFT);

            switch (config.type.value()) {
                case VIDEO_ONLY:
                    zed.retrieveImage(rightImage, VIEW::RIGHT);
                    break;
                case COMPLETE_DECOMPOSITION:
                    zed.retrieveImage(rightImage, VIEW::RIGHT);
                    zed.retrieveMeasure(depthImage, MEASURE::DEPTH);
                    break;
            }

            // Convert SVO image from RGBA to RGB
            cv::cvtColor(leftImageOcv, imageSideByside(cv::Rect(0, 0, imageSize.width, imageSize.height)), cv::COLOR_BGRA2BGR);
            cv::cvtColor(rightImageOcv, imageSideByside(cv::Rect(imageSize.width, 0, imageSize.width, imageSize.height)), cv::COLOR_BGRA2BGR);
            // Write the RGB image in the video
            videoWriter.write(imageSideByside);
            if (config.type == APP_TYPE::COMPLETE_DECOMPOSITION) {
                ostringstream filename1;
                filename1 << config.genericOutput.value() << setfill('0') << setw(6) << svoPos << ".png";
                ostringstream filename2;
                filename2 << config.genericOutput.value() << "_depth" << setw(6) << svoPos << ".png";

                cv::imwrite(filename1.str(), imageSideByside);
                cv::Mat depth16;
                depthImageOcv.convertTo(depth16, CV_16UC1);
                cv::imwrite(filename2.str(), depth16);
            }
        } else if (err == ERROR_CODE::END_OF_SVOFILE_REACHED) {
            print("SVO end has been reached. Exiting now.");
            exit_app = true;
        } else {
            print("Grab Error: ", err);
            exit_app = true;
        }
    }

    videoWriter.release();
    zed.close();
    return EXIT_SUCCESS;
}

// Pre-conditions: command line options are valid, the folder exists.
// All this does is opens the folder, retrieves a list of .svo files
// and then converts each video individually
void exportFolder(Configuration &config) {
    Configuration tempConfig = config;

    for (const auto& file : fs::directory_iterator(config.location.value().c_str())) {
        if (file.path().extension() == ".svo") {
            tempConfig.location = file.path().c_str();
            exportVideo(tempConfig);
        }
    }
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
