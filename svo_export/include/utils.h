//
// Created by sherlock on 17/05/2021.
//

// This entire file has been yoinked from the zed-examples github

#ifndef SVO_EXPORT_UTILS_H
#define SVO_EXPORT_UTILS_H

#pragma once
#include <sys/types.h>
#include <sys/stat.h>

static bool exit_app = false;

// Handle the CTRL-C keyboard signal
#ifdef _WIN32
#include <Windows.h>
#include <sl/Camera.hpp>

void CtrlHandler(DWORD fdwCtrlType) {
    exit_app = (fdwCtrlType == CTRL_C_EVENT);
}
#else
#include <csignal>
#include <sl/Camera.hpp>

void nix_exit_handler(int s) {
    exit_app = true;
}
#endif

// Set the function to handle the CTRL-C
void SetCtrlHandler() {
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);
#else // unix
    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = nix_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
#endif
}

// Display progress bar
void progressBar(float ratio, unsigned int w) {
    unsigned int c = ratio * w;
    for (unsigned int x = 0; x < c; x++) std::cout << "=";
    for (unsigned int x = c; x < w; x++) std::cout << " ";
    std::cout << (int) (ratio * 100) << "% ";
    std::cout << "\r" << std::flush;
}

// If the current project uses openCV
#if defined (__OPENCV_ALL_HPP__) || defined(OPENCV_ALL_HPP)
// Conversion function between sl::Mat and cv::Mat
cv::Mat slMat2cvMat(sl::Mat &input) {
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
        default: break;
    }
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM::CPU));
}
#endif

bool directoryExists(const std::string& directory) {
    struct stat info{};
    if (stat(directory.c_str(), &info) != 0)
        return false;
    else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows
        return true;

    return false;
}

#endif //SVO_EXPORT_UTILS_H