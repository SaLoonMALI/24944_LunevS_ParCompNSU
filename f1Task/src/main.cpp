/*
 ============================================================================
 Author      : Sam Lunev
 Version     : .2026.0
 Copyright   : All rights reserved
 ============================================================================
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>

#ifdef DOUBLE_OPT
using Fpdt = double;
#else
using Fpdt = float;
#endif

#define arr_size 10000000
#define chunk_quantity 5
#define chunk_size 2000000

using namespace std;


int main() {
    vector<Fpdt> data(arr_size);
    vector<thread> threads;
    Fpdt sum = 0;
 
    for (int i = 0; i < chunk_quantity; i++) {
    	// Chunk size calculation
    	int start  = i * chunk_size;
    	int end = (i + 1) * chunk_size;

    	// Launch threads
    	// *Lambda function syntax: [](){};
    	threads.emplace_back([start, end, &data]() {
    		for (int j = start; j < end; j++) {
    			// Sine wave generation
    			data[j] = sin(2.0 * M_PI * j / (Fpdt)arr_size);
    		}
    	});
    }

    //Wait for all threads to finish
    for (auto& thread : threads) {
       	thread.join();
    } // Loop through each thread

    for (int i = 0; i < arr_size; i++) {
    	sum += data[i];
    }
    cout << sum << endl;

    return 0;
}
