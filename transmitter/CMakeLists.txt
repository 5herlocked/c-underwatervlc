cmake_minimum_required(VERSION 3.10)
project(transmitter)

set(CMAKE_CXX_STANDARD 17)

find_library(GPIOD_LIBRARY NAMES libgpiod.so)
if(NOT GPIOD_LIBRARY)
    message(FATAL_ERROR "gpiod library not found. Install apt install libgpiod-dev")
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} include/utils.h src/transmitter.cpp)
target_link_libraries(${PROJECT_NAME} ${GPIOD_LIBRARY})