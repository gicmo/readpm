cmake_minimum_required(VERSION 3.0)
project(readpm)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cc patchmaster.c patchmaster.h)
add_executable(readpm ${SOURCE_FILES})