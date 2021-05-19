//
// Created by sherlock on 12/05/2021.
//

#include "zed_record.h"
#include <time.h>
#include <sl/Camera.hpp>
#include <stdio.h>
#include <iostream>
#include <getopt.h>

#include "utils.h"


using namespace sl;
using namespace std;

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");
void parseArgs(int argc, char* argv[], InitParameters& param);
void getResolution(string resolutionArg, InitParameters& param);
void getFrameRate(string frameRateArg, InitParameters& param);

int main(int argc, char* argv[]) {
    Camera zed;

    InitParameters init;
    init.camera_resolution = RESOLUTION::VGA;
    init.camera_fps = 100;
    init.depth_mode = DEPTH_MODE::ULTRA;
    init.coordinate_units = UNIT::MILLIMETER;
    init.coordinate_system = COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;

    parseArgs(argc, argv, init);
}

void print(string msg_prefix, ERROR_CODE err_code, string msg_suffix) {
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

void parseArgs(int argc, char* argv[], InitParameters& param) {
    if (argc == 1) {
        cout << "Using default values of WVGA@100fps with the filename being " << ctime(&time(0)) << ".svo"
    }

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage();
            return;
        }
        else if ((arg == "-r") || (arg == "--resolution")) {
            getResolution(argv[i + 1], param)
        }
        else if ((arg == "-f") || (arg == "--framerate")) {
            getFrameRate(argv[i+1], param)
        }
    }
}

void getResolution(string resolutionArg, InitParameters& param) {
    if (param.camera_fps == null) {
        // feel free to set the resolution as we see fit
        if ((resolutionArg == "VGA")) {

        }
    }
    else {
        // resolutions possible are restricted by the framerate
    }
}

void getFrameRate(string frameRateArg, InitParameters& param) {
    int frameRateNum;
    try {
        frameRateNum = std::atoi(frameRateArg.c_str());
    } catch (std::invalid_argument arg) {
        cout << "Looks like the framerate you provided is not a number" << endl;
    }

    if (param.camera_resolution == null) {
        // feel free to set the framerate as we see fit
        switch (frameRateNum) {
            case 15:
                param.camera_fps = 15;
                break;
            case 30:
                param.camera_fps = 30;
                break;
            case 60:
                param.camera_fps = 60;
                break;
            case 100:
                param.camera_fps = 100;
                break;
            default:
                cout << "Frame rate chosen was not from an acceptable value set." << endl;
                cout << "Setting it to the default framerate of your chosen resolution" << endl;
                break;
        }
    }
    else {
        // possible framerates are limited by resolution choice
        switch(param.camera_resolution) {
            case RESOLUTION::VGA:
                // acceptable framerates 15, 30, 60, 100
                if (frameRateNum == 15) {
                    param.camera_fps = 15;
                }
                else if (frameRateNum == 30) {
                    param.camera_fps = 30;
                }
                else if (frameRateNum == 60) {
                    param.camera_fps = 60;
                }
                else {
                    param.camera_fps = 100;
                }
                break;
            case RESOLUTION::HD720:
                // acceptable framerates 15, 30, 60
                if (frameRateNum == 15) {
                    param.camera_fps = 15;
                }
                else if (frameRateNum == 30) {
                    param.camera_fps = 30;
                }
                else {
                    param.camera_fps = 60;
                }
                break;
            case RESOLUTION::HD1080:
                // acceptable framerates 15, 30
                if (frameRateNum == 15) {
                    param.camera_fps = 15;
                }
                else {
                    param.camera_fps = 30;
                }
                break;
            case RESOLUTION:HD2K:
                // acceptable framerate 15
                param.camera_fps = 15;
                break;
        }
    }
}