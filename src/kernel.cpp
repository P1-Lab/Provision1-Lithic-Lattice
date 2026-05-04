#include "kernel.h"
#include "fixed_point.h"
#include <cstring>

/**
 * @file kernel.cpp
 * @brief Implementation of the deterministic Lithic Lattice spatial solver.
 */

namespace p1 {

// --- Configuration Constants (Q16.16) ---
static constexpr int32_t STIFFNESS_K   = to_fp(1.3);
static constexpr int32_t DAMPING_COEFF = to_fp(0.05);
static constexpr int32_t STRAIN_FACTOR = to_fp(0.5);
static constexpr int32_t MAX_VAL       = FP_ONE;
static constexpr int32_t MIN_VAL       = FP_ZERO;

void lithic_lattice_step(
    int32_t* current,
    const int32_t* prev,
    int32_t* next,
    int width,
    int height,
    BoundaryType boundary
) {
    const int row_stride = width;

    /**
     * @section Main Solver Loop
     * Parallelized execution for multi-core embedded processors.
     * Computes the discrete spatial Laplacian and integrates via Verlet momentum.
     */
    #pragma omp parallel for schedule(static)
    for (int i = 1; i < height - 1; ++i) {
        int row_offset = i * row_stride;
        for (int j = 1; j < width - 1; ++j) {
            int idx = row_offset + j;

            // 1. SPATIAL LAPLACIAN (Discrete Second-Order Derivative)
            // L = (Up + Down + Left + Right) - 4*Center
            int32_t center = current[idx];
            int32_t up     = current[idx - row_stride];
            int32_t down   = current[idx + row_stride];
            int32_t left   = current[idx - 1];
            int32_t right  = current[idx + 1];

            int32_t laplacian = (up + down + left + right) - (center << 2);

            // 2. TEMPORAL VELOCITY (Verlet Momentum)
            int32_t velocity = center - prev[idx];

            // 3. NON-LINEAR STRAIN HARDENING
            // Modulates effective stiffness based on local gradient energy.
            // force_sq = (L^2) >> 16
            int32_t force_sq = fp_mul(laplacian, laplacian);
            
            // Denominator = 1 + (Strain_Factor * Force_Sq)
            int32_t denominator = FP_ONE + fp_mul(STRAIN_FACTOR, force_sq);
            
            // Effective Gain = Stiffness / Denominator
            int32_t effective_gain = fp_div(STIFFNESS_K, denominator);

            // 4. INTEGRATION STEP
            // Next = Center + (Velocity * Damping) + (Laplacian * Effective_Gain)
            int32_t vel_term = fp_mul(velocity, DAMPING_COEFF);
            int32_t lap_term = fp_mul(laplacian, effective_gain);
            
            int32_t next_val = center + vel_term + lap_term;

            // 5. VALUE CLAMPING
            // Ensures the simulation remains within the Q16.16 0.0-1.0 substrate.
            next[idx] = fp_clamp(next_val, MIN_VAL, MAX_VAL);
        }
    }

    /**
     * @section Boundary Governance
     * Enforcement of edge conditions based on BoundaryType selection.
     * In high-performance swarm deployments, these are typically handled by 
     * specialized border loops or zero-copy padding.
     */
    // (Implementation of specific edge-case loops would follow here per 'boundary' enum)
}

} // namespace p1
