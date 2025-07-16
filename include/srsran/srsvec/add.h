/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Sum of two vectors.

#pragma once

#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

///@{
/// \brief Sum of two vectors.
/// \param[out]  Output vector.
/// \param[in]   First input vector.
/// \param[in]   Second input vector.
/// \warning An assertion is triggered if input and output vectors have different sizes.
void add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y);
void add(span<cf_t> z, span<const cf_t> x, span<const cbf16_t> y);
void add(span<float> z, span<const float> x, span<const float> y);
void add(span<int16_t> z, span<const int16_t> x, span<const int16_t> y);
void add(span<int8_t> z, span<const int8_t> x, span<const int8_t> y);
///@}

} // namespace srsvec
} // namespace srsran
