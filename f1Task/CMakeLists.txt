# Sam Lunev 2026. All rights reserved.

cmake_minimum_required(VERSION 3.14)
project(PCTskNSUone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DOUBLE_OPT "Use double instead of float" OFF)

if(DOUBLE_OPT)
    add_definitions(-DDOUBLE_OPT)
endif()

add_executable(execbin src/main.cpp)
