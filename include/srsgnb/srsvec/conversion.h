/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_CONVERSION_H_
#define SRSGNB_SRSVEC_CONVERSION_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

/**
 * @brief Converts from complex float to int16 applying the given scaling
 * @param x
 * @param scale
 * @param z
 */
void convert(span<const cf_t> x, float scale, span<int16_t> z);

/**
 * @brief Converts from complex float to int16 applying the given scaling and swapping the real and imaginary
 * @param x
 * @param scale
 * @param z
 */
void convert_swap(span<const cf_t> x, float scale, span<int16_t> z);

/**
 * @brief Converts from int16 to complex float applying the given scaling
 * @param x
 * @param scale
 * @param z
 */
void convert(span<const int16_t> x, float scale, span<cf_t> z);

/**
 * @brief Converts from int16 to complex float applying the given scaling and swapping the real and imaginary
 * @param x
 * @param scale
 * @param z
 */
void convert_swap(span<const int16_t> x, float scale, span<cf_t> z);

/**
 * @brief Converts from float to int16 applying the given scaling
 * @param x
 * @param scale
 * @param z
 */
void convert(span<const float> x, float scale, span<int16_t> z);

/**
 * @brief Converts from int16 to float applying the given scaling
 * @param x
 * @param scale
 * @param z
 */
void convert(span<const int16_t> x, float scale, span<float> z);

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_CONVERSION_H_
