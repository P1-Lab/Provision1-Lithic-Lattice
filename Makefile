# --- Provision1: Lithic Lattice Build System ---
# Target: Embedded Linux / Multi-core Drone Architecture
# Requirements: G++ with OpenMP support

CXX      := g++
CXXFLAGS := -O3 -std=c++17 -fopenmp -Wall -Wextra -fPIC
INCLUDES := -Iinclude
SRC_DIR  := src
TEST_DIR := tests
OBJ_DIR  := obj
BIN_DIR  := bin

# Sources
KERNEL_SRC := $(SRC_DIR)/kernel.cpp
BENCH_SRC  := $(TEST_DIR)/bench_swarm.cpp

# Objects
KERNEL_OBJ := $(OBJ_DIR)/kernel.o
BENCH_OBJ  := $(OBJ_DIR)/bench_swarm.o

# Target Binary
TARGET := $(BIN_DIR)/bench_swarm

.PHONY: all clean directories

all: directories $(TARGET)

# Create necessary directory structure
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

# Compile Kernel Object
$(KERNEL_OBJ): $(KERNEL_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile Benchmark Object
$(BENCH_OBJ): $(BENCH_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link Benchmark Binary
$(TARGET): $(KERNEL_OBJ) $(BENCH_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Cleanup Workspace
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "[CLEAN] Workspace sanitized."

# Hardware-specific Optimization (Optional)
# Usage: make native
native: CXXFLAGS += -march=native
native: all
