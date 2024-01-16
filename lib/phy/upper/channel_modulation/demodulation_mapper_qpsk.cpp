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
#include "demodulation_mapper_qpsk.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

#ifdef HAVE_NEON
#include "neon_helpers.h"
#endif // HAVE_NEON

using namespace srsran;

// Maximum (absolute) value considered for quantization. Larger values will be clipped.
static constexpr float RANGE_LIMIT_FLOAT = 24;

#ifdef HAVE_AVX2

static void demod_QPSK_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  __m256 symbols_0 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 0));
  __m256 symbols_1 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 4));
  __m256 symbols_2 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 8));
  __m256 symbols_3 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 12));

  // Load noise.
  __m256 noise_0 = _mm256_loadu_ps(noise_var + 0);
  __m256 noise_1 = _mm256_loadu_ps(noise_var + 8);

  // Compute noise inverses.
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

  // Calculate l_value.
  __m256 GAIN      = _mm256_set1_ps(2.0F * M_SQRT2f32);
  __m256 l_value_0 = _mm256_mul_ps(_mm256_mul_ps(GAIN, symbols_0), rcp_noise_0_);
  __m256 l_value_1 = _mm256_mul_ps(_mm256_mul_ps(GAIN, symbols_1), rcp_noise_1_);
  __m256 l_value_2 = _mm256_mul_ps(_mm256_mul_ps(GAIN, symbols_2), rcp_noise_2_);
  __m256 l_value_3 = _mm256_mul_ps(_mm256_mul_ps(GAIN, symbols_3), rcp_noise_3_);

  // Store result.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr),
                      mm256::quantize_ps(l_value_0, l_value_1, l_value_2, l_value_3, RANGE_LIMIT_FLOAT));
}

#endif // HAVE_AVX2

#ifdef HAVE_NEON

static void demod_QPSK_neon(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
{
  // Load symbols.
  float32x4_t symbols_0 = vld1q_f32(reinterpret_cast<const float*>(symbol + 0));
  float32x4_t symbols_1 = vld1q_f32(reinterpret_cast<const float*>(symbol + 2));
  float32x4_t symbols_2 = vld1q_f32(reinterpret_cast<const float*>(symbol + 4));
  float32x4_t symbols_3 = vld1q_f32(reinterpret_cast<const float*>(symbol + 6));

  // Load noise.
  float32x4_t noise_0 = vld1q_f32(noise_var + 0);
  float32x4_t noise_1 = vld1q_f32(noise_var + 4);

  // Compute noise inverses.
  float32x4_t rcp_noise_0 = neon::safe_div(vdupq_n_f32(1), noise_0);
  float32x4_t rcp_noise_1 = neon::safe_div(vdupq_n_f32(1), noise_1);

  // Repeat noise values for real and imaginary parts.
  float32x4x2_t noise_rep_0 = vzipq_f32(rcp_noise_0, rcp_noise_0);
  float32x4x2_t noise_rep_1 = vzipq_f32(rcp_noise_1, rcp_noise_1);
  // float32x4_t   rcp_noise_0_ = noise_rep_0.val[0];
  // float32x4_t   rcp_noise_1_ = noise_rep_0.val[1];
  // float32x4_t   rcp_noise_2_ = noise_rep_1.val[0];
  // float32x4_t   rcp_noise_3_ = noise_rep_1.val[1];

  // Calculate l_value.
  float32x4_t GAIN      = vdupq_n_f32(2.0F * M_SQRT2f32);
  float32x4_t l_value_0 = vmulq_f32(vmulq_f32(GAIN, symbols_0), noise_rep_0.val[0]);
  float32x4_t l_value_1 = vmulq_f32(vmulq_f32(GAIN, symbols_1), noise_rep_0.val[1]);
  float32x4_t l_value_2 = vmulq_f32(vmulq_f32(GAIN, symbols_2), noise_rep_1.val[0]);
  float32x4_t l_value_3 = vmulq_f32(vmulq_f32(GAIN, symbols_3), noise_rep_1.val[1]);

  // Store result.
  vst1q_s8(reinterpret_cast<int8_t*>(llr),
           neon::quantize_f32(l_value_0, l_value_1, l_value_2, l_value_3, RANGE_LIMIT_FLOAT));
}

#endif // HAVE_NEON

static log_likelihood_ratio demod_QPSK_symbol(float x, float noise_var)
{
  // Note: "noise_var > 0" is false also when "noise_var" is NaN.
  if (!(noise_var > 0)) {
    return log_likelihood_ratio(0);
  }
  float l_value = 2.0F * M_SQRT2f32 * x / noise_var;
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

void srsran::demodulate_soft_QPSK(span<log_likelihood_ratio> llrs,
                                  span<const cf_t>           symbols,
                                  span<const float>          noise_vars)
{
  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  std::size_t           symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 16 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 16) * 16; symbol_index != symbol_index_end;
       symbol_index += 16) {
    demod_QPSK_avx2(llr_it, symbols_it, noise_it);

    llr_it += 32;
    symbols_it += 16;
    noise_it += 16;
  }
#endif // HAVE_AVX2

#ifdef HAVE_NEON
  // For NEON, it generates 16 LLRs simultaneously. The input is read in batches of 8 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 8) * 8; symbol_index != symbol_index_end; symbol_index += 8) {
    demod_QPSK_neon(llr_it, symbols_it, noise_it);

    llr_it += 16;
    symbols_it += 8;
    noise_it += 8;
  }
#endif // HAVE_NEON

  // Process remainder symbols with the generic algorithm.
  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    cf_t this_symbol = *symbols_it++;
    *llr_it++        = demod_QPSK_symbol(std::real(this_symbol), *noise_it);
    *llr_it++        = demod_QPSK_symbol(std::imag(this_symbol), *noise_it);
    ++noise_it;
  }
}