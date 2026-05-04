#ifndef PROVISION1_LITHIC_LATTICE_KERNEL_H
#define PROVISION1_LITHIC_LATTICE_KERNEL_H

#include <cstdint>

/**
 * @file kernel.h
 * @brief Deterministic Q16.16 boundary detection and spatial mapping kernel.
 */

namespace p1 {

/**
 * @brief Specifies the treatment of lattice edges during simulation.
 */
enum class BoundaryType {
    DIRICHLET, // Boundary fixed at 0 (Infinite energy sink)
    NEUMANN,   // Zero gradient (Reflective boundary)
    ABSORBING  // Dissipative (Perfectly Matched Layer simulation)
};

/**
 * @brief Executes a single time-step integration of the Lithic Lattice.
 * 
 * Computes spatial strain via a discrete Laplacian and integrates temporal momentum 
 * using Verlet integration in Q16.16 fixed-point arithmetic.
 *
 * @param current  Pointer to the current state buffer (width * height).
 * @param prev     Pointer to the previous state buffer (momentum reference).
 * @param next     Pointer to the destination buffer for the computed state.
 * @param width    Grid width in cells.
 * @param height   Grid height in cells.
 * @param boundary Boundary condition logic to apply at grid edges.
 */
void lithic_lattice_step(
    int32_t* current,
    const int32_t* prev,
    int32_t* next,
    int width,
    int height,
    BoundaryType boundary
);

} // namespace p1

#endif // PROVISION1_LITHIC_LATTICE_KERNEL_H
