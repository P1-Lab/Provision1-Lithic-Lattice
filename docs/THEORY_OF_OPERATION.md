# THEORY_OF_OPERATION.md

## Mathematical Framework: Discrete Physical Simulation

The **Lithic Lattice Kernel** operates by treating spatial sensor data as a coupled oscillator network. Instead of treating pixels or voxels as independent data points, the engine models them as a continuous physical substrate governed by the laws of classical mechanics.

---

### 1. The Spatial Laplacian ($\nabla^2$)
The core of the boundary detection logic is the **Discrete Laplacian Operator**. In a 2D grid, the Laplacian measures the divergence of the gradient, effectively identifying local discontinuities (boundaries).

We use a five-point stencil for the discrete approximation:

$$ \nabla^2 \phi_{i,j} \approx \phi_{i+1,j} + \phi_{i-1,j} + \phi_{i,j+1} + \phi_{i,j-1} - 4\phi_{i,j} $$

*   **Engineering Application:** In a uniform field, the Laplacian is zero. At a physical boundary (an obstacle), the Laplacian yields a high-magnitude result. This provides an immediate, deterministic map of structural limits without requiring object recognition.

### 2. Time-Integration: Verlet Velocity
To maintain temporal consistency and predict movement within the swarm, the kernel utilizes **Verlet Integration**. Unlike standard Euler integration, Verlet is time-reversible and significantly more stable for physical simulations.

The state is updated based on the current position ($x_t$) and the previous position ($x_{t-1}$):

$$ x_{t+1} = x_t + (x_t - x_{t-1}) \cdot D + A \cdot \Delta t^2 $$

Where:
*   **$(x_t - x_{t-1})$**: Represents the current velocity (momentum).
*   **$D$**: Damping coefficient (`DAMPING_COEFF`) to dissipate energy and prevent feedback loops.
*   **$A$**: Acceleration, derived from the Laplacian (the "force" exerted by environmental boundaries).

### 3. Non-Linear Strain Hardening
To handle high-dynamic-range sensor input, the engine implements a **Strain Hardening** algorithm. This prevents the simulation from "shattering" or becoming unstable when encountering high-energy transients (abrupt sensor changes).

The effective gain ($G_{eff}$) is modulated by the local force:

$$ G_{eff} = \frac{K}{1 + \alpha \cdot (\nabla^2 \phi)^2} $$

*   **$K$**: Base stiffness (`STIFFNESS_K`).
*   **$\alpha$**: Strain factor (`STRAIN_FACTOR`).
*   **Result:** As the spatial "force" (Laplacian) increases, the local stiffness decreases. This non-linear response compresses the signal naturally, ensuring that boundaries remain sharp but the numerical simulation remains within the `Q16.16` bounds.

---

### 4. Numerical Precision: Q16.16 Fixed-Point
To ensure bit-exact synchronization across a drone swarm, all transcendental and differential operations are performed in **Q16.16 Fixed-Point Arithmetic**.

*   **Integer Scaling:** Values are scaled by $2^{16}$ (65536).
*   **Multiplication:** Requires a 64-bit intermediate product followed by a 16-bit right shift to maintain scale: $(A \times B) \gg 16$.
*   **Determinism:** By avoiding the IEEE 754 floating-point standard, we eliminate rounding errors that vary between ARM and x86 architectures, ensuring the swarm maintains a unified spatial "truth."

### 5. Boundary Governance
The engine supports multiple boundary conditions for the lattice edges:
1.  **Dirichlet:** Field fixed at zero (Infinite Sink).
2.  **Neumann:** Zero gradient at the edge (Reflective).
3.  **Absorbing:** Perfectly Matched Layer (PML) logic to dissipate energy, preventing artificial standing waves from interfering with sensor fusion.
