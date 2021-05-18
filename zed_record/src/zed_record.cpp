//
// Created by sherlock on 12/05/2021.
//

#include "zed_record.h"
#include <sl/Camera.hpp>

#include "utils.h"


using namespace sl;
using namespace std;

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");
void parseArgs(int argc, char **argv, sl::InitParameters& param);