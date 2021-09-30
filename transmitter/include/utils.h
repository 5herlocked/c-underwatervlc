#ifndef TRANSMITTER_UTILS_H
#define TRANSMITTER_UTILS_H

#pragma once
#include "iostream"
#include <sys/types.h>
#include <sys/stat.h>

// Display progress bar
void progressBar(int completed, int failed) {
    std::cout << "Completed: " << completed << "\t Failed: " << failed;
    std::cout << "\r" << std::flush;
}

#endif