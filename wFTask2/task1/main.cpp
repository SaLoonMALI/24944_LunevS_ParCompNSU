// Sam Lunev. 2026. All Rights Reserved.

#include <iostream>
#include <cstdio>
#include <omp.h>
#include <fstream>
#include <chrono>
#include <cmath>
#include <cstdlib>   // for std::atoi

#define VEC_SIZE 10000

int main(int argc, char **argv)
{
    int num_threads = 1;
    int size = VEC_SIZE;

    if (argc >= 2) {
        num_threads = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        size = std::atoi(argv[2]);
    }

    // Ensure OpenMP uses the requested number of threads
    omp_set_num_threads(num_threads);

    // Allocate main arrays
    double **vec1 = new double*[size];
    double *vec2 = new double[size];
    double *res  = new double[size];

    auto start_time = std::chrono::steady_clock::now();

    // Initialize vec2 and res
    for (int i = 0; i < size; i++)
    {
        vec2[i] = size - i;
        res[i]  = 0.0;
        vec1[i] = nullptr; // good practice before assigning in parallel
    }

    auto end1 = std::chrono::steady_clock::now();

    // Parallelize the outer loop over i
    #pragma omp parallel for
    for (int i = 0; i < size; i++)
    {
        vec1[i] = new double[size];
        for (int y = 0; y < size; y++)
        {
            vec1[i][y] = double(y + 1);
            res[i]    += vec2[y] * vec1[i][y];
        }
    }

    auto end2 = std::chrono::steady_clock::now();

    const std::chrono::duration<double> elapsed_seconds1(end1 - start_time);
    const std::chrono::duration<double> elapsed_seconds2(end2 - end1);

    std::cout << "Init time: " << elapsed_seconds1.count() << std::endl;
    std::cout << "Work time: " << elapsed_seconds2.count() << std::endl;

    // Free allocated memory
    for (int i = 0; i < size; i++)
    {
        delete[] vec1[i];
    }

    delete[] vec1;
    delete[] vec2;
    delete[] res;

    return 0;
}