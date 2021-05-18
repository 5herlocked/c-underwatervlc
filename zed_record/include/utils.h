//
// Created by sherlock on 17/05/2021.
//

// This entire file has been yoinked from the zed-examples github

#ifndef ZED_RECORD_UTILS_H
#define ZED_RECORD_UTILS_H

#pragma once

static bool exit_app = false;

// Handle the CTRL-C keyboard signal
#ifdef _WIN32
#include <Windows.h>
void CtrlHandler(DWORD fdwCtrlType) {
    exit_app = (fdwCtrlType == CTRL_C_EVENT);
}

#else
#include <signal.h>
void nix_exit_handler(int s) {
    exit_app = true;
}
#endif

// Set the function to handle the CTRL-C
void SetCtrlHandler() {
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE);

#else // unix
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = nix_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
#endif
}

#endif //ZED_RECORD_UTILS_H
