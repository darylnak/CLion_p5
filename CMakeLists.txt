cmake_minimum_required(VERSION 3.8)
project(p5)

set(CMAKE_CXX_STANDARD 98)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -g -ansi")

set(SOURCE_FILES CPUTimer.h evac.cpp evac.h EvacRunner.cpp EvacRunner.h)
add_executable(p5 ${SOURCE_FILES})