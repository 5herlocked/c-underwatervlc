//
// Created by sherlock on 12/05/2021.
//

#include "svo_export.h"
#include "iostream"
#include "sstream"
#include "opencv2/opencv.hpp"
#include "utils.h"
#include "sl/Camera.hpp"

using namespace sl;
using namespace std;

enum APP_TYPE {
    FOLDER_OF_VIDS,
    COMPLETE_DECOMPOSITION
};

void print(const string& msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, const string& msg_suffix = "");
void parseArgs(int argc, char* argv[]);
void exportFolder(const string& folderPath);
void exportVideo(const string& videoPath);
void showUsage();

int main(int argc, char* argv[]) {

}

void print(const string& msg_prefix, ERROR_CODE err_code, const string& msg_suffix) {

}

void parseArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            return;
        } else if ((arg == "-r") || (arg == "--resolution")) {
            resStr = argv[i + 1];
        } else if ((arg == "-f") || (arg == "--framerate")) {
            frameRateStr = argv[i + 1];
        } else if ((arg == "-o") || (arg == "--output")) {
            file_name = argv[i + 1];
        } else {
            cout << "Unknown option: <" << argv[i] << endl;
        }
    }
}

void exportFolder(const string &folderPath) {

}

void exportVideo(const string &videoPath) {

}

void showUsage() {

}
