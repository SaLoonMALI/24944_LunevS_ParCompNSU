#!/bin/bash

# Sam Lunev. 2026. All Rights Reserved.

echo "=== System Information ==="
echo "Node Name: $(hostname)"
echo "CPU Info:"
lscpu
echo ""
echo "Server Name:"
cat /sys/devices/virtual/dmi/id/product_name 2>/dev/null || echo "Not available"
echo ""
echo "NUMA Nodes:"
numactl --hardware
echo ""
echo "Memory per NUMA Node:"
numactl --hardware | grep "node.*size"
echo ""
echo "OS Information:"
cat /etc/os-release | grep "PRETTY_NAME"
echo ""

echo "=== Building Project ==="
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "=== Running Benchmark ==="

THREADS=(1 2 4 7 8 16 20 40)
#THREADS=(1)
SIZES=(1000)

OUTPUT_FILE1="benchmark_results1.txt"
OUTPUT_FILE2="benchmark_results2.txt"
OUTPUT_FILE3="benchmark_results3.txt"
OUTPUT_FILE4="benchmark_results4.txt"

for size in "${SIZES[@]}"; do
    echo "Running with size: $size"
    
    echo "Running with 1 thread, size $size"
    REF_TIME1=$(timeout 199s ./build/j1_omp 1 $size 2>/dev/null || echo "TIMEOUT")
    REF_TIME2=$(timeout 199s ./build/j2_omp 1 $size 2>/dev/null || echo "TIMEOUT")
    REF_TIME3=$(timeout 199s ./build/j1s_omp 1 $size 2>/dev/null || echo "TIMEOUT")
    REF_TIME4=$(timeout 199s ./build/j2s_omp 1 $size 2>/dev/null || echo "TIMEOUT")
    
    echo "Reference time j1 (1 thread): $REF_TIME1 seconds"
    echo "Reference time j2 (1 thread): $REF_TIME2 seconds"
    echo "Reference time j1s (1 thread): $REF_TIME3 seconds"
    echo "Reference time j2s (1 thread): $REF_TIME4 seconds"
    
    for threads in "${THREADS[@]}"; do
        echo "Running with $threads threads, size $size"
        TIME1=$(timeout 999s ./build/j1_omp $threads $size 2>/dev/null || echo "TIMEOUT")
        TIME2=$(timeout 999s ./build/j2_omp $threads $size 2>/dev/null || echo "TIMEOUT")
        TIME3=$(timeout 999s ./build/j1s_omp $threads $size 2>/dev/null || echo "TIMEOUT")
        TIME4=$(timeout 999s ./build/j2s_omp $threads $size 2>/dev/null || echo "TIMEOUT")
        
        echo "Time j1 with $threads threads: $TIME1 seconds"
        echo "Time j2 with $threads threads: $TIME2 seconds"
        echo "Time j1s with $threads threads: $TIME3 seconds"
        echo "Time j2s with $threads threads: $TIME4 seconds"
        
        if (( $(echo "$REF_TIME1 > 0" | bc -l) )); then
            SPEEDUP1=$(echo "scale=4; $REF_TIME1 / $TIME1" | bc -l)
        else
            SPEEDUP1=0
        fi
        if (( $(echo "$REF_TIME2 > 0" | bc -l) )); then
            SPEEDUP2=$(echo "scale=4; $REF_TIME2 / $TIME2" | bc -l)
        else
            SPEEDUP2=0
        fi
        if (( $(echo "$REF_TIME3 > 0" | bc -l) )); then
            SPEEDUP3=$(echo "scale=4; $REF_TIME3 / $TIME3" | bc -l)
        else
            SPEEDUP3=0
        fi
        if (( $(echo "$REF_TIME4 > 0" | bc -l) )); then
            SPEEDUP4=$(echo "scale=4; $REF_TIME4 / $TIME4" | bc -l)
        else
            SPEEDUP4=0
        fi
        
        echo "Speedup: $SPEEDUP1"
        echo "Speedup: $SPEEDUP2"
        echo "Speedup: $SPEEDUP3"
        echo "Speedup: $SPEEDUP4"
        
        echo "$threads,$SPEEDUP1" >> $OUTPUT_FILE1
        echo "$threads,$SPEEDUP2" >> $OUTPUT_FILE2
        echo "$threads,$SPEEDUP3" >> $OUTPUT_FILE3
        echo "$threads,$SPEEDUP4" >> $OUTPUT_FILE4
    done
done
