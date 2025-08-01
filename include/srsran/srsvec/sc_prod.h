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
/// \brief Product of a vector by a scalar.

#pragma once

#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

///@{
/// \brief Product of a vector by a scalar.
/// \param[out]  z Output vector.
/// \param[in]   x Input vector.
/// \param[in]   h Scalar factor.
/// \warning An assertion is triggered if the input and output vectors have different sizes.
void sc_prod(span<cf_t> z, span<const cf_t> x, cf_t h);
void sc_prod(span<cbf16_t> z, span<const cbf16_t> x, cf_t h);
void sc_prod(span<cf_t> z, span<const cf_t> x, float h);
void sc_prod(span<float> z, span<const float> x, float h);
void sc_prod(span<int16_t> z, span<const int16_t> x, int16_t h);
///@}

} // namespace srsvec
} // namespace srsran
