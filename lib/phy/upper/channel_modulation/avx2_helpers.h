/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <immintrin.h>

namespace srsgnb {

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

/// \brief Clips the values of a single-precision AVX register.
///
/// The values greater than \c range_ceil or lower than \c range_floor are substituted by their corresponding range
/// limits.
///
/// \param[in] value       Input values.
/// \param[in] range_ceil  Ceiling values.
/// \param[in] range_floor Floor values.
/// \return A single-precision AVX register containing the clipped values.
inline __m256i clip_epi32(__m256i value, __m256i range_ceil, __m256i range_floor)
{
  value = _mm256_blendv_epi8(value, range_ceil, _mm256_cmpgt_epi32(value, range_ceil));
  value = _mm256_blendv_epi8(value, range_floor, _mm256_cmpgt_epi32(range_floor, value));
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
  // Clipping.
  __m256 RANGE_CEIL  = _mm256_set1_ps(range_limit);
  __m256 RANGE_FLOOR = _mm256_set1_ps(-range_limit);
  value_0            = clip_ps(value_0, RANGE_CEIL, RANGE_FLOOR);
  value_1            = clip_ps(value_1, RANGE_CEIL, RANGE_FLOOR);
  value_2            = clip_ps(value_2, RANGE_CEIL, RANGE_FLOOR);
  value_3            = clip_ps(value_3, RANGE_CEIL, RANGE_FLOOR);

  // Scale and round to the nearest integer.
  __m256 SCALE = _mm256_set1_ps(static_cast<float>(log_likelihood_ratio::max().to_int()) / range_limit);
  value_0      = _mm256_round_ps(_mm256_mul_ps(value_0, SCALE), _MM_FROUND_NINT);
  value_1      = _mm256_round_ps(_mm256_mul_ps(value_1, SCALE), _MM_FROUND_NINT);
  value_2      = _mm256_round_ps(_mm256_mul_ps(value_2, SCALE), _MM_FROUND_NINT);
  value_3      = _mm256_round_ps(_mm256_mul_ps(value_3, SCALE), _MM_FROUND_NINT);

  // Convert to 32 bit.
  __m256i llr_i32_0 = _mm256_cvtps_epi32(value_0);
  __m256i llr_i32_1 = _mm256_cvtps_epi32(value_1);
  __m256i llr_i32_2 = _mm256_cvtps_epi32(value_2);
  __m256i llr_i32_3 = _mm256_cvtps_epi32(value_3);

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

inline __m256i compute_interval_idx(__m256 value, float interval_width, int nof_intervals)
{
  __m256i nof_intervals_minus_one = _mm256_set1_epi32(nof_intervals - 1);
  __m256i nof_intervals_half      = _mm256_set1_epi32(nof_intervals / 2);

  // Scale.
  value = _mm256_mul_ps(value, _mm256_set1_ps(1.0F / interval_width));

  // Round to lowest integer.
  value = _mm256_round_ps(value, _MM_FROUND_FLOOR);

  // Convert to int32.
  __m256i idx = _mm256_cvtps_epi32(value);

  // Add interval offset.
  idx = _mm256_add_epi32(idx, nof_intervals_half);

  // Clip index.
  idx = clip_epi32(idx, nof_intervals_minus_one, _mm256_setzero_si256());

  // Clip integer and return.
  return idx;
}

template <typename Container>
inline __m256 look_up_table(const Container& table, __m256i indexes)
{
  // Get 3 bit LSB.
  __m256i indexes_lsb = _mm256_and_si256(indexes, _mm256_set1_epi32(0b111));

  // Get MSBs.
  __m256i indexes_msb = _mm256_srai_epi32(indexes, 3);

  // Initialise result.
  __m256 result = _mm256_setzero_ps();

  for (int index_msb = 0, index_msb_end = table.size() / 8; index_msb != index_msb_end; ++index_msb) {
    // Load eight values.
    __m256 small_table = _mm256_loadu_ps(&table[index_msb * 8]);

    // Select values from eight value table.
    __m256 result_lut = _mm256_permutevar8x32_ps(small_table, indexes_lsb);

    // Select mask if the MSB of the index matches.
    __m256i mask = _mm256_cmpeq_epi32(_mm256_set1_epi32(index_msb), indexes_msb);

    // Selects values.
    result = _mm256_blendv_ps(result, result_lut, (__m256)mask);
  }

  return result;
}

template <>
inline __m256 look_up_table(const std::array<float, 8>& table, __m256i indexes)
{
  return _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data()), indexes);
}

template <>
inline __m256 look_up_table(const std::array<float, 16>& table, __m256i indexes)
{
  // Get 3 bit LSB.
  __m256i indexes_lsb = _mm256_and_si256(indexes, _mm256_set1_epi32(0b111));

  // Get MSBs.
  __m256i indexes_msb = _mm256_srai_epi32(indexes, 3);

  // Get lower part of the table.
  __m256 lower = _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data()), indexes_lsb);

  // Get upper part of the table.
  __m256 upper = _mm256_permutevar8x32_ps(_mm256_loadu_ps(table.data() + 8), indexes_lsb);

  // Generate mask. Set to true if the MSB of the index is greater than zero.
  __m256i mask = _mm256_cmpgt_epi32(indexes_msb, _mm256_setzero_si256());

  // Select upper or lower value.
  return _mm256_blendv_ps(lower, upper, (__m256)mask);
}

} // namespace mm256
} // namespace srsgnb