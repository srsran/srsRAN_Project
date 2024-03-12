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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/support/srsran_assert.h"
#include <cstdint>

/// \brief The srsvec library provides optimized SIMD commonly used operations.
///
/// The supported vector data types are:
/// - srsran::span<cf_t>: For complex float vectors
/// - srsran::span<float>: For float vectors
/// - srsran::span<int16_t>: For signed 16 bit integer vectors
/// - srsran::span<int8_t>: For signed 8 bit integer vectors
/// - srsran::span<uint8_t>: For unsigned 8 bit integer vectors
///
/// To make the span constant use srsran::span<const T>.

/// Asserts that both objects have the same size.
#define srsran_srsvec_assert_size(X, Y)                                                                                \
  srsran_assert(                                                                                                       \
      X.size() == Y.size(), "The size " #X " (i.e., {}) and " #Y " (i.e., {}) must be equal.", X.size(), Y.size())
