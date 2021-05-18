cmake_minimum_required(VERSION 3.10)
project(svo_export)

set(CMAKE_CXX_STANDARD 20)

find_package(ZED 3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} include/utils.h src/svo_export.cpp)

target_link_libraries(${PROJECT_NAME} ${ZED_LIBS} ${OpenCV_LIBRARIES})