//
// Created by sherlock on 28/05/2021.
//

#include "utils.h"
#include "analysis_tool.h"

int main(int argc, char *argv[]) {
    Configuration config{};
    parseArgs(argc, argv, config);

    // Because of the checks in parseArgs before we come down here, we know all config files are valid
    // So the std::optional is entirely unnecessary anywhere except for parseArgs
    error_code fs_error;

    switch (config.source.value()) {
        case SINGLE_VIDEO:
            // make sure vid exists then send it off to be processed in the proper manner
            if (fs::is_regular_file(config.location.value().c_str(), fs_error)) {
                auto file = fs::path(config.location.value());
                optional<std::string> output_val = replaceExtension(file);

                auto generatedData = analyseVideo(config);
                if (generatedData.has_value()) {
                    createCSV(generatedData.value(), output_val.value());
                }
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

void parseArgs(int argc, char *argv[], Configuration &app_config) {
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
                if (!app_config.app.has_value()) {
                    app_config.app = APP_TYPE::RAW_ANALYSIS;
                }
                app_config.location = argv[++i];
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-f") || (arg == "--file")) {
            if (!app_config.source.has_value()) {
                // If the source hasn't already been declared
                app_config.source = SOURCE_TYPE::SINGLE_VIDEO;
                app_config.app = APP_TYPE::RAW_ANALYSIS;
                fs::path file_path = argv[++i];
                app_config.location = file_path.string();
            } else {
                cout << "You have attempted to use 2 source flags. Please make up your mind." << endl;
                showUsage();
            }
        } else if ((arg == "-s") || (arg == "--dataset")) {
            // Sets a flag telling us to look for the on_100fps and off_100fps files
            // to set a baseline for the dataset
            app_config.app = APP_TYPE::DATASET_ANALYSIS;
        } else {
            cout << "Unknown option: " << argv[i] << endl;
        }
    }
}

/*
 * TODO: Make analyseVideo a more elegant operation. Currently: it has 2 functions (find a way to unify functions so that all it does is, analyse the video in **one** way)
 * analyseVideo returns the average of the scalar values of the video if using the dataset flag
 *
 * The way forward:
 *  Simplest solution is make analyse video accept vec<cv::Scalar> but that might end up looking a little messy
 *      It should let us reduce the burden on the stack by declaring the variable in the heap and just passing the pointer
 *      But, it could lead to some slowdown
 *  The more tricky solution would be using complex flag structures which would further complicate maintenance
 */
optional<vector<LogEntry>>
analyseVideo(Configuration &config, const optional<cv::Scalar> &ledONVal, const optional<cv::Scalar> &ledOFFVal) {
    cv::VideoCapture video(config.location.value());
    auto frameMeans = vector<LogEntry>();

    MouseData roiBox;

    if (!video.isOpened()) {
        cout << "Cannot open the video file" << endl;
        exit(-1);
    }

    // Material frames we need
    cv::Mat frame;

    bool readSuccess = video.read(frame);
    if (!readSuccess) {
        cout << "Unable to read the video" << endl;
        return {};
    }

    cv::namedWindow("source vid", cv::WINDOW_FREERATIO);
    cv::imshow("source vid", frame);

    auto roi = cv::selectROI("source vid", frame, true, false);

    cv::Mat roiMask(frame, roi);

    cv::imshow("roi vid", roiMask);

    double fps = video.get(cv::CAP_PROP_FPS);
    double totalFrames = video.get(cv::CAP_PROP_FRAME_COUNT);

    int position = 0;

    cv::destroyAllWindows();

    for (int i = 0; i < totalFrames; ++i) {
        readSuccess = video.read(frame);

        if (!readSuccess) {
            cout << "Found end of video" << endl;
            break;
        }

        // This should be the ROI mat
        roiMask = frame(roi);

        cv::Scalar average = cv::mean(roiMask);
        double deltaTime = (double)i / fps;
        optional<int> deducedBit = nullopt;

        // TODO: Make the threshold logic smart
        if (ledONVal.has_value() && ledOFFVal.has_value()) {
            // It's a dataset
            // find diff between ledON and ledOFF
            // split the diff to get threshold?
            // deducedBit = average > threshold ? 1 : average < threshold ? 0 : nullopt;
            auto threshold = getScalarAverage({ ledONVal.value(), ledOFFVal.value() });

            // Refer only to the B of the BRG values
            if (average[0] > threshold[0]) {
                deducedBit = 1;
            } else if (average[0] < threshold[0]) {
                deducedBit = 0;
            } else {
                deducedBit = nullopt;
            }
        }

        frameMeans.push_back(LogEntry{
                deltaTime,
                average,
                deducedBit
        });
        progressBar((float) position / totalFrames, 30);
    }

    video.release();

    return frameMeans;
}

// Pre-conditions: command line options are valid, the folder exists.
// All this does is opens the folder, retrieves a list of .svo files
// and then converts each video individually
void analyseFolder(Configuration &config) {
    Configuration tempConfig = config;

    if (config.app.has_value() && config.app.value() == APP_TYPE::RAW_ANALYSIS) {
        for (const auto &file : fs::directory_iterator(config.location.value().c_str())) {
            if (file.path().extension() == ".avi") {
                optional<std::string> temp = file.path().string();
                optional<std::string> output_val = replaceExtension(file.path());
                tempConfig.location = temp;
                tempConfig.genericOutput = output_val;

                auto generatedData = analyseVideo(tempConfig);
                if (generatedData.has_value()) {
                    createCSV(generatedData.value(), tempConfig.genericOutput.value());
                }
            }
        }
    } else if (config.app.has_value() && config.app.value() == APP_TYPE::DATASET_ANALYSIS) {
        // TODO: Dataset analysis
        fs::path ledOnFile;
        fs::path ledOffFile;

        for (const auto &file : fs::directory_iterator(config.location.value().c_str())) {
            if (file.path().has_extension() && file.path().extension() == ".avi") {
                const string &filename = file.path().filename().string();
                if (filename.find("on") != string::npos) {
                    ledOnFile = file.path();
                } else if (filename.find("off") != string::npos) {
                    ledOffFile = file.path();
                }
            }
        }

        analyseDataset(config, ledOnFile, ledOffFile);
    } else {
        cout << "Congratulations, you have done something mathematically impossible. You must be proud." << endl;
        cout << "Now go fix your options" << endl;
        exit(-1);
    }
}

optional<std::string> replaceExtension(const fs::path &path) {
    if (path.empty()) {
        cout << "Path: " << path.string() << " is empty" << endl;
        return nullopt;
    }
    return path.filename().replace_extension().string();
}

void analyseDataset(Configuration &configuration, const fs::path &ledON, const fs::path &ledOFF) {
    // setup internal variables
    auto tempConfig = configuration;
    vector<cv::Scalar> scalarEntries = vector<cv::Scalar>();
    auto getScalar = [&scalarEntries] (const LogEntry& l) { scalarEntries.push_back(l.frameAverage); };

    // Sets up temporary configuration files
    tempConfig.location = ledON.string();
    tempConfig.genericOutput = replaceExtension(ledON);
    auto scalarAverages = analyseVideo(tempConfig);
    createCSV(scalarAverages.value(), ledON.filename().replace_extension().string());
    // extract scalar averages from the logs
    for_each(scalarAverages.value().cbegin(), scalarAverages.value().cend(), getScalar);
    auto ledONAverage = getScalarAverage(scalarEntries);

    // empty scalar entries
    scalarEntries.clear();

    tempConfig.location = ledOFF.string();
    tempConfig.genericOutput = replaceExtension(ledOFF);
    scalarAverages = analyseVideo(tempConfig);
    createCSV(scalarAverages.value(), ledOFF.filename().replace_extension().string());
    // extract the scalar averages from the logs
    for_each(scalarAverages.value().cbegin(), scalarAverages.value().cend(), getScalar);
    auto ledOFFAverage = getScalarAverage(scalarEntries);

    // iterate through the rest of the directory
    for (const auto &file : fs::directory_iterator(configuration.location.value().c_str())) {
        // Ignore the ON, OFF files
        if (file.path().filename().string().find("on") != string::npos ||
            file.path().filename().string().find("off") != string::npos) {
            continue;
        } else if (file.path().extension() == ".avi") {
            optional<std::string> temp = file.path().string();
            optional<std::string> output_val = replaceExtension(file.path());
            tempConfig.location = temp;
            tempConfig.genericOutput = output_val;

            auto generatedData = analyseVideo(tempConfig, ledONAverage, ledOFFAverage);
            if (generatedData.has_value()) {
                createCSV(generatedData.value(), tempConfig.genericOutput.value());
            }
        }
    }
}

/*
 * Iterative average akin to: m_n = i/n * Sum_(i=1)^n a_i
 * It's to prevent overflow (precaution)
 * numbers we're normally dealing with range from 0-255
 * but factor in ~ 30,000 entries leads to a max of 7,650,000 (within range of double)
 * but better have an infinitely extensible bit of code rather running into an integer overflow issue
 * which is tough to detect even in C++
 *
 * Returns [0, 0, 0, 0] if we run into logs that don't exist
 * Else returns [mB, mR, mG, 255]
 * The final channel is A (alpha) which is not used in our videos or analysis
 */
cv::Scalar getScalarAverage(const vector<cv::Scalar> &scalars) {
    int t = 1;

    auto meanStore = cv::Scalar();

    for (cv::Scalar l : scalars) {
        for (int i = 0; i < l.channels; ++i) {
            meanStore[i] += (l[i] - meanStore[i]) / t;
        }
        ++t;
    }

    return meanStore;
}

void createCSV(const vector<LogEntry> &logs, const string &filename) {
    fstream csvStream;
    csvStream.open(filename + ".csv", ios::out);

    csvStream << "deltaTime" << "," << "blue" << "," << "green" << "," << "red" << "," << "bit" << "\n";

    // frameAverage is of type double[4], we need to destructure it
    for (const LogEntry &entry : logs) {
        if (entry.deducedBit.has_value()) {
            csvStream << entry.deltaTime << "," << entry.frameAverage.val[0] << "," << entry.frameAverage.val[1] << ","
                      << entry.frameAverage.val[2] << "," << entry.deducedBit.value() << "\n";
        } else {
            csvStream << entry.deltaTime << "," << entry.frameAverage.val[0] << "," << entry.frameAverage.val[1] << ","
                      << entry.frameAverage.val[2] << ", N/A" << "\n";
        }
    }

    csvStream.close();
}

void showUsage() {
    cout << "./analysis_tool -s -f <file_path> -d <folder_path> -o <output_name>" << endl;
    cout << "-s or --dataset\t: Sets the dataset flag and stipulates that the included folder path contains a full "
            "dataset that can be analysed contextually" << endl;
    cout << "-f or --file\t: File path of the avi file you want to analyse" << endl;
    cout << "-d or --folder\t: Path to a folder with svos to be analysed" << endl;
    exit(-1);
}
