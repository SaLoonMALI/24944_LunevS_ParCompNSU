# Sam Lunev. 2026. All Rights Reserved.

cmake_minimum_required(VERSION 3.14)
project(Integrate_OMP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(integrate_omp main.cpp)

target_link_libraries(integrate_omp PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(integrate_omp PRIVATE -fopenmp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(integrate_omp PRIVATE /openmp)
endif()