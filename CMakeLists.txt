cmake_minimum_required(VERSION 3.19)
project(c_underwatervlc)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("receiver")
add_subdirectory("svo_export")
add_subdirectory("transmitter")
add_subdirectory("zed_record")