# Sam Lunev. 2026. All Rights Reserved.

cmake_minimum_required(VERSION 3.10)
project(ParallelMatrixVector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(parallel_matrix_vector main.cpp)

target_link_libraries(parallel_matrix_vector PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(parallel_matrix_vector PRIVATE -fopenmp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(parallel_matrix_vector PRIVATE /openmp)
endif()