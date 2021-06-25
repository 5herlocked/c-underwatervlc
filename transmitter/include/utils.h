#ifndef TRANSMITTER_UTILS_H
#define TRANSMITTER_UTILS_H

#pragma once
#include <sys/types.h>
#include <sys/stat.h>

// Display progress bar
void progressBar(float ratio, unsigned int w, int completed, int failed) {
    unsigned int c = ratio * w;
    for (unsigned int x = 0; x < c; x++) std::cout << "=";
    for (unsigned int x = c; x < w; x++) std::cout << " ";
    std::cout << (int) (ratio * 100) << "% " << std::endl;
    std::cout << "Completed: " << completed << "\t Failed: " << failed;
    std::cout << "\r" << std::flush;
}

#endif