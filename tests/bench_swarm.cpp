/**
 * @file bench_swarm.cpp
 * @brief Performance benchmarking for the Lithic Lattice kernel.
 * 
 * Measures execution latency and throughput for spatial mapping across 
 * various grid resolutions. Optimized for multi-core embedded profiling.
 */

#include "../include/kernel.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace p1;
using namespace std::chrono;

/**
 * @brief Profiles a single resolution tier.
 */
void run_benchmark(int width, int height, int iterations) {
    size_t size = static_cast<size_t>(width) * height;
    
    // Allocate aligned buffers for physical simulation
    std::vector<int32_t> current(size, 32768); // 0.5 in Q16.16
    std::vector<int32_t> prev(size, 32768);
    std::vector<int32_t> next(size, 0);

    // Warm-up iteration
    lithic_lattice_step(current.data(), prev.data(), next.data(), width, height, BoundaryType::ABSORBING);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        lithic_lattice_step(current.data(), prev.data(), next.data(), width, height, BoundaryType::ABSORBING);
        // Swap buffers to simulate continuous time-steps
        std::swap(current, next);
    }

    auto end = high_resolution_clock::now();
    auto total_ms = duration_cast<milliseconds>(end - start).count();
    double avg_latency = static_cast<double>(total_ms) / iterations;

    std::cout << std::left << std::setw(15) << (std::to_string(width) + "x" + std::to_string(height))
              << std::setw(15) << iterations
              << std::setw(20) << std::fixed << std::setprecision(3) << avg_latency << " ms"
              << std::endl;
}

int main() {
    std::cout << "--- PROVISION1: LITHIC LATTICE PERFORMANCE AUDIT ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Resolution" 
              << std::setw(15) << "Iterations" 
              << std::setw(20) << "Avg Latency" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    // Standard low-power drone sensor resolutions
    run_benchmark(128, 128, 1000);  // Micro-drone / Lidar subset
    run_benchmark(320, 240, 500);   // Standard navigation feed
    run_benchmark(640, 480, 100);   // High-fidelity spatial mapping

    std::cout << "--- BENCHMARK COMPLETE ---" << std::endl;

    return 0;
}
