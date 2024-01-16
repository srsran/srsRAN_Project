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

#include <immintrin.h>

namespace srsran {

namespace mm256 {

/// \brief Absolute values.
/// \param[in] value Input single-precision AVX register.
/// \return A single-precision AVX register with the absolute value.
inline __m256 abs_ps(__m256 value)
{
  const __m256 mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7fffffff));
  return _mm256_and_ps(value, mask);
}

/// \brief Copy sign from a single-precision AVX register.
/// \param[in] value0 Single-precision AVX register.
/// \param[in] value1 Single-precision AVX register.
/// \return A single-precision AVX register with the magnitudes of \c value0 and the signs of \c value1.
/// \remark A zero in the second argument is considered as a positive number, following the convention of \c
/// std::copysign.
inline __m256 copysign_ps(__m256 value0, __m256 value1)
{
  __m256 abs_value0  = abs_ps(value0);
  __m256 sign_value1 = _mm256_and_ps(value1, _mm256_set1_ps(-0.0));
  return _mm256_or_ps(abs_value0, sign_value1);
}

/// \brief Clips the values of a single-precision AVX register.
///
/// The values greater than \c range_ceil or lower than \c range_floor are substituted by their corresponding range
/// limits.
///
/// \param[in] value       Input values.
/// \param[in] range_ceil  Ceiling values.
/// \param[in] range_floor Floor values.
/// \return A single-precision AVX register containing the clipped values.
inline __m256 clip_ps(__m256 value, __m256 range_ceil, __m256 range_floor)
{
  value = _mm256_blendv_ps(value, range_ceil, _mm256_cmp_ps(value, range_ceil, _CMP_GT_OS));
  value = _mm256_blendv_ps(value, range_floor, _mm256_cmp_ps(value, range_floor, _CMP_LT_OS));
  return value;
}

/// \brief Clips the values of an AVX register carrying eight signed 32-bit integers.
///
/// The values greater than \c range_ceil or lower than \c range_floor are substituted by their corresponding range
/// limits.
///
/// \param[in] value       Input values.
/// \param[in] range_ceil  Ceiling values.
/// \param[in] range_floor Floor values.
/// \return An AVX register containing the clipped values.
inline __m256i clip_epi32(__m256i value, __m256i range_ceil, __m256i range_floor)
{
  value = _mm256_blendv_epi8(value, range_ceil, _mm256_cmpgt_epi32(value, range_ceil));
  value = _mm256_blendv_epi8(value, range_floor, _mm256_cmpgt_epi32(range_floor, value));
  return value;
}

/// \brief Ensures that 32-bit integers are bounded, otherwise set them to zero.
///
/// \param[in] value      Integers to be tested.
/// \param[in] bound_up   Upper bound.
/// \param[in] bound_low  Lower bound.
/// \return The input integers with zeros in place of the out-of-bound values.
inline __m256i check_bounds_epi32(__m256i value, __m256i bound_up, __m256i bound_low)
{
  __m256i ZEROS = _mm256_set1_epi8(0);
  __m256i mask  = _mm256_cmpgt_epi32(value, bound_up);
  // Since the mask is set for all bytes of the int32_t integers, we can use the byte-wise blend.
  value = _mm256_blendv_epi8(value, ZEROS, mask);

  mask = _mm256_cmpgt_epi32(bound_low, value);
  // Since the mask is set for all bytes of the int32_t integers, we can use the byte-wise blend.
  value = _mm256_blendv_epi8(value, ZEROS, mask);

  return value;
}

/// \brief Clips and quantizes four single-precision AVX registers, continuous log-likelihood ratio to the discrete
/// representation of type \c log_likelihood_ratio in a single AVX register.
///
/// \param[in] value_0      Single-precision AVX register with the first eight continuous log-likelihood ratio.
/// \param[in] value_1      Single-precision AVX register with the second eight continuous log-likelihood ratio.
/// \param[in] value_2      Single-precision AVX register with the third eight continuous log-likelihood ratio.
/// \param[in] value_3      Single-precision AVX register with the fourth eight continuous log-likelihood ratio.
/// \param[in] range_limit  The input value mapped to \ref log_likelihood_ratio::max().
/// \return A quantized representation of the input values as \c log_likelihood_ratio quantity.
/// \note The quantization in the range <tt>(-range_limit, range_limit)</tt> is [mid-tread
/// uniform](https://en.wikipedia.org/wiki/Quantization_(signal_processing)#Mid-riser_and_mid-tread_uniform_quantizers),
/// with quantization step <tt> range_limit / LLR_MAX </tt>.
/// \note All values larger (in magnitude) than \c range_limit, will be clipped and mapped to
/// <tt>&plusmn;LLR_MAX</tt>, depending on their sign.
inline __m256i quantize_ps(__m256 value_0, __m256 value_1, __m256 value_2, __m256 value_3, float range_limit)
{
  // Scale.
  __m256 SCALE = _mm256_set1_ps(static_cast<float>(log_likelihood_ratio::max().to_int()) / range_limit);
  value_0      = _mm256_mul_ps(value_0, SCALE);
  value_1      = _mm256_mul_ps(value_1, SCALE);
  value_2      = _mm256_mul_ps(value_2, SCALE);
  value_3      = _mm256_mul_ps(value_3, SCALE);

  // Clip and round to the nearest integer.
  __m256 RANGE_CEIL  = _mm256_set1_ps(log_likelihood_ratio::max().to_int());
  __m256 RANGE_FLOOR = _mm256_set1_ps(log_likelihood_ratio::min().to_int());
  value_0            = _mm256_round_ps(clip_ps(value_0, RANGE_CEIL, RANGE_FLOOR), _MM_FROUND_NINT);
  value_1            = _mm256_round_ps(clip_ps(value_1, RANGE_CEIL, RANGE_FLOOR), _MM_FROUND_NINT);
  value_2            = _mm256_round_ps(clip_ps(value_2, RANGE_CEIL, RANGE_FLOOR), _MM_FROUND_NINT);
  value_3            = _mm256_round_ps(clip_ps(value_3, RANGE_CEIL, RANGE_FLOOR), _MM_FROUND_NINT);

  // Convert to 32 bit.
  __m256i llr_i32_0 = _mm256_cvtps_epi32(value_0);
  __m256i llr_i32_1 = _mm256_cvtps_epi32(value_1);
  __m256i llr_i32_2 = _mm256_cvtps_epi32(value_2);
  __m256i llr_i32_3 = _mm256_cvtps_epi32(value_3);

  // Check bounds one more time: if value_X contains a NaN, its casting to int32_t is indeterminate - set it to zero.
  __m256i BOUND_UP  = _mm256_set1_epi32(log_likelihood_ratio::max().to_int());
  __m256i BOUND_LOW = _mm256_set1_epi32(log_likelihood_ratio::min().to_int());
  llr_i32_0         = check_bounds_epi32(llr_i32_0, BOUND_UP, BOUND_LOW);
  llr_i32_1         = check_bounds_epi32(llr_i32_1, BOUND_UP, BOUND_LOW);
  llr_i32_2         = check_bounds_epi32(llr_i32_2, BOUND_UP, BOUND_LOW);
  llr_i32_3         = check_bounds_epi32(llr_i32_3, BOUND_UP, BOUND_LOW);

  // Re-collocate SSE registers.
  __m256i llr_i32_0_ = _mm256_permute2f128_si256(llr_i32_0, llr_i32_1, 0x20);
  __m256i llr_i32_1_ = _mm256_permute2f128_si256(llr_i32_0, llr_i32_1, 0x31);
  __m256i llr_i32_2_ = _mm256_permute2f128_si256(llr_i32_2, llr_i32_3, 0x20);
  __m256i llr_i32_3_ = _mm256_permute2f128_si256(llr_i32_2, llr_i32_3, 0x31);

  // Conversion to 16 bit.
  __m256i llr_i16_0 = _mm256_packs_epi32(llr_i32_0_, llr_i32_1_);
  __m256i llr_i16_1 = _mm256_packs_epi32(llr_i32_2_, llr_i32_3_);

  // Re-collocate SSE registers.
  __m256i llr_i16_0_ = _mm256_permute2f128_si256(llr_i16_0, llr_i16_1, 0x20);
  __m256i llr_i16_1_ = _mm256_permute2f128_si256(llr_i16_0, llr_i16_1, 0x31);

  // Conversion to 8 bit.
  return _mm256_packs_epi16(llr_i16_0_, llr_i16_1_);
}

/// \brief Helper function to calculate an interval index from single-precision AVX register values.
/// \param[in] value          Input AVX register.
/// \param[in] interval_width Interval width.
/// \param[in] nof_intervals  Number of intervals.
/// \return An AVX register carrying eight signed 32-bit integers with the corresponding interval indexes.
inline __m256i compute_interval_idx(__m256 value, float interval_width, int nof_intervals)
{
  // Scale.
  value = _mm256_mul_ps(value, _mm256_set1_ps(1.0F / interval_width));

  // Round to the lowest integer.
  value = _mm256_round_ps(value, _MM_FROUND_FLOOR);

  // Convert to int32.
  __m256i idx = _mm256_cvtps_epi32(value);

  // Add interval offset.
  idx = _mm256_add_epi32(idx, _mm256_set1_epi32(nof_intervals / 2));

  // Clip index.
  idx = clip_epi32(idx, _mm256_set1_epi32(nof_intervals - 1), _mm256_setzero_si256());

  // Clip integer and return.
  return idx;
}

/// \brief Get values from a look-up table.
/// \param[in] table   Look-up table containing eight single-precision values.
/// \param[in] indexes AVX register containing eight indexes.
/// \return A single-precision AVX register containing the eight values corresponding to the indexes.
inline __m256 look_up_table(const std::array<float, 8>& table, __m256i indexes)
{
  return _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data()), indexes);
}

/// \brief Get values from a look-up table.
/// \param[in] table   Look-up table containing sixteen single-precision values.
/// \param[in] indexes AVX register containing eight indexes.
/// \return A single-precision AVX register containing the eight values corresponding to the indexes.
inline __m256 look_up_table(const std::array<float, 16>& table, __m256i indexes)
{
  // Get lower part of the table.
  __m256 lower = _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data()), indexes);

  // Get upper part of the table.
  __m256 upper = _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data() + 8), indexes);

  // Generate mask. Set to true if the MSB of the index is greater than zero.
  __m256i mask = _mm256_cmpgt_epi32(indexes, _mm256_set1_epi32(0b111));

  // Select upper or lower value.
  return _mm256_blendv_ps(lower, upper, (__m256)mask);
}

/// \brief Applies an interval function to a series of values.
/// \tparam Table             Look-up table type. All tables mut be of the same type.
/// \param[in] value          Single-precision AVX register with eight input values.
/// \param[in] rcp_noise      Single-precision AVX register with the reciprocal noise corresponding to the values.
/// \param[in] nof_intervals  Number of intervals.
/// \param[in] interval_width Interval width to quantify the interval indexes.
/// \param[in] slopes         Table with the slope of each interval.
/// \param[in] intercepts     Table with the interception points of each interval.
/// \return A single-precision AVX register containing the results of the interval function.
/// \remark The number of intervals must be lower than or equal to \c Table size.
template <typename Table>
inline __m256 interval_function(__m256       value,
                                __m256       rcp_noise,
                                float        interval_width,
                                unsigned     nof_intervals,
                                const Table& slopes,
                                const Table& intercepts)
{
  __m256i interval_index = mm256::compute_interval_idx(value, interval_width, nof_intervals);

  __m256 slope     = mm256::look_up_table(slopes, interval_index);
  __m256 intercept = mm256::look_up_table(intercepts, interval_index);

  return _mm256_mul_ps(_mm256_add_ps(_mm256_mul_ps(slope, value), intercept), rcp_noise);
}

/// \brief Safe division.
///
/// \return <tt>dividend / divisor</tt> if \c divisor is greater than zero, \c 0 otherwise.
inline __m256 safe_div(__m256 dividend, __m256 divisor)
{
  static const __m256 all_zero = _mm256_setzero_ps();
  // _CMP_GT_OQ: compare greater than, ordered (nan is false) and quite (no exceptions raised).
#if defined(__AVX512F__) && defined(__AVX512VL__)
  __mmask8 mask = _mm256_cmp_ps_mask(divisor, all_zero, _CMP_GT_OQ);
  return _mm256_maskz_div_ps(mask, dividend, divisor);
#else
  __m256 mask   = _mm256_cmp_ps(divisor, all_zero, _CMP_GT_OQ);
  __m256 result = _mm256_div_ps(dividend, divisor);
  return _mm256_blendv_ps(all_zero, result, mask);
#endif
}

} // namespace mm256
} // namespace srsran
