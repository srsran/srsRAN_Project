/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_CONVERSION_H
#define SRSGNB_SRSVEC_CONVERSION_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

/// Converts from complex float to int16 applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling prior conversion
/// \param [out] z resultant data
void convert(span<const cf_t> x, float scale, span<int16_t> z);

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

/// Converts from int16 to float applying the given scaling
///
/// \param [in] x is the input data
/// \param [in] scale input data scaling after conversion
/// \param [out] z resultant data
void convert(span<const int16_t> x, float scale, span<float> z);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_CONVERSION_H
