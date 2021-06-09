cmake_minimum_required(VERSION 3.10)
project(c_underwatervlc)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("receiver")
add_subdirectory("transmitter")
add_subdirectory("analysis_tool")