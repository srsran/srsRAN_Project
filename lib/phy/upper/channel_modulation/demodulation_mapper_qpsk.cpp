/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "demodulation_mapper_qpsk.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

using namespace srsgnb;

namespace {

#ifdef HAVE_AVX2

inline void demod_QPSK_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var, float range_limit)
{
  // Load symbols.
  __m256 symbols_0 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 0));
  __m256 symbols_1 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 4));
  __m256 symbols_2 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 8));
  __m256 symbols_3 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 12));

  // Load noise.
  __m256 noise_0 = _mm256_loadu_ps(noise_var + 0);
  __m256 noise_1 = _mm256_loadu_ps(noise_var + 8);

  // Repeat noise values for real and imaginary parts.
  __m256 noise_3 = _mm256_unpackhi_ps(noise_1, noise_1);
  __m256 noise_2 = _mm256_unpacklo_ps(noise_1, noise_1);
  noise_1        = _mm256_unpackhi_ps(noise_0, noise_0);
  noise_0        = _mm256_unpacklo_ps(noise_0, noise_0);

  // Re-collocate SSE registers.
  __m256 noise_0_ = _mm256_permute2f128_ps(noise_0, noise_1, 0x20);
  __m256 noise_1_ = _mm256_permute2f128_ps(noise_0, noise_1, 0x31);
  __m256 noise_2_ = _mm256_permute2f128_ps(noise_2, noise_3, 0x20);
  __m256 noise_3_ = _mm256_permute2f128_ps(noise_2, noise_3, 0x31);

  // Calculate l_value.
  __m256 GAIN      = _mm256_set1_ps(2.0F * M_SQRT2f32);
  __m256 l_value_0 = _mm256_div_ps(_mm256_mul_ps(GAIN, symbols_0), noise_0_);
  __m256 l_value_1 = _mm256_div_ps(_mm256_mul_ps(GAIN, symbols_1), noise_1_);
  __m256 l_value_2 = _mm256_div_ps(_mm256_mul_ps(GAIN, symbols_2), noise_2_);
  __m256 l_value_3 = _mm256_div_ps(_mm256_mul_ps(GAIN, symbols_3), noise_3_);

  // Store result.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(llr),
                      mm256::quantize_ps(l_value_0, l_value_1, l_value_2, l_value_3, range_limit));
}

#endif // HAVE_AVX2

inline log_likelihood_ratio demod_QPSK_symbol(float x, float noise_var, float range_limit)
{
  float l_value = 2.0F * M_SQRT2f32 * x / noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

} // namespace

void srsgnb::demodulate_soft_QPSK(span<log_likelihood_ratio> llrs,
                                  span<const cf_t>           symbols,
                                  span<const float>          noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 600;

  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  std::size_t           symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 16 symbols.
  for (std::size_t symbol_index_end = (symbols.size() / 16) * 16; symbol_index != symbol_index_end;
       symbol_index += 16) {
    demod_QPSK_avx2(llr_it, symbols_it, noise_it, RANGE_LIMIT_FLOAT);

    llr_it += 32;
    symbols_it += 16;
    noise_it += 16;
  }
#endif // HAVE_AVX2

  // Process remainder symbols with the generic algorithm.
  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    cf_t this_symbol = *symbols_it++;
    *llr_it++        = demod_QPSK_symbol(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++        = demod_QPSK_symbol(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    ++noise_it;
  }
}