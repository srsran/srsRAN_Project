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
#include "demodulation_mapper_qam64.h"
#include "demodulation_mapper_intervals.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

#ifdef HAVE_NEON
#include "neon_helpers.h"
#endif // HAVE_NEON

using namespace srsran;

// Square root of 1/42.
static const float M_SQRT1_42 = 1.0F / std::sqrt(42.0F);

// Maximum (absolute) value considered for quantization. Larger values will be clipped.
static constexpr float RANGE_LIMIT_FLOAT = 20;

static constexpr int              NOF_INTERVALS_01  = 8;
static const float                INTERVAL_WIDTH_01 = 2 * M_SQRT1_42;
static const std::array<float, 8> SLOPE_01          = {16 * M_SQRT1_42,
                                                       12 * M_SQRT1_42,
                                                       8 * M_SQRT1_42,
                                                       4 * M_SQRT1_42,
                                                       4 * M_SQRT1_42,
                                                       8 * M_SQRT1_42,
                                                       12 * M_SQRT1_42,
                                                       16 * M_SQRT1_42};
static const std::array<float, 8> INTERCEPT_01 =
    {24.0F / 21, 12.0F / 21, 4.0F / 21, 0.0F, 0.0F, -4.0F / 21, -12.0F / 21, -24.0F / 21};

constexpr int                     NOF_INTERVALS_23  = 8;
const float                       INTERVAL_WIDTH_23 = 2 * M_SQRT1_42;
static const std::array<float, 8> SLOPE_23          = {8 * M_SQRT1_42,
                                                       4 * M_SQRT1_42,
                                                       4 * M_SQRT1_42,
                                                       8 * M_SQRT1_42,
                                                       -8 * M_SQRT1_42,
                                                       -4 * M_SQRT1_42,
                                                       -4 * M_SQRT1_42,
                                                       -8 * M_SQRT1_42};
static const std::array<float, 8> INTERCEPT_23 =
    {20.0F / 21, 8.0F / 21, 8.0F / 21, 12.0F / 21, 12.0F / 21, 8.0F / 21, 8.0F / 21, 20.0F / 21};

static constexpr int              NOF_INTERVALS_45  = 4;
static const float                INTERVAL_WIDTH_45 = 4 * M_SQRT1_42;
static const std::array<float, 8> SLOPE_45 =
    {4 * M_SQRT1_42, -4 * M_SQRT1_42, 4 * M_SQRT1_42, -4 * M_SQRT1_42, 0, 0, 0, 0};
static const std::array<float, 8> INTERCEPT_45 = {12.0F / 21, -4.0F / 21, -4.0F / 21, 12.0F / 21, 0, 0, 0, 0};

#ifdef HAVE_AVX2

static void demod_QAM64_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  __m256 symbols_0 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 0));
  __m256 symbols_1 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 4));
  __m256 symbols_2 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 8));
  __m256 symbols_3 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 12));

  // Load noise.
  __m256 noise_0 = _mm256_loadu_ps(noise_var + 0);
  __m256 noise_1 = _mm256_loadu_ps(noise_var + 8);

  // Make noise reciprocal.
  __m256 rcp_noise_0 = mm256::safe_div(_mm256_set1_ps(1), noise_0);
  __m256 rcp_noise_1 = mm256::safe_div(_mm256_set1_ps(1), noise_1);

  // Repeat noise values for real and imaginary parts.
  __m256 rcp_noise_3 = _mm256_unpackhi_ps(rcp_noise_1, rcp_noise_1);
  __m256 rcp_noise_2 = _mm256_unpacklo_ps(rcp_noise_1, rcp_noise_1);
  rcp_noise_1        = _mm256_unpackhi_ps(rcp_noise_0, rcp_noise_0);
  rcp_noise_0        = _mm256_unpacklo_ps(rcp_noise_0, rcp_noise_0);

  // Re-collocate SSE registers.
  __m256 rcp_noise_0_ = _mm256_permute2f128_ps(rcp_noise_0, rcp_noise_1, 0x20);
  __m256 rcp_noise_1_ = _mm256_permute2f128_ps(rcp_noise_0, rcp_noise_1, 0x31);
  __m256 rcp_noise_2_ = _mm256_permute2f128_ps(rcp_noise_2, rcp_noise_3, 0x20);
  __m256 rcp_noise_3_ = _mm256_permute2f128_ps(rcp_noise_2, rcp_noise_3, 0x31);

  // Calculate l_value for bits 0 and 1.
  __m256 l_value_01_0 =
      mm256::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  __m256 l_value_01_1 =
      mm256::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  __m256 l_value_01_2 =
      mm256::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  __m256 l_value_01_3 =
      mm256::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);

  // Calculate l_value for bits 2 and 3.
  __m256 l_value_23_0 =
      mm256::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  __m256 l_value_23_1 =
      mm256::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  __m256 l_value_23_2 =
      mm256::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  __m256 l_value_23_3 =
      mm256::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);

  // Calculate l_value for bits 4 and 5.
  __m256 l_value_45_0 =
      mm256::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  __m256 l_value_45_1 =
      mm256::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  __m256 l_value_45_2 =
      mm256::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  __m256 l_value_45_3 =
      mm256::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);

  // Re-collocate values.
  __m256 l_value_0  = _mm256_shuffle_ps(l_value_01_0, l_value_23_0, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_1  = _mm256_shuffle_ps(l_value_45_0, l_value_01_0, _MM_SHUFFLE(3, 2, 1, 0));
  __m256 l_value_2  = _mm256_shuffle_ps(l_value_23_0, l_value_45_0, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 l_value_3  = _mm256_shuffle_ps(l_value_01_1, l_value_23_1, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_4  = _mm256_shuffle_ps(l_value_45_1, l_value_01_1, _MM_SHUFFLE(3, 2, 1, 0));
  __m256 l_value_5  = _mm256_shuffle_ps(l_value_23_1, l_value_45_1, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 l_value_6  = _mm256_shuffle_ps(l_value_01_2, l_value_23_2, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_7  = _mm256_shuffle_ps(l_value_45_2, l_value_01_2, _MM_SHUFFLE(3, 2, 1, 0));
  __m256 l_value_8  = _mm256_shuffle_ps(l_value_23_2, l_value_45_2, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 l_value_9  = _mm256_shuffle_ps(l_value_01_3, l_value_23_3, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_10 = _mm256_shuffle_ps(l_value_45_3, l_value_01_3, _MM_SHUFFLE(3, 2, 1, 0));
  __m256 l_value_11 = _mm256_shuffle_ps(l_value_23_3, l_value_45_3, _MM_SHUFFLE(3, 2, 3, 2));

  // Re-collocate SSE registers.
  __m256 l_value_0_  = _mm256_permute2f128_ps(l_value_0, l_value_1, 0x20);
  __m256 l_value_1_  = _mm256_permute2f128_ps(l_value_2, l_value_0, 0x30);
  __m256 l_value_2_  = _mm256_permute2f128_ps(l_value_1, l_value_2, 0x31);
  __m256 l_value_3_  = _mm256_permute2f128_ps(l_value_3, l_value_4, 0x20);
  __m256 l_value_4_  = _mm256_permute2f128_ps(l_value_5, l_value_3, 0x30);
  __m256 l_value_5_  = _mm256_permute2f128_ps(l_value_4, l_value_5, 0x31);
  __m256 l_value_6_  = _mm256_permute2f128_ps(l_value_6, l_value_7, 0x20);
  __m256 l_value_7_  = _mm256_permute2f128_ps(l_value_8, l_value_6, 0x30);
  __m256 l_value_8_  = _mm256_permute2f128_ps(l_value_7, l_value_8, 0x31);
  __m256 l_value_9_  = _mm256_permute2f128_ps(l_value_9, l_value_10, 0x20);
  __m256 l_value_10_ = _mm256_permute2f128_ps(l_value_11, l_value_9, 0x30);
  __m256 l_value_11_ = _mm256_permute2f128_ps(l_value_10, l_value_11, 0x31);

  // Store result.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr + 0),
                      mm256::quantize_ps(l_value_0_, l_value_1_, l_value_2_, l_value_3_, RANGE_LIMIT_FLOAT));
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr + 32),
                      mm256::quantize_ps(l_value_4_, l_value_5_, l_value_6_, l_value_7_, RANGE_LIMIT_FLOAT));
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr + 64),
                      mm256::quantize_ps(l_value_8_, l_value_9_, l_value_10_, l_value_11_, RANGE_LIMIT_FLOAT));
}
#endif // HAVE_AVX2

#ifdef HAVE_NEON

static void demod_QAM64_neon(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  float32x4_t symbols_0 = vld1q_f32(reinterpret_cast<const float*>(symbol + 0));
  float32x4_t symbols_1 = vld1q_f32(reinterpret_cast<const float*>(symbol + 2));
  float32x4_t symbols_2 = vld1q_f32(reinterpret_cast<const float*>(symbol + 4));
  float32x4_t symbols_3 = vld1q_f32(reinterpret_cast<const float*>(symbol + 6));

  // Load noise.
  float32x4_t noise_0 = vld1q_f32(noise_var);
  float32x4_t noise_1 = vld1q_f32(noise_var + 4);

  // Make noise reciprocal.
  float32x4_t rcp_noise_0 = neon::safe_div(vdupq_n_f32(1.0f), noise_0);
  float32x4_t rcp_noise_1 = neon::safe_div(vdupq_n_f32(1.0f), noise_1);

  // Repeat noise values for real and imaginary parts.
  float32x4x2_t noise_rep_0  = vzipq_f32(rcp_noise_0, rcp_noise_0);
  float32x4_t   rcp_noise_0_ = noise_rep_0.val[0];
  float32x4_t   rcp_noise_1_ = noise_rep_0.val[1];
  float32x4x2_t noise_rep_1  = vzipq_f32(rcp_noise_1, rcp_noise_1);
  float32x4_t   rcp_noise_2_ = noise_rep_1.val[0];
  float32x4_t   rcp_noise_3_ = noise_rep_1.val[1];

  // Calculate l_value for bits 0 and 1.
  float32x4_t l_value_01_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  float32x4_t l_value_01_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  float32x4_t l_value_01_2 =
      neon::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  float32x4_t l_value_01_3 =
      neon::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);

  // Calculate l_value for bits 2 and 3.
  float32x4_t l_value_23_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  float32x4_t l_value_23_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  float32x4_t l_value_23_2 =
      neon::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  float32x4_t l_value_23_3 =
      neon::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);

  // Calculate l_value for bits 4 and 5.
  float32x4_t l_value_45_0 =
      neon::interval_function(symbols_0, rcp_noise_0_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  float32x4_t l_value_45_1 =
      neon::interval_function(symbols_1, rcp_noise_1_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  float32x4_t l_value_45_2 =
      neon::interval_function(symbols_2, rcp_noise_2_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  float32x4_t l_value_45_3 =
      neon::interval_function(symbols_3, rcp_noise_3_, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);

  // Re-collocate values.
  float32x4_t l_value_0 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_0), vreinterpretq_u64_f32(l_value_23_0)));
  float32x4_t l_value_1 = vreinterpretq_f32_u64(
      vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_0), vreinterpretq_u64_f32(vextq_f32(l_value_01_0, l_value_01_0, 2))));
  float32x4_t l_value_2 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_23_0), vreinterpretq_u64_f32(l_value_45_0)));
  //
  float32x4_t l_value_3 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_1), vreinterpretq_u64_f32(l_value_23_1)));
  float32x4_t l_value_4 = vreinterpretq_f32_u64(
      vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_1), vreinterpretq_u64_f32(vextq_f32(l_value_01_1, l_value_01_1, 2))));
  float32x4_t l_value_5 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_23_1), vreinterpretq_u64_f32(l_value_45_1)));
  //
  float32x4_t l_value_6 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_2), vreinterpretq_u64_f32(l_value_23_2)));
  float32x4_t l_value_7 = vreinterpretq_f32_u64(
      vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_2), vreinterpretq_u64_f32(vextq_f32(l_value_01_2, l_value_01_2, 2))));
  float32x4_t l_value_8 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_23_2), vreinterpretq_u64_f32(l_value_45_2)));
  //
  float32x4_t l_value_9 =
      vreinterpretq_f32_u64(vtrn1q_u64(vreinterpretq_u64_f32(l_value_01_3), vreinterpretq_u64_f32(l_value_23_3)));
  float32x4_t l_value_10 = vreinterpretq_f32_u64(
      vtrn1q_u64(vreinterpretq_u64_f32(l_value_45_3), vreinterpretq_u64_f32(vextq_f32(l_value_01_3, l_value_01_3, 2))));
  float32x4_t l_value_11 =
      vreinterpretq_f32_u64(vtrn2q_u64(vreinterpretq_u64_f32(l_value_23_3), vreinterpretq_u64_f32(l_value_45_3)));

  // Store result.
  vst1q_s8(reinterpret_cast<int8_t*>(llr),
           neon::quantize_f32(l_value_0, l_value_1, l_value_2, l_value_3, RANGE_LIMIT_FLOAT));
  vst1q_s8(reinterpret_cast<int8_t*>(llr + 16),
           neon::quantize_f32(l_value_4, l_value_5, l_value_6, l_value_7, RANGE_LIMIT_FLOAT));
  vst1q_s8(reinterpret_cast<int8_t*>(llr + 32),
           neon::quantize_f32(l_value_8, l_value_9, l_value_10, l_value_11, RANGE_LIMIT_FLOAT));
}
#endif // HAVE_NEON

static log_likelihood_ratio demod_64QAM_symbol_01(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

static log_likelihood_ratio demod_64QAM_symbol_23(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

static log_likelihood_ratio demod_64QAM_symbol_45(float value, float rcp_noise_var)
{
  float l_value = interval_function(value, rcp_noise_var, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

void srsran::demodulate_soft_QAM64(span<log_likelihood_ratio> llrs,
                                   span<const cf_t>           symbols,
                                   span<const float>          noise_vars)
{
  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  std::size_t           symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 96 LLRs simultaneously. The input is read in batches of 16 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 16) * 16; symbol_index != symbol_index_end;
       symbol_index += 16) {
    demod_QAM64_avx2(llr_it, symbols_it, noise_it);

    llr_it += 96;
    symbols_it += 16;
    noise_it += 16;
  }
#endif // HAVE_AVX2

#ifdef HAVE_NEON
  // For NEON, it generates 48 LLRs simultaneously. The input is read in batches of 8 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 8) * 8; symbol_index != symbol_index_end; symbol_index += 8) {
    demod_QAM64_neon(llr_it, symbols_it, noise_it);

    llr_it += 48;
    symbols_it += 8;
    noise_it += 8;
  }
#endif // HAVE_NEON

  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    //  Set all LLR to zero if the symbol is near zero.
    if (is_near_zero(*symbols_it)) {
      for (unsigned i_bit = 0; i_bit != 6; ++i_bit) {
        *llr_it++ = 0;
      }
      ++symbols_it;
      ++noise_it;
      continue;
    }

    float rcp_noise = 0;
    // Note: "*noise_it > 0" is false also when "*noise_it" is NaN.
    if (*noise_it > 0) {
      rcp_noise = 1 / (*noise_it);
    }
    *llr_it++ = demod_64QAM_symbol_01(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_64QAM_symbol_01(std::imag(*symbols_it), rcp_noise);
    *llr_it++ = demod_64QAM_symbol_23(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_64QAM_symbol_23(std::imag(*symbols_it), rcp_noise);
    *llr_it++ = demod_64QAM_symbol_45(std::real(*symbols_it), rcp_noise);
    *llr_it++ = demod_64QAM_symbol_45(std::imag(*symbols_it), rcp_noise);
    ++symbols_it;
    ++noise_it;
  }
}
