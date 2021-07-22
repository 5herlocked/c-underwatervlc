#include <iostream>
#include <fstream>
#include <optional>
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

enum APP_TYPE {
    RAW_ANALYSIS,
    DATASET_ANALYSIS
};

struct MouseData {
    int location{};
    cv::Point points[4];
};

struct LogEntry {
    double deltaTime{};
    cv::Scalar frameAverage{};
    optional<int> deducedBit{};
};

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    optional<std::string> location;
    optional<SOURCE_TYPE> source;
    optional<APP_TYPE> app;
    optional<std::string> genericOutput;
};

struct Options {
    string shortOpt{};
    string longOpt{};
    string description{};
    string arguments{};
};

void parseArgs(int argc, char *argv[], Configuration &config);

void analyseFolder(Configuration &config);

optional<vector<LogEntry>> analyseVideo(Configuration &config, const optional<cv::Scalar> &ledONVal = nullopt,
                                        const optional<cv::Scalar> &ledOFFVal = nullopt);

void analyseDataset(Configuration &configuration, const fs::path &ledON, const fs::path &ledOFF);

void createCSV(const vector<LogEntry> &logs, const string &filename);

void showUsage();

cv::Scalar getScalarAverage(const vector<cv::Scalar> &scalars);

optional<std::string> replaceExtension(const fs::path &path);

void capturePointsCallback(int event, int x, int y, int flags, void *userdata);