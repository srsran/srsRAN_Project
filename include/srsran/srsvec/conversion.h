/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// \brief Converts a sequence of numbers from complex float to int16 applying the given scaling and rounding the result
/// to the nearest integer.
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert(span<const cf_t> x, float scale, span<int16_t> z);

/// \brief Converts from int16 to complex float applying the given scaling.
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert(span<const int16_t> x, float scale, span<cf_t> z);

/// \brief Converts a sequence of numbers from float to int16 applying the given scaling and rounding the result to the
/// nearest integer.
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert(span<const float> x, float scale, span<int16_t> z);

/// Converts from int16 to float applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert(span<const int16_t> x, float scale, span<float> z);

/// \brief Converts a sequence of numbers from complex float to complex brain float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<cbf16_t> out, span<const cf_t> in);

/// \brief Converts a sequence of numbers from complex brain float to complex float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<cf_t> out, span<const cbf16_t> in);

} // namespace srsvec
} // namespace srsran
