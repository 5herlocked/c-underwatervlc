//
// Created by sherlock on 17/05/2021.
//

// This entire file has been yoinked from the zed-examples github

#ifndef ANALYSIS_TOOL_UTILS_H
#define ANALYSIS_TOOL_UTILS_H

#pragma once

#include <sys/types.h>
#include <sys/stat.h>

static bool exit_app = false;

// Handle the CTRL-C keyboard signal
#ifdef _WIN32

#include <Windows.h>

void CtrlHandler(DWORD fdwCtrlType) {
    exit_app = (fdwCtrlType == CTRL_C_EVENT);
}

#else
#include <csignal>

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

bool directoryExists(const std::string &directory) {
    struct stat info{};
    if (stat(directory.c_str(), &info) != 0)
        return false;
    else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows
        return true;

    return false;
}

#endif //ANALYSIS_TOOL_UTILS_H