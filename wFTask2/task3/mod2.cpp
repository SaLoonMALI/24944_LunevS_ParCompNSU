// Sam Lunev. 2026. All Rights Reserved.

#include <iostream>
#include <cstdio>
#include <omp.h>
#include <fstream>
#include <chrono>
#include <cmath>
#include <cstdlib>


#define SIZE 1000


inline int index(int i, int j, int size) {
    return i * size + j;
}

int main(int argc, char **argv) {
//
    int num_threads = 1;
    int size = SIZE;

    if (argc >= 2) {
	num_threads = std::atoi(argv[1]);
    }
    if (argc >= 3) {
	size = std::atoi(argv[2]);
    }
    
    // Force OpenMP to use requested number of threads
    omp_set_num_threads(num_threads);
//

//
    // Alloc 2D array as 1D long-array
    double* A = new double[size * size];
    double* b = new double[size];
    double* sol = new double[size];
    double* res = new double[size];

    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
	for (int j = 0; j < size; j++) {
	    if (i == j) {
                A[index(i, j, size)] = 2.0;
	    } else {
                A[index(i, j, size)] = 1.0;
	    }
	}
        b[i] = size + 1;
	res[i] = 0.0;
	sol[i] = 0.0;
    }
//

//
    auto start_time = std::chrono::steady_clock::now();

    // Processing loop with enhanced parallelization
        for (int z = 0; z < size; z++) {
        #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < size; i++) {
                double sum = 0.0;
                for (int j = 0; j < size; j++) {
                    if (j != i) {
                        sum += (A[index(i, j, size)] * res[j]);
                    }
                }
                sol[i] = (1 - 0.0001) * res[i] + 0.0001 * ((b[i] - sum) / A[index(i, i, size)]);
            }

            double sum_error = 0.0;
            #pragma omp for
            for (int i = 0; i < size; i++) {
		double diff = sol[i] - res[i];
                sum_error += diff * diff;
            }
            sum_error = sqrt(sum_error);

            #pragma omp for
            for (int i = 0; i < size; i++) {
                res[i] = sol[i];
            }
	}
    }

    auto end_time = std::chrono::steady_clock::now();

    const std::chrono::duration<double> elapsed_seconds(end_time - start_time);
    std::cout << elapsed_seconds.count() << std::endl;
//

//
    delete[] A;
    delete[] b;
    delete[] sol;
    delete[] res;
//
    return 0;
}

