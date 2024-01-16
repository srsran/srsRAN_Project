/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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