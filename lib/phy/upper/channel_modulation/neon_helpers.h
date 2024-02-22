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

#include "srsran/support/math_utils.h"
#include <arm_neon.h>

namespace srsran {

namespace neon {

/// \brief Absolute values.
/// \brief Absolute values.
/// \param[in] value Input single-precision NEON register.
/// \return A single-precision NEON register with the absolute value.
inline float32x4_t abs_f32(float32x4_t value)
{
  return vabsq_f32(value);
}

/// \brief Copy sign from a single-precision NEON register.
/// \param[in] value0 Single-precision NEON register.
/// \param[in] value1 Single-precision NEON register.
/// \return A single-precision NEON register with the magnitudes of \c value0 and the signs of \c value1.
/// \remark A zero in the second argument is considered as a positive number, following the convention of \c
/// std::copysign.
inline float32x4_t copysign_f32(float32x4_t value0, float32x4_t value1)
{
  float32x4_t abs_value0 = vabsq_f32(value0);
  float32x4_t neg_value0 = vnegq_f32(abs_value0);
  // uint32x4_t  sign_mask  = vtstq_s32(vreinterpretq_s32_f32(value1), vdupq_n_s32(0x80000000));
  uint32x4_t sign_mask = vreinterpretq_u32_s32(vshrq_n_s32(vreinterpretq_s32_f32(value1), 31));
  return vbslq_f32(sign_mask, neg_value0, abs_value0);
}

/// \brief Clips the values of a single-precision NEON register.
///
/// The values greater than \c range_ceil or lower than \c range_floor are substituted by their corresponding range
/// limits.
///
/// \param[in] value       Input values.
/// \param[in] range_ceil  Ceiling values.
/// \param[in] range_floor Floor values.
/// \return A single-precision NEON register containing the clipped values.
inline float32x4_t clip_f32(float32x4_t value, float32x4_t range_ceil, float32x4_t range_floor)
{
  value = vbslq_f32(vcgtq_f32(value, range_ceil), range_ceil, value);
  value = vbslq_f32(vcgtq_f32(range_floor, value), range_floor, value);
  return value;
}

/// \brief Clips the values of a NEON register carrying four signed 32-bit integers.
///
/// The values greater than \c range_ceil or lower than \c range_floor are substituted by their corresponding range
/// limits.
///
/// \param[in] value       Input values.
/// \param[in] range_ceil  Ceiling values.
/// \param[in] range_floor Floor values.
/// \return A NEON register containing the clipped values.
inline int32x4_t clip_s32(int32x4_t value, int32x4_t range_ceil, int32x4_t range_floor)
{
  value = vbslq_s32(vcgtq_s32(value, range_ceil), range_ceil, value);
  value = vbslq_s32(vcgtq_s32(range_floor, value), range_floor, value);
  return value;
}

/// \brief Ensures that 32-bit integers are bounded, otherwise set them to zero.
///
/// \param[in] value      Integers to be tested.
/// \param[in] bound_up   Upper bound.
/// \param[in] bound_low  Lower bound.
/// \return The input integers with zeros in place of the out-of-bound values.
inline int32x4_t check_bounds_s32(int32x4_t value, int32x4_t bound_up, int32x4_t bound_low)
{
  int32x4_t  ZEROS = vdupq_n_s32(0);
  uint32x4_t mask  = vcgtq_s32(value, bound_up);
  value            = vbslq_s32(mask, ZEROS, value);

  mask  = vcgtq_s32(bound_low, value);
  value = vbslq_s32(mask, ZEROS, value);

  return value;
}

/// \brief Clips and quantizes four single-precision NEON registers, continuous log-likelihood ratio to the discrete
/// representation of type \c log_likelihood_ratio in a single NEON register.
///
/// \param[in] value_0      Single-precision NEON register with the first four continuous log-likelihood ratio.
/// \param[in] value_1      Single-precision NEON register with the second four continuous log-likelihood ratio.
/// \param[in] value_2      Single-precision NEON register with the third four continuous log-likelihood ratio.
/// \param[in] value_3      Single-precision NEON register with the fourth four continuous log-likelihood ratio.
/// \param[in] range_limit  The input value mapped to \ref log_likelihood_ratio::max().
/// \return A quantized representation of the input values as \c log_likelihood_ratio quantity.
/// \note The quantization in the range <tt>(-range_limit, range_limit)</tt> is [mid-tread
/// uniform](https://en.wikipedia.org/wiki/Quantization_(signal_processing)#Mid-riser_and_mid-tread_uniform_quantizers),
/// with quantization step <tt> range_limit / LLR_MAX </tt>.
/// \note All values larger (in magnitude) than \c range_limit, will be clipped and mapped to
/// <tt>&plusmn;LLR_MAX</tt>, depending on their sign.
inline int8x16_t
quantize_f32(float32x4_t value_0, float32x4_t value_1, float32x4_t value_2, float32x4_t value_3, float range_limit)
{
  // Scale.
  float32x4_t SCALE = vdupq_n_f32(static_cast<float>(log_likelihood_ratio::max().to_int()) / range_limit);
  value_0           = vmulq_f32(value_0, SCALE);
  value_1           = vmulq_f32(value_1, SCALE);
  value_2           = vmulq_f32(value_2, SCALE);
  value_3           = vmulq_f32(value_3, SCALE);

  // Clip and round to the nearest integer.
  float32x4_t RANGE_CEIL  = vdupq_n_f32(log_likelihood_ratio::max().to_int());
  float32x4_t RANGE_FLOOR = vdupq_n_f32(log_likelihood_ratio::min().to_int());
  value_0                 = vrndnq_f32(clip_f32(value_0, RANGE_CEIL, RANGE_FLOOR));
  value_1                 = vrndnq_f32(clip_f32(value_1, RANGE_CEIL, RANGE_FLOOR));
  value_2                 = vrndnq_f32(clip_f32(value_2, RANGE_CEIL, RANGE_FLOOR));
  value_3                 = vrndnq_f32(clip_f32(value_3, RANGE_CEIL, RANGE_FLOOR));

  // Convert to 32 bit integer.
  int32x4_t llr_s32_0 = vcvtq_s32_f32(value_0);
  int32x4_t llr_s32_1 = vcvtq_s32_f32(value_1);
  int32x4_t llr_s32_2 = vcvtq_s32_f32(value_2);
  int32x4_t llr_s32_3 = vcvtq_s32_f32(value_3);

  // Check bounds one more time: if value_X contains a NaN, its casting to int32_t is indeterminate - set it to zero.
  int32x4_t BOUND_UP  = vdupq_n_s32(log_likelihood_ratio::max().to_int());
  int32x4_t BOUND_LOW = vdupq_n_s32(log_likelihood_ratio::min().to_int());
  llr_s32_0           = check_bounds_s32(llr_s32_0, BOUND_UP, BOUND_LOW);
  llr_s32_1           = check_bounds_s32(llr_s32_1, BOUND_UP, BOUND_LOW);
  llr_s32_2           = check_bounds_s32(llr_s32_2, BOUND_UP, BOUND_LOW);
  llr_s32_3           = check_bounds_s32(llr_s32_3, BOUND_UP, BOUND_LOW);

  // Conversion to 16 bit and cast to int8 (every odd element of the vector will be 0 or 0xff).
  int8x8_t llr_s8_0 = vreinterpret_s8_s16(vmovn_s32(llr_s32_0));
  int8x8_t llr_s8_1 = vreinterpret_s8_s16(vmovn_s32(llr_s32_1));
  int8x8_t llr_s8_2 = vreinterpret_s8_s16(vmovn_s32(llr_s32_2));
  int8x8_t llr_s8_3 = vreinterpret_s8_s16(vmovn_s32(llr_s32_3));

  // Merge resulting LLRs into a single register.
  int8x8_t out_llrs_s8_0 = vuzp1_s8(llr_s8_0, llr_s8_1);
  int8x8_t out_llrs_s8_1 = vuzp1_s8(llr_s8_2, llr_s8_3);
  return vcombine_s8(out_llrs_s8_0, out_llrs_s8_1);
}

/// \brief Helper function to calculate an interval index from single-precision NEON register values.
/// \param[in] value          Input NEON register.
/// \param[in] interval_width Interval width.
/// \param[in] nof_intervals  Number of intervals.
/// \return A NEON register carrying four signed 32-bit integers with the corresponding interval indexes.
inline uint32x4_t compute_interval_idx(float32x4_t value, float interval_width, int nof_intervals)
{
  // Scale.
  value = vmulq_f32(value, vdupq_n_f32(1.0F / interval_width));

  // Round to the lowest integer.
  value = vrndmq_f32(value);

  // Convert to int32.
  int32x4_t idx = vcvtq_s32_f32(value);

  // Add interval offset.
  idx = vaddq_s32(idx, vdupq_n_s32(nof_intervals / 2));

  // Clip index and return.
  return vreinterpretq_u32_s32(clip_s32(idx, vdupq_n_s32(nof_intervals - 1), vdupq_n_s32(0)));
}

/// \brief Get values from a look-up table.
/// \param[in] table   Look-up table containing eight single-precision values.
/// \param[in] indexes NEON register containing four indexes.
/// \return A single-precision NEON register containing the eight values corresponding to the indexes.
inline float32x4_t look_up_table(const float* table, uint32x4_t indexes)
{
  float32x4_t value = vdupq_n_f32(0.0);
  value             = vsetq_lane_f32(table[vgetq_lane_u32(indexes, 0)], value, 0);
  value             = vsetq_lane_f32(table[vgetq_lane_u32(indexes, 1)], value, 1);
  value             = vsetq_lane_f32(table[vgetq_lane_u32(indexes, 2)], value, 2);
  value             = vsetq_lane_f32(table[vgetq_lane_u32(indexes, 3)], value, 3);
  return value;
}

/// \brief Applies an interval function to a series of values.
/// \tparam Table             Look-up table type. All tables mut be of the same type.
/// \param[in] value          Single-precision NEON register with eight input values.
/// \param[in] rcp_noise      Single-precision NEON register with the reciprocal noise corresponding to the values.
/// \param[in] nof_intervals  Number of intervals.
/// \param[in] interval_width Interval width to quantify the interval indexes.
/// \param[in] slopes         Table with the slope of each interval.
/// \param[in] intercepts     Table with the interception points of each interval.
/// \return A single-precision NEON register containing the results of the interval function.
/// \remark The number of intervals must be lower than or equal to \c Table size.
template <typename Table>
inline float32x4_t interval_function(float32x4_t  value,
                                     float32x4_t  rcp_noise,
                                     float        interval_width,
                                     unsigned     nof_intervals,
                                     const Table& slopes,
                                     const Table& intercepts)
{
  uint32x4_t interval_index = neon::compute_interval_idx(value, interval_width, nof_intervals);

  float32x4_t slope     = neon::look_up_table(slopes.data(), interval_index);
  float32x4_t intercept = neon::look_up_table(intercepts.data(), interval_index);

  float32x4_t result = vmulq_f32(vaddq_f32(vmulq_f32(slope, value), intercept), rcp_noise);

  float32x4_t threshold = vdupq_n_f32(near_zero);
  float32x4_t zero      = vdupq_n_f32(0.0f);
  result                = vbslq_f32(vcgeq_f32(vabdq_f32(value, zero), threshold), result, zero);

  return result;
}

/// \brief Safe division.
///
/// \return <tt>dividend / divisor</tt> if \c divisor is not zero, \c 0 otherwise.
inline float32x4_t safe_div(float32x4_t dividend, float32x4_t divisor)
{
  static const float32x4_t all_zero = vdupq_n_f32(0.0f);
  // Compare divisor to zero (greater than).
  uint32x4_t  mask   = vcgtq_f32(divisor, all_zero);
  float32x4_t result = vdivq_f32(dividend, divisor);
  return vbslq_f32(mask, result, all_zero);
}

} // namespace neon

} // namespace srsran