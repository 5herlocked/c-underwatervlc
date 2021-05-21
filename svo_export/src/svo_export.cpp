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

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");

int main(int argc, char* argv[]) {

}