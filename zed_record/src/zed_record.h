//
// Created by sherlock on 12/05/2021.
//
#pragma once
#include "sys/types.h"
#include "sys/stat.h"
#include "csignal"

static bool exit_app = false;

void nix_exit_handler(int s) {
    exit_app = true;
}

void SetCtrlHandler () {
#ifdef __linux__
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = nix_exit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
#endif
}

#ifndef C_UNDERWATERVLC_ZED_RECORD_H
#define C_UNDERWATERVLC_ZED_RECORD_H


class zed_record {

};


#endif //C_UNDERWATERVLC_ZED_RECORD_H
