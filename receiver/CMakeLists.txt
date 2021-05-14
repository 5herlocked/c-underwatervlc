cmake_minimum_required(VERSION 3.19)
project(receiver)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/receiver.cpp)