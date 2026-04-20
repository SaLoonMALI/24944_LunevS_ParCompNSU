# Sam Lunev. 2026. All Rights Reserved.

cmake_minimum_required(VERSION 3.14)
project(Jacobi_OMP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(j1_omp mod1.cpp)
add_executable(j2_omp mod2.cpp)
add_executable(j1s_omp mod1_.cpp)
add_executable(j2s_omp mod2_.cpp)

target_link_libraries(j1_omp PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(j2_omp PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(j1s_omp PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(j2s_omp PRIVATE OpenMP::OpenMP_CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(j1_omp PRIVATE -fopenmp)
    target_compile_options(j2_omp PRIVATE -fopenmp)
    target_compile_options(j1s_omp PRIVATE -fopenmp)
    target_compile_options(j2s_omp PRIVATE -fopenmp)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(j1_omp PRIVATE /openmp)
    target_compile_options(j2_omp PRIVATE /openmp)
    target_compile_options(j1s_omp PRIVATE /openmp)
    target_compile_options(j2s_omp PRIVATE /openmp)
endif()