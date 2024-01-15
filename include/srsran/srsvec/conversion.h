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
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// Converts from complex float to int16 applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert(span<const cf_t> x, float scale, span<int16_t> z);

/// Converts a sequence of numbers from complex float to int16 applying the given scaling and rounding the result to the
/// nearest integer.
///
/// \param[in]  x     Input data.
/// \param[in]  scale Scaling factor.
/// \param[out] z     Output data.
void convert_round(span<const cf_t> x, float scale, span<int16_t> z);

/// Converts and swaps real and imaginary from complex float to int16 applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert_swap(span<const cf_t> x, float scale, span<int16_t> z);

/// Converts from int16 to complex float applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert(span<const int16_t> x, float scale, span<cf_t> z);

/// Converts and swaps real and imaginary from int16 to complex float applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert_swap(span<const int16_t> x, float scale, span<cf_t> z);

/// Converts from float to int16 applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert(span<const float> x, float scale, span<int16_t> z);

/// Converts a sequence of numbers from float to int16 applying the given scaling and rounding the result to the nearest
/// integer.
///
/// \param[in] x     Input data.
/// \param[in] scale Scaling factor.
/// \param[out] z    Output data.
void convert_round(span<const float> x, float scale, span<int16_t> z);

/// Converts from int16 to float applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert(span<const int16_t> x, float scale, span<float> z);

} // namespace srsvec
} // namespace srsran
