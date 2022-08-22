/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "demodulation_mapper_qam64.h"
#include "demodulation_mapper_intervals.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

using namespace srsgnb;

namespace {

// Square root of 1/42.
static const float M_SQRT1_42 = 1.0F / std::sqrt(42.0F);

// Maximum (absolute) value considered for quantization. Larger values will be clipped.
static constexpr float RANGE_LIMIT_FLOAT = 150;

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

inline void demod_QAM64_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var)
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
  __m256 rcp_noise_0 = _mm256_div_ps(_mm256_set1_ps(1), noise_0);
  __m256 rcp_noise_1 = _mm256_div_ps(_mm256_set1_ps(1), noise_1);

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

inline log_likelihood_ratio demod_64QAM_symbol_01(float value, float noise_var)
{
  float l_value = interval_function(value, noise_var, INTERVAL_WIDTH_01, NOF_INTERVALS_01, SLOPE_01, INTERCEPT_01);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

inline log_likelihood_ratio demod_64QAM_symbol_23(float value, float noise_var)
{
  float l_value = interval_function(value, noise_var, INTERVAL_WIDTH_23, NOF_INTERVALS_23, SLOPE_23, INTERCEPT_23);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

inline log_likelihood_ratio demod_64QAM_symbol_45(float value, float noise_var)
{
  float l_value = interval_function(value, noise_var, INTERVAL_WIDTH_45, NOF_INTERVALS_45, SLOPE_45, INTERCEPT_45);
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

} // namespace

void srsgnb::demodulate_soft_QAM64(span<log_likelihood_ratio> llrs,
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

  for (std::size_t symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    float rcp_noise = 1 / (*noise_it);
    *llr_it++       = demod_64QAM_symbol_01(std::real(*symbols_it), rcp_noise);
    *llr_it++       = demod_64QAM_symbol_01(std::imag(*symbols_it), rcp_noise);
    *llr_it++       = demod_64QAM_symbol_23(std::real(*symbols_it), rcp_noise);
    *llr_it++       = demod_64QAM_symbol_23(std::imag(*symbols_it), rcp_noise);
    *llr_it++       = demod_64QAM_symbol_45(std::real(*symbols_it), rcp_noise);
    *llr_it++       = demod_64QAM_symbol_45(std::imag(*symbols_it), rcp_noise);
    ++symbols_it;
    ++noise_it;
  }
}