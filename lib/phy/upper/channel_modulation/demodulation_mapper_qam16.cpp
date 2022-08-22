/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "demodulation_mapper_qam16.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

using namespace srsgnb;

namespace {

// Square root of 1/10.
static const float M_SQRT1_10 = 1.0F / std::sqrt(10.0F);

#ifdef HAVE_AVX2
inline void demod_QAM16_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var, float range_limit)
{
  // Load symbols.
  __m256 symbols_0 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 0));
  __m256 symbols_1 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 4));

  __m256 abs_symbols_0 = mm256::abs_ps(symbols_0);
  __m256 abs_symbols_1 = mm256::abs_ps(symbols_1);

  // Load noise.
  __m256 noise_0 = _mm256_loadu_ps(noise_var + 0);

  // Make noise reciprocal.
  __m256 rcp_noise_0 = _mm256_div_ps(_mm256_set1_ps(1), noise_0);

  // Repeat noise values for real and imaginary parts.
  __m256 rcp_noise_1 = _mm256_unpackhi_ps(rcp_noise_0, rcp_noise_0);
  rcp_noise_0        = _mm256_unpacklo_ps(rcp_noise_0, rcp_noise_0);

  // Re-collocate SSE registers.
  __m256 rcp_noise_0_ = _mm256_permute2f128_ps(rcp_noise_0, rcp_noise_1, 0x20);
  __m256 rcp_noise_1_ = _mm256_permute2f128_ps(rcp_noise_0, rcp_noise_1, 0x31);

  // Calculate l_value for bits 0 and 1.
  __m256 GAIN_FIRST         = _mm256_set1_ps(4.0F * M_SQRT1_10);
  __m256 GAIN_LARGE         = _mm256_set1_ps(2.0F);
  __m256 CONST_0_8          = _mm256_set1_ps(0.8F);
  __m256 l_value_01_first_0 = _mm256_mul_ps(GAIN_FIRST, symbols_0);
  __m256 l_value_01_first_1 = _mm256_mul_ps(GAIN_FIRST, symbols_1);
  __m256 l_value_01_second_0 =
      _mm256_sub_ps(_mm256_mul_ps(GAIN_LARGE, l_value_01_first_0), mm256::copysign_ps(CONST_0_8, symbols_0));
  __m256 l_value_01_second_1 =
      _mm256_sub_ps(_mm256_mul_ps(GAIN_LARGE, l_value_01_first_1), mm256::copysign_ps(CONST_0_8, symbols_1));

  // Calculate threshold mask.
  __m256 THRESHOLD_ABS    = _mm256_set1_ps(2 * M_SQRT1_10);
  __m256 threshold_mask_0 = _mm256_cmp_ps(abs_symbols_0, THRESHOLD_ABS, _CMP_GT_OS);
  __m256 threshold_mask_1 = _mm256_cmp_ps(abs_symbols_1, THRESHOLD_ABS, _CMP_GT_OS);

  // Select l_value for bits 0 and 1.
  __m256 l_value_01_0 = _mm256_blendv_ps(l_value_01_first_0, l_value_01_second_0, threshold_mask_0);
  __m256 l_value_01_1 = _mm256_blendv_ps(l_value_01_first_1, l_value_01_second_1, threshold_mask_1);

  // Calculate l_value for bits 2 and 3.
  __m256 l_value_23_0 = _mm256_sub_ps(CONST_0_8, mm256::abs_ps(l_value_01_first_0));
  __m256 l_value_23_1 = _mm256_sub_ps(CONST_0_8, mm256::abs_ps(l_value_01_first_1));

  // Multiply by noise reciprocal.
  l_value_01_0 = _mm256_mul_ps(l_value_01_0, rcp_noise_0_);
  l_value_01_1 = _mm256_mul_ps(l_value_01_1, rcp_noise_1_);
  l_value_23_0 = _mm256_mul_ps(l_value_23_0, rcp_noise_0_);
  l_value_23_1 = _mm256_mul_ps(l_value_23_1, rcp_noise_1_);

  // Re-collocate values.
  __m256 l_value_0 = _mm256_shuffle_ps(l_value_01_0, l_value_23_0, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_1 = _mm256_shuffle_ps(l_value_01_0, l_value_23_0, _MM_SHUFFLE(3, 2, 3, 2));
  __m256 l_value_2 = _mm256_shuffle_ps(l_value_01_1, l_value_23_1, _MM_SHUFFLE(1, 0, 1, 0));
  __m256 l_value_3 = _mm256_shuffle_ps(l_value_01_1, l_value_23_1, _MM_SHUFFLE(3, 2, 3, 2));

  // Re-collocate SSE registers.
  __m256 l_value_0_ = _mm256_permute2f128_ps(l_value_0, l_value_1, 0x20);
  __m256 l_value_1_ = _mm256_permute2f128_ps(l_value_0, l_value_1, 0x31);
  __m256 l_value_2_ = _mm256_permute2f128_ps(l_value_2, l_value_3, 0x20);
  __m256 l_value_3_ = _mm256_permute2f128_ps(l_value_2, l_value_3, 0x31);

  // Store result.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr),
                      mm256::quantize_ps(l_value_0_, l_value_1_, l_value_2_, l_value_3_, range_limit));
}
#endif // HAVE_AVX2

inline log_likelihood_ratio demod_16QAM_symbol_01(float x, float noise_var, float range_limit)
{
  float l_value = 4 * M_SQRT1_10 * x;
  if (std::abs(x) > 2 * M_SQRT1_10) {
    l_value = 2 * l_value - std::copysign(0.8F, x);
  }

  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

inline log_likelihood_ratio demod_16QAM_symbol_23(float x, float noise_var, float range_limit)
{
  float l_value = 0.8F - 4 * M_SQRT1_10 * std::abs(x);
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

} // namespace

void srsgnb::demodulate_soft_QAM16(span<log_likelihood_ratio> llrs,
                                   span<const cf_t>           symbols,
                                   span<const float>          noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 300;

  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  std::size_t           symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 8 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 8) * 8; symbol_index != symbol_index_end; symbol_index += 8) {
    demod_QAM16_avx2(llr_it, symbols_it, noise_it, RANGE_LIMIT_FLOAT);

    llr_it += 32;
    symbols_it += 8;
    noise_it += 8;
  }
#endif // HAVE_AVX2

  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    *llr_it++ = demod_16QAM_symbol_01(std::real(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_01(std::imag(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_23(std::real(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_23(std::imag(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    ++symbols_it;
    ++noise_it;
  }
}