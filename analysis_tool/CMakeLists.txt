cmake_minimum_required(VERSION 3.10)
project(analysis_tool)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV 4 REQUIRED)

if (NOT WIN32)
    set(SPECIAL_OS_LIBS "pthread" "X11")
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${OpenCV_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} include/utils.h src/analysis_tool.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})