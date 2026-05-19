cmake_minimum_required(VERSION 3.10)
project(Parallel)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(parallel main.cpp)

target_link_libraries(parallel PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(parallel PRIVATE -fopenmp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(parallel PRIVATE /openmp)
endif()