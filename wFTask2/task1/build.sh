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
SIZES=(20000 40000)

OUTPUT_FILE="benchmark_results.txt"
echo "Thread Count,Size,Time,Speedup" > $OUTPUT_FILE

for size in "${SIZES[@]}"; do
    echo "Running with size: $size"
    
    echo "Running with 1 thread, size $size"
    REF_TIME=$(timeout 60s ./build/parallel_matrix_vector 1 $size 2>/dev/null | grep "Work time:" | awk '{print $3}' || echo "TIMEOUT")
    
    if [ "$REF_TIME" = "TIMEOUT" ]; then
        echo "Timeout for 1 thread, size $size"
        continue
    fi
    
    echo "Reference time (1 thread): $REF_TIME seconds"
    
    for threads in "${THREADS[@]}"; do
        echo "Running with $threads threads, size $size"
        TIME=$(timeout 60s ./build/parallel_matrix_vector $threads $size 2>/dev/null | grep "Work time:" | awk '{print $3}' || echo "TIMEOUT")
        
        if [ "$TIME" = "TIMEOUT" ]; then
            echo "Timeout for $threads threads, size $size"
            continue
        fi
        
        echo "Time with $threads threads: $TIME seconds"
        
        if (( $(echo "$REF_TIME > 0" | bc -l) )); then
            SPEEDUP=$(echo "scale=4; $REF_TIME / $TIME" | bc -l)
        else
            SPEEDUP=0
        fi
        
        echo "Speedup: $SPEEDUP"
        
        echo "$threads,$size,$TIME,$SPEEDUP" >> $OUTPUT_FILE
    done
done

echo ""
echo "=== Benchmark Results ==="
cat $OUTPUT_FILE

echo ""
echo "=== Summary in Requested Format ==="
echo "P=2; T1 [T(1, n)]; T2) (P=4; T4; S4) (P=6; T6; S6) (P=8; T8; S8)"

echo "P=2; T1 [T(1, 20000)]; T2) (P=4; T4; S4) (P=6; T6; S6) (P=8; T8; S8)" > summary.txt
echo "P=2; T1 [T(1, 40000)]; T2) (P=4; T4; S4) (P=6; T6; S6) (P=8; T8; S8)" >> summary.txt

cat summary.txt

echo ""
echo "Results saved to $OUTPUT_FILE and summary.txt"
