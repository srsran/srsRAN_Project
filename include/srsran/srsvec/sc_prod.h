/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
void sc_prod(span<cbf16_t> z, span<const cf_t> x, float h);
///@}

} // namespace srsvec
} // namespace srsran
