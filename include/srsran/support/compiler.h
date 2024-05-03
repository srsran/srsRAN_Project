/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Defines generic compiler macros that get translated into compiler specific directives simplifying portability
/// between different compilers.

#pragma once

/// Emits a warning if a caller of the function with this attribute does not use its return value.
#define SRSRAN_NODISCARD __attribute__((warn_unused_result))

/// Provides a hint to the compiler that a condition is likely to be true.
#define SRSRAN_LIKELY(EXPR) __builtin_expect((bool)(EXPR), true)
/// Provides a hint to the compiler that a condition is likely to be false.
#define SRSRAN_UNLIKELY(EXPR) __builtin_expect((bool)(EXPR), false)

/// States that it is undefined behavior for the compiler to reach this point.
#define SRSRAN_UNREACHABLE __builtin_unreachable()

/// Verifies if compile-time symbol is defined.
#define SRSRAN_IS_DEFINED(x) SRSRAN_IS_DEFINED2(x)
#define SRSRAN_IS_DEFINED2(x) (#x[0] == 0 || (#x[0] >= '1' && #x[0] <= '9'))

/// Ensures a function is always inlinable.
#define SRSRAN_FORCE_INLINE __attribute__((always_inline)) inline

/// Wrapper macro for std::launder which is only active for C++17 and above.
#if __cplusplus >= 201703L
#define SRSRAN_LAUNDER(a) std::launder((a))
#else
#define SRSRAN_LAUNDER(a) (a)
#endif
