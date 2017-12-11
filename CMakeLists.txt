cmake_minimum_required(VERSION 3.8)
project(p5)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES CPUTimer.h evac.cpp evac.h EvacRunner.cpp EvacRunner.h MyCity.cpp MyCity.h MyRoad.cpp MyRoad.h)
add_executable(p5 ${SOURCE_FILES})