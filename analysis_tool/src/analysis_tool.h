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

struct LogEntry {
    cv::Scalar frameMean{};
    cv::Scalar frameStdDev{};
};

struct Configuration {
    // This should be separated into private variables and public accessor methods but
    // I am becoming a little lazy
    optional<std::string> location;
    optional<SOURCE_TYPE> source;
    optional<APP_TYPE> app;
    optional<std::string> genericOutput;
    optional<cv::ColorConversionCodes> colourSpace;
    optional<bool> noInteract;
};

void parseArgs(int argc, char *argv[], Configuration &config);

optional<cv::ColorConversionCodes> getColourSpace(const string& argv);

void analyseFolder(Configuration &config);

optional<vector<LogEntry>> analyseVideo(Configuration &config, const optional<cv::Scalar> &ledONVal = nullopt,
                                        const optional<cv::Scalar> &ledOFFVal = nullopt);

void analyseDataset(Configuration &configuration, const fs::path &ledON, const fs::path &ledOFF);

void printVideoDispersion(Configuration &config, const cv::Scalar &scalarMeans, const cv::Scalar &scalarStdDev,
                          const string &title);

void showUsage();

cv::Scalar getScalarAverage(const vector<cv::Scalar> &scalars);

optional<std::string> replaceExtension(const fs::path &path);