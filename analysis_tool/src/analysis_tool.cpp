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
        } else if ((arg == "-o") || (arg == "--output")) {
            if (!app_config.genericOutput.has_value()) {
                // If the output value has not already been declared
                app_config.genericOutput = argv[++i];
            } else {
                cout << "You have attempted to set 2 values of output. Please make up your mind" << endl;
            }
        } else if ((arg == "-s") || (arg == "--dataset")) {
            // Sets a flag telling us to look for the on_100fps and off_100fps files
            // to set a baseline for the dataset
            app_config.app = APP_TYPE::DATASET_ANALYSIS;
        } else if ((arg == "-c") || (arg == "--colour")) {
            app_config.colourSpace = getColourSpace(argv[++i]);
        } else if ((arg == "-n") || (arg == "--nointeract")) {
            app_config.noInteract = true;
        } else {
            cout << "Unknown option: " << argv[i] << endl;
        }
    }
}

optional<cv::ColorConversionCodes> getColourSpace(const string& argv) {
    if ((argv == "gray") || (argv == "GRAY")) return cv::ColorConversionCodes::COLOR_BGR2GRAY;
    if ((argv == "hsl") || (argv == "HSL")) return cv::ColorConversionCodes::COLOR_BGR2HLS;

    return nullopt;
}

/*
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

    cv::namedWindow("source vid", cv::WINDOW_AUTOSIZE);
    cv::imshow("source vid", frame);

    auto roi = cv::selectROI("source vid", frame, true, false);

    cv::Mat roiMask(frame, roi);

    cv::Mat roiMod;

    if (config.colourSpace.has_value()) {
        cv::cvtColor(roiMask, roiMod, config.colourSpace.value());
    } else {
        roiMod = roiMask;
    }

    cv::imshow("roi frame", roiMod);

    cv::waitKey(0);

    double totalFrames = video.get(cv::CAP_PROP_FRAME_COUNT);

    cv::destroyAllWindows();


    for (int i = 0; i < totalFrames; ++i) {
        readSuccess = video.read(frame);

        if (!readSuccess) {
            cout << "Found end of video" << endl;
            break;
        }

        // This should be the ROI mat
        roiMask = frame(roi);

        // Change colour space
        if (config.colourSpace.has_value()) {
            cv::cvtColor(roiMask, roiMod, config.colourSpace.value());
        } else {
            roiMod = roiMask;
        }

        cv::Scalar mean, stdDev;

        cv::meanStdDev(roiMod, mean, stdDev);

        frameMeans.push_back(LogEntry{
            mean,
            stdDev
        });

        // if no interact doesn't have a value
        if (!config.noInteract.has_value()) {
            progressBar((float) i / totalFrames, 30);
        }
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
            }
        }
    } else if (config.app.has_value() && config.app.value() == APP_TYPE::DATASET_ANALYSIS) {
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
        showUsage();
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
    vector<cv::Scalar> logMeans = vector<cv::Scalar>();
    vector<cv::Scalar> logStdDev = vector<cv::Scalar>();
    auto getScalar = [&logMeans, &logStdDev] (const LogEntry& l) {
        logMeans.push_back(l.frameMean);
        logStdDev.push_back(l.frameStdDev);
    };

    // Sets up temporary configuration files
    tempConfig.location = ledON.string();
    tempConfig.genericOutput = replaceExtension(ledON);
    auto scalarAverages = analyseVideo(tempConfig);
    // extract scalar averages from the logs
    for_each(scalarAverages.value().cbegin(), scalarAverages.value().cend(), getScalar);
    auto ledONAverage = getScalarAverage(logMeans);
    auto ledONStdDev = getScalarAverage(logStdDev);

    // reset scalar buffer
    logMeans.clear();
    logStdDev.clear();

    tempConfig.location = ledOFF.string();
    tempConfig.genericOutput = replaceExtension(ledOFF);
    scalarAverages = analyseVideo(tempConfig);
    // extract the scalar averages from the logs
    for_each(scalarAverages.value().cbegin(), scalarAverages.value().cend(), getScalar);
    auto ledOFFAverage = getScalarAverage(logMeans);
    auto ledOFFStdDev = getScalarAverage(logStdDev);

    printVideoDispersion(configuration, ledONAverage, ledONStdDev, "LED ON");
    printVideoDispersion(configuration, ledOFFAverage, ledOFFStdDev, "LED OFF");
}

void printVideoDispersion(Configuration &config, const cv::Scalar &scalarMeans, const cv::Scalar &scalarStdDev,
                          const string &title) {
    printf("%s\n", title.c_str());

    if (config.colourSpace.has_value()) {
        if (config.colourSpace.value() == cv::COLOR_BGR2GRAY) {
            printf("\tGRAY\n"
                   "Mean:\t%.2lf\n"
                   "StdDev:\t%.2lf\n", scalarMeans[0], scalarStdDev[0]);
        }
        if (config.colourSpace.value() == cv::COLOR_BGR2HLS) {
            printf("\tHUE\tLIGHTNESS\tSATURATION\n"
                   "Mean:\t%.2lf\t%.2lf\t%.2lf\n"
                   "StdDev:\t%.2lf\t%.2lf\t%.2lf\n",
                   scalarMeans[0], scalarMeans[1], scalarMeans[2],
                   scalarStdDev[0], scalarStdDev[1], scalarStdDev[2]);
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

void showUsage() {
    // TODO: Fill out help section
    cout << "./analysis_tool -s -f <file_path> -d <folder_path> -o <output_name>" << endl;
    cout << "-s or --dataset\t: Sets the dataset flag and stipulates that the included folder path contains a full "
            "dataset that can be analysed contextually" << endl;
    cout << "-f or --file\t: File path of the avi file you want to analyse" << endl;
    cout << "-d or --folder\t: Path to a folder with svos to be analysed" << endl;
    cout << "-o or --output\t: Generic output name for the generated analysis files" << endl;
    exit(-1);
}
