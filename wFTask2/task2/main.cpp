// Sam Lunev. 2026. All Rights Reserved.

#include <iostream>
#include <chrono>
#include <omp.h>
#include <cmath>
#include <cstdlib>

#define VEC_SIZE 10000
#define BEGIN -5.0
#define END 5.0

double func(double x) {
    return sin(x) * cos(x) * exp(-x*x) * log(x + 2);
}

double integrate_omp(double a, double b, int n) {
    const double w = (b - a) / n;
    double sum = 0.0;

// 'schedule(static)' controls how loop iterations are divided among threads. (0: 0, 1, 2; 1: 3, 4, 5; ...)
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) {
        double local = func(a + i * w + (w / 2.0)) * w;
        #pragma omp atomic
        sum += local;
    }

    return sum;
}

/*
double integrate_omp(double a, double b, int n) {
    const double w = (b - a) / n;
    double sum = 0.0;

// 'schedule(static)' controls how loop iterations are divided among threads. (0: 0, 1, 2; 1: 3, 4, 5; ...)
    #pragma omp parallel for schedule(static) reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += func(a + i * w + (w / 2.0)) * w;
    }

    return sum;
}
*/

int main(int argc, char **argv) {
    int num_threads = 1;
    int size = VEC_SIZE;

    if (argc >= 2) {
        num_threads = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        size = std::atoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    auto start = std::chrono::steady_clock::now();
    integrate_omp(BEGIN, END, size);
    auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds1(end - start);
    std::cout << "Work time: " << elapsed_seconds1.count() << std::endl;
    return 0;
}
