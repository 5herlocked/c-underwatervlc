cmake_minimum_required(VERSION 3.10)
project(ber_tool)

set(CMAKE_CXX_STANDARD 17)

if (NOT WIN32)
    set(SPECIAL_OS_LIBS "pthread")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} include/utils.h src/ber_tool.cpp include/csv.h)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()