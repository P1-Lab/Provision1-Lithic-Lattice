#ifndef PROVISION1_FIXED_POINT_H
#define PROVISION1_FIXED_POINT_H

#include <cstdint>

/**
 * @file fixed_point.h
 * @brief Q16.16 fixed-point arithmetic utility definitions.
 */

namespace p1 {

// --- Q16.16 Base Constants ---
static constexpr int32_t FP_SHIFT = 16;
static constexpr int32_t FP_ONE   = (1 << FP_SHIFT);   // 65536
static constexpr int32_t FP_ZERO  = 0;
static constexpr int32_t FP_HALF  = (1 << (FP_SHIFT - 1)); // 32768

/**
 * @brief Multiplies two Q16.16 values.
 * Uses 64-bit intermediate to prevent overflow before the shift.
 */
inline int32_t fp_mul(int32_t a, int32_t b) {
    return static_cast<int32_t>((static_cast<int64_t>(a) * b) >> FP_SHIFT);
}

/**
 * @brief Divides two Q16.16 values.
 * Scales the numerator by 2^16 before division to maintain precision.
 */
inline int32_t fp_div(int32_t a, int32_t b) {
    if (b == 0) return 0; // Guard against division by zero
    return static_cast<int32_t>((static_cast<int64_t>(a) << FP_SHIFT) / b);
}

/**
 * @brief Clamps a Q16.16 value within a specified range.
 */
inline int32_t fp_clamp(int32_t val, int32_t min_val, int32_t max_val) {
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

/**
 * @brief Converts a floating point literal to Q16.16 at compile time.
 */
constexpr int32_t to_fp(double v) {
    return static_cast<int32_t>(v * 65536.0 + (v >= 0 ? 0.5 : -0.5));
}

} // namespace p1

#endif // PROVISION1_FIXED_POINT_H
