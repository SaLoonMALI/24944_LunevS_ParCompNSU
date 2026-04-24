// Sam Lunev. 2026. All Rights Reserved.

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#define VEC_SIZE 10000


int main(int argc, char** argv) {
    int num_threads = 1;
    int size = VEC_SIZE;

    if (argc >= 2) {
        num_threads = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        size = std::atoi(argv[2]);
    }
//

    double* vec1 = new double[size * size];
    double* vec2 = new double[size];
    double* res = new double[size];

    for (int i = 0; i < size; i++) {
        vec2[i] = size - i;
        res[i] = 0.0;
    }

//
    auto start_time = std::chrono::steady_clock::now();

    std::thread* threads = new std::thread[num_threads];
    int chunk_size = size / num_threads;

    for (int t = 0; t < num_threads; t++) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? size : (t + 1) * chunk_size;

        threads[t] = std::thread([&, start, end, size]() {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < size; j++) {
                    vec1[i * size + j] = double(j+1);
                    res[i] += vec2[j] * vec1[i * size + j];
                }
            }
        });
    }

    for (int i = 0; i < num_threads; i++) {
        threads[i].join();
    }

    auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds(end - start_time);
    std::cout << elapsed_seconds.count() << std::endl;
//

    delete[] vec1;
    delete[] vec2;
    delete[] res;
    delete[] threads;

    return 0;
}
