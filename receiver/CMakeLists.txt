cmake_minimum_required(VERSION 3.10)
project(receiver)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/sys)

add_executable(${PROJECT_NAME} include/utils.h
        include/serialib.h include/serialib.cpp
        src/receiver.cpp src/receiver.h
        )