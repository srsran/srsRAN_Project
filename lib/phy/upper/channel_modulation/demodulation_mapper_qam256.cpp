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
#include "demodulation_mapper_qam256.h"
#include "demodulation_mapper_intervals.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

#ifdef HAVE_NEON
#include "neon_helpers.h"
#endif // HAVE_NEON

using namespace srsran;

// Square root of 1/170.
const float M_SQRT1_170 = 1.0F / std::sqrt(170.0F);

// Maximum (absolute) value considered for quantization. Larger values will be clipped.
constexpr float RANGE_LIMIT_FLOAT = 20;

const float                               INTERVAL_WIDTH_01 = 2 * M_SQRT1_170;
constexpr unsigned                        NOF_INTERVALS_01  = 16;
const std::array<float, NOF_INTERVALS_01> SLOPE_01          = {32 * M_SQRT1_170,
                                                               28 * M_SQRT1_170,
                                                               24 * M_SQRT1_170,
                                                               20 * M_SQRT1_170,
                                                               16 * M_SQRT1_170,
                                                               12 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               12 * M_SQRT1_170,
                                                               16 * M_SQRT1_170,
                                                               20 * M_SQRT1_170,
                                                               24 * M_SQRT1_170,
                                                               28 * M_SQRT1_170,
                                                               32 * M_SQRT1_170};
const std::array<float, NOF_INTERVALS_01> INTERCEPT_01      = {112.0F / 85,
                                                               84.0F / 85,
                                                               60.0F / 85,
                                                               40.0F / 85,
                                                               24.0F / 85,
                                                               12.0F / 85,
                                                               4.0F / 85,
                                                               0.0F,
                                                               0.0F,
                                                               -4.0F / 85,
                                                               -12.0F / 85,
                                                               -24.0F / 85,
                                                               -40.0F / 85,
                                                               -60.0F / 85,
                                                               -84.0F / 85,
                                                               -112.0F / 85};
const float                               INTERVAL_WIDTH_23 = 2 * M_SQRT1_170;
constexpr unsigned                        NOF_INTERVALS_23  = 16;
const std::array<float, NOF_INTERVALS_23> SLOPE_23          = {16 * M_SQRT1_170,
                                                               12 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               12 * M_SQRT1_170,
                                                               16 * M_SQRT1_170,
                                                               -16 * M_SQRT1_170,
                                                               -12 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170,
                                                               -12 * M_SQRT1_170,
                                                               -16 * M_SQRT1_170};
const std::array<float, NOF_INTERVALS_23> INTERCEPT_23      = {88.0F / 85,
                                                               60.0F / 85,
                                                               36.0F / 85,
                                                               16.0F / 85,
                                                               16.0F / 85,
                                                               28.0F / 85,
                                                               36.0F / 85,
                                                               40.0F / 85,
                                                               40.0F / 85,
                                                               36.0F / 85,
                                                               28.0F / 85,
                                                               16.0F / 85,
                                                               16.0F / 85,
                                                               36.0F / 85,
                                                               60.0F / 85,
                                                               88.0F / 85};
const float                               INTERVAL_WIDTH_45 = 2 * M_SQRT1_170;
constexpr unsigned                        NOF_INTERVALS_45  = 16;
const std::array<float, NOF_INTERVALS_45> SLOPE_45          = {8 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               8 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               -8 * M_SQRT1_170};
const std::array<float, NOF_INTERVALS_45> INTERCEPT_45      = {52.0F / 85,
                                                               24.0F / 85,
                                                               24.0F / 85,
                                                               44.0F / 85,
                                                               -20.0F / 85,
                                                               -8.0F / 85,
                                                               -8.0F / 85,
                                                               -12.0F / 85,
                                                               -12.0F / 85,
                                                               -8.0F / 85,
                                                               -8.0F / 85,
                                                               -20.0F / 85,
                                                               44.0F / 85,
                                                               24.0F / 85,
                                                               24.0F / 85,
                                                               52.0F / 85};
const float                               INTERVAL_WIDTH_67 = 4 * M_SQRT1_170;
constexpr unsigned                        NOF_INTERVALS_67  = 8;
const std::array<float, NOF_INTERVALS_67> SLOPE_67          = {4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170,
                                                               4 * M_SQRT1_170,
                                                               -4 * M_SQRT1_170};
const std::array<float, NOF_INTERVALS_67> INTERCEPT_67 =
    {28.0F / 85, -20.0F / 85, 12.0F / 85, -4.0F / 85, -4.0F / 85, 12.0F / 85, -20.0F / 85, 28.0F / 85};

#ifdef HAVE_AVX2
static void demod_QAM256_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  __m256 symbols = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol));

  // Load noise. 4 values only.
  __m128 noise = _mm_loadu_ps(noise_var);

  // Make noise reciprocal.
  // _CMP_GT_OQ: compare greater than, ordered (nan is false) and quite (no exceptions raised).
  __m128 mask      = _mm_cmp_ps(noise, _mm_set1_ps(0), _CMP_GT_OQ);
  __m128 rcp_noise = _mm_div_ps(_mm_set1_ps(1), noise);
  rcp_noise        = _mm_blendv_ps(_mm_set1_ps(0), rcp_noise, mask);

  // Repeat noise values for real and imaginary parts and put the results in an AVX register.
  __m256 rcp_noise_ = _mm256_set_m128(_mm_unpackhi_ps(rcp_noise, rcp_noise), _mm_unpacklo_ps(rcp_noise, rcp_noise));

  // Calculate l_value for bits 0 and 1.
  __m256 l_value_01 =
      mm256::interval_function(symbols, rcp_noise_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);

  // Calculate l_value for bits 2 and 3.
  __m256 l_value_23 =
      mm256::interval_function(symbols, rcp_noise_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);

  // Calculate l_value for bits 4 and 5.
  __m256 l_value_45 =
      mm256::interval_function(symbols, rcp_noise_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);

  // Calculate l_value for bits 6 and 7.
  __m256 l_value_67 =
      mm256::interval_function(symbols, rcp_noise_, INTERVAL_WIDTH_67, NOF_INTERVALS_67, SLOPE_67, INTERCEPT_67);

  // Re-collocate values.
  __m256 l_value_0 = _mm256_shuffle_ps(l_value_01, l_value_23, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_1 = _mm256_shuffle_ps(l_value_45, l_value_67, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_2 = _mm256_shuffle_ps(l_value_01, l_value_23, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 l_value_3 = _mm256_shuffle_ps(l_value_45, l_value_67, _MM_SHUFFLE(3, 2, 3, 2));

  // Re-collocate SSE registers.
  __m256 l_value_0_ = _mm256_permute2f128_ps(l_value_0, l_value_1, 0x20);
  __m256 l_value_1_ = _mm256_permute2f128_ps(l_value_2, l_value_3, 0x20);
  __m256 l_value_2_ = _mm256_permute2f128_ps(l_value_0, l_value_1, 0x31);
  __m256 l_value_3_ = _mm256_permute2f128_ps(l_value_2, l_value_3, 0x31);

  // Store result.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr),
                      mm256::quantize_ps(l_value_0_, l_value_1_, l_value_2_, l_value_3_, RANGE_LIMIT_FLOAT));
}
#endif // HAVE_AVX2

#ifdef HAVE_NEON
static void demod_QAM256_neon(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  float32x4_t symbols_0 = vld1q_f32(reinterpret_cast<const float*>(symbol + 0));
  float32x4_t symbols_1 = vld1q_f32(reinterpret_cast<const float*>(symbol + 2));

  // Load noise.
  float32x4_t noise_0 = vld1q_f32(noise_var);

  // Make noise reciprocal.
  float32x4_t rcp_noise_0 = neon::safe_div(vdupq_n_f32(1.0f), noise_0);

  // Repeat noise values for real and imaginary parts.
  float32x4x2_t noise_rep_0  = vzipq_f32(rcp_noise_0, rcp_noise_0);
  float32x4_t   rcp_noise_0_ = noise_rep_0.val[0];
  float32x4_t   rcp_noise_1_ = noise_rep_0.val[1];

  // Calculate l_value for bits 0 and 1.
  float32x4_t l_value_01_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  float32x4_t l_value_01_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);

  // Calculate l_value for bits 2 and 3.
  float32x4_t l_value_23_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  float32x4_t l_value_23_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);

  // Calculate l_value for bits 4 and 5.
  float32x4_t l_value_45_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  float32x4_t l_value_45_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);

  // Calculate l_value for bits 6 and 7.
  float32x4_t l_value_67_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_67, NOF_INTERVALS_67, SLOPE_67, INTERCEPT_67);
  float32x4_t l_value_67_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_67, NOF_INTERVALS_67, SLOPE_67, INTERCEPT_67);

  // Re-collocate values.
  float32x4_t l_value_0 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_0), vreinterpretq_u64_f32(l_value_23_0)));
  float32x4_t l_value_1 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_0), vreinterpretq_u64_f32(l_value_67_0)));
  float32x4_t l_value_2 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_01_0), vreinterpretq_u64_f32(l_value_23_0)));
  float32x4_t l_value_3 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_45_0), vreinterpretq_u64_f32(l_value_67_0)));
  //
  float32x4_t l_value_4 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_1), vreinterpretq_u64_f32(l_value_23_1)));
  float32x4_t l_value_5 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_1), vreinterpretq_u64_f32(l_value_67_1)));
  float32x4_t l_value_6 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_01_1), vreinterpretq_u64_f32(l_value_23_1)));
  float32x4_t l_value_7 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_45_1), vreinterpretq_u64_f32(l_value_67_1)));

  // Store result.
  vst1q_s8(reinterpret_cast<int8_t*>(llr),
           neon::quantize_f32(l_value_0, l_value_1, l_value_2, l_value_3, RANGE_LIMIT_FLOAT));
  vst1q_s8(reinterpret_cast<int8_t*>(llr + 16),
           neon::quantize_f32(l_value_4, l_value_5, l_value_6, l_value_7, RANGE_LIMIT_FLOAT));
}
#endif // HAVE_NEON

static log_likelihood_ratio demod_256QAM_symbol_01(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

static log_likelihood_ratio demod_256QAM_symbol_23(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

static log_likelihood_ratio demod_256QAM_symbol_45(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

static log_likelihood_ratio demod_256QAM_symbol_67(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_67, NOF_INTERVALS_67, SLOPE_67, INTERCEPT_67);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

void srsran::demodulate_soft_QAM256(span<log_likelihood_ratio> llrs,
                                    span<const cf_t>           symbols,
                                    span<const float>          noise_vars)
{
  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  std::size_t           symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 4 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 4) * 4; symbol_index != symbol_index_end; symbol_index += 4) {
    demod_QAM256_avx2(llr_it, symbols_it, noise_it);

    llr_it += 32;
    symbols_it += 4;
    noise_it += 4;
  }
#endif // HAVE_AVX2

#ifdef HAVE_NEON
  // For NEON, it generates 32 LLRs simultaneously. The input is read in batches of 4 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 4) * 4; symbol_index != symbol_index_end; symbol_index += 4) {
    demod_QAM256_neon(llr_it, symbols_it, noise_it);

    llr_it += 32;
    symbols_it += 4;
    noise_it += 4;
  }
#endif // HAVE_NEON

  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    //  Set all LLR to zero if the symbol is near zero.
    if (is_near_zero(*symbols_it)) {
      for (unsigned i_bit = 0; i_bit != 8; ++i_bit) {
        *llr_it++ = 0;
      }
      ++symbols_it;
      ++noise_it;
      continue;
    }

    float rcp_noise = 0;
    // Note: "noise_var > 0" is false also when "noise_var" is NaN.
    if (*noise_it > 0) {
      rcp_noise = 1 / (*noise_it);
    }
    *llr_it++ = demod_256QAM_symbol_01(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_01(std::imag(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_23(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_23(std::imag(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_45(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_45(std::imag(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_67(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_256QAM_symbol_67(std::imag(*symbols_it), rcp_noise);
    ++symbols_it;
    ++noise_it;
  }
}
