//
// Created by sherlock on 12/05/2021.
//

#include <ctime>
#include <sl/Camera.hpp>
#include <utils.hpp>


using namespace std;

struct Configuration {
    sl::InitParameters init;
    int frameRate{};
    string fileName{};
};

void showUsage();

void print(const string &msg_prefix, sl::ERROR_CODE err_code = sl::ERROR_CODE::SUCCESS, const string &msg_suffix = "");

void parseArgs(int argc, char *argv[], Configuration &config);

int getValidFrameRate(sl::RESOLUTION resolution, int rate);

int record(const Configuration &appConfig);

int main(int argc, char *argv[]) {
    Configuration config;
    parseArgs(argc, argv, config);

    // verify arguments?

    return record(config);
}

int record(const Configuration &appConfig) {
    sl::Camera zed;

    auto returnedState = zed.open(appConfig.init);

    if (returnedState != sl::ERROR_CODE::SUCCESS) {
        print("Camera Open Failed", returnedState, "Exit Program.");
        return EXIT_FAILURE;
    }

    returnedState = zed.enableRecording(sl::RecordingParameters(appConfig.fileName.c_str()));

    if (returnedState != sl::ERROR_CODE::SUCCESS) {
        print("Recording ZED failed", returnedState, "Exiting Program");
        zed.close();
        return EXIT_FAILURE;
    }

    printf("SVO is recording, use CTRL+C to stop.\n");
    SetCtrlHandler();
    int framesRecorded = 0, framesFailed = 0;
    sl::RecordingStatus recStatus;

    while (!exit_app) {
        if (zed.grab() == sl::ERROR_CODE::SUCCESS) {
            recStatus = zed.getRecordingStatus();
            if (recStatus.status) {
                framesRecorded++;
            } else {
                framesFailed++;
            }
            ProgressUpdater(framesRecorded, framesFailed);
        }
    }

    zed.disableRecording();
    zed.close();
    return EXIT_SUCCESS;
}

void print(const string &msg_prefix, sl::ERROR_CODE err_code, const string &msg_suffix) {
    cout << "[Sample]";
    if (err_code != sl::ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout << " ";
    cout << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

void parseArgs(int argc, char *argv[], Configuration &appConfig) {
    time_t tt;
    struct tm *ti;
    time(&tt);
    ti = localtime(&tt);

    if (argc == 1) {
        cout << "Using default values of WVGA@100fps with the filename being " << asctime(ti) << ".svo" << endl;

        appConfig.fileName = asctime(ti);
    }

    string frameRateStr;
    string resStr;

    // TODO: Add error checking in each of the if branches
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            showUsage();
            return;
        } else if ((arg == "-r") || (arg == "--resolution")) {
            resStr = argv[++i];
        } else if ((arg == "-f") || (arg == "--framerate")) {
            frameRateStr = argv[++i];
        } else if ((arg == "-o") || (arg == "--output")) {
            appConfig.fileName = argv[++i];
        } else {
            cout << "Unknown option: <" << argv[i] << endl;
        }
    }

    int frameRate = stoi(frameRateStr, nullptr, 10);
    if (resStr == "WVGA" || resStr == "wvga") {
        appConfig.init.camera_resolution = sl::RESOLUTION::VGA;
        appConfig.init.camera_fps = getValidFrameRate(appConfig.init.camera_resolution, frameRate);
    } else if (resStr == "HD" || resStr == "hd") {
        appConfig.init.camera_resolution = sl::RESOLUTION::HD720;
        appConfig.init.camera_fps = getValidFrameRate(appConfig.init.camera_resolution, frameRate);
    } else if (resStr == "FULLHD" || resStr == "fullhd") {
        appConfig.init.camera_resolution = sl::RESOLUTION::HD1080;
        appConfig.init.camera_fps = getValidFrameRate(appConfig.init.camera_resolution, frameRate);
    } else if (resStr == "ULTRAHD" || resStr == "ultrahd") {
        appConfig.init.camera_resolution = sl::RESOLUTION::HD2K;
        appConfig.init.camera_fps = getValidFrameRate(appConfig.init.camera_resolution, frameRate);
    }
}

int getValidFrameRate(sl::RESOLUTION resolution, int rate) {
    switch (resolution) {
        case sl::RESOLUTION::VGA:
            // acceptable framerates 15, 30, 60, 100
            if (rate == 15) {
                return 15;
            } else if (rate == 30) {
                return 30;
            } else if (rate == 60) {
                return 60;
            } else {
                return 100;
            }
        case sl::RESOLUTION::HD720:
            // acceptable framerates 15, 30, 60
            if (rate == 15) {
                return 15;
            } else if (rate == 30) {
                return 30;
            } else {
                return 60;
            }
        case sl::RESOLUTION::HD1080:
            // acceptable framerates 15, 30
            if (rate == 15) {
                return 15;
            } else {
                return 30;
            }
        case sl::RESOLUTION::HD2K:
            // acceptable framerate 15
            return 15;
        case sl::RESOLUTION::LAST:
            break;
    }
}

void showUsage() {
    cout << "./zed_record -r <resolution> -f <frameRate> -o <output_name>" << endl;
    cout << "-r or --resolution\t: Define the resolution of the recording. Accepted values: WVGA, HD, FULLHD, 4K" << endl;
    cout << "-f or --framerate\t: Define the framerate of the recording. Limited by the resolution. Accepted values are 15, 30, 60, 100" << endl;
    cout << "-o or --output\t: Define the output name of the file. Limited only by the filesystem" << endl;

    exit(-1);
}
