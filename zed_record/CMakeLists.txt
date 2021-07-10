cmake_minimum_required(VERSION 3.10)
project(zed_record)

option(LINK_SHARED_ZED "Link with the ZED SDK shared executable" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

find_package(ZED 3 REQUIRED)
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)

set(special_os_libs "pthread" "X11")

include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} include/utils.hpp src/zed_record.cpp)

if (LINK_SHARED_ZED)
    set(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED})
else()
    set(ZED_LIBS ${ZED_STATIC_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARY})
endif()

target_link_libraries(${PROJECT_NAME} ${ZED_LIBS} ${OpenCV_LIBRARIES})
