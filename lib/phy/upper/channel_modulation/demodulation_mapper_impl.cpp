/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "demodulation_mapper_impl.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsgnb_assert.h"

#ifdef HAVE_AVX2
#include "avx2_helpers.h"
#endif // HAVE_AVX2

using namespace srsgnb;

// Square root of 1/10.
static const float M_SQRT1_10 = 1.0F / std::sqrt(10.0F);

// Square root of 1/42.
static const float M_SQRT1_42 = 1.0F / std::sqrt(42.0F);

// Square root of 1/170.
static const float M_SQRT1_170 = 1.0F / std::sqrt(170.0F);

static log_likelihood_ratio demod_BPSK_symbol(cf_t z, float noise_var, float range_limit)
{
  float l_value = 2.0F * M_SQRT2f32 * (std::real(z) + std::imag(z)) / noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
};

static void
demodulate_soft_BPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 600;
  std::transform(symbols.begin(), symbols.end(), noise_vars.begin(), llrs.begin(), [](cf_t z, float n) {
    return demod_BPSK_symbol(z, n, RANGE_LIMIT_FLOAT);
  });
}

static void
demodulate_soft_PI_2_BPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 600;

  // Even-indexed symbols.
  auto*       out = llrs.begin();
  const auto* in  = symbols.begin();
  const auto* n   = noise_vars.begin();
  for (; out < llrs.end(); out += 2, in += 2, n += 2) {
    *out = demod_BPSK_symbol(*in, *n, RANGE_LIMIT_FLOAT);
  }

  // Odd-indexed symbols.
  out = llrs.begin() + 1;
  in  = symbols.begin() + 1;
  n   = noise_vars.begin() + 1;
  for (; out < llrs.end(); out += 2, in += 2, n += 2) {
    *out = demod_BPSK_symbol(cf_t(std::imag(*in), -std::real(*in)), *n, RANGE_LIMIT_FLOAT);
  }
}

static log_likelihood_ratio demod_QPSK_symbol(float x, float noise_var, float range_limit)
{
  float l_value = 2.0F * M_SQRT2f32 * x / noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

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
                      avx2::quantize(l_value_0, l_value_1, l_value_2, l_value_3, range_limit));
}

inline void demod_QAM16_avx2(log_likelihood_ratio* llr, const cf_t* symbol, const float* noise_var, float range_limit)
{
  // Load symbols.
  __m256 symbols_0 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 0));
  __m256 symbols_1 = _mm256_loadu_ps(reinterpret_cast<const float*>(symbol + 4));

  __m256 abs_symbols_0 = avx2::abs(symbols_0);
  __m256 abs_symbols_1 = avx2::abs(symbols_1);

  // Load noise.
  __m256 noise_0 = _mm256_loadu_ps(noise_var + 0);

  // Repeat noise values for real and imaginary parts.
  __m256 noise_1 = _mm256_unpackhi_ps(noise_0, noise_0);
  noise_0        = _mm256_unpacklo_ps(noise_0, noise_0);

  // Re-collocate SSE registers.
  __m256 noise_0_ = _mm256_permute2f128_ps(noise_0, noise_1, 0x20);
  __m256 noise_1_ = _mm256_permute2f128_ps(noise_0, noise_1, 0x31);

  // Calculate l_value for bits 0 and 1.
  __m256 GAIN_FIRST         = _mm256_set1_ps(4.0F * M_SQRT1_10);
  __m256 GAIN_LARGE         = _mm256_set1_ps(2.0F);
  __m256 CONST_0_8          = _mm256_set1_ps(0.8F);
  __m256 l_value_01_first_0 = _mm256_mul_ps(GAIN_FIRST, symbols_0);
  __m256 l_value_01_first_1 = _mm256_mul_ps(GAIN_FIRST, symbols_1);
  __m256 l_value_01_second_0 =
      _mm256_sub_ps(_mm256_mul_ps(GAIN_LARGE, l_value_01_first_0), avx2::copysign(CONST_0_8, symbols_0));
  __m256 l_value_01_second_1 =
      _mm256_sub_ps(_mm256_mul_ps(GAIN_LARGE, l_value_01_first_1), avx2::copysign(CONST_0_8, symbols_1));

  // Calculate threshold mask.
  __m256 THRESHOLD_ABS    = _mm256_set1_ps(2 * M_SQRT1_10);
  __m256 threshold_mask_0 = _mm256_cmp_ps(abs_symbols_0, THRESHOLD_ABS, _CMP_GT_OS);
  __m256 threshold_mask_1 = _mm256_cmp_ps(abs_symbols_1, THRESHOLD_ABS, _CMP_GT_OS);

  // Select l_value for bits 0 and 1.
  __m256 l_value_01_0 = _mm256_blendv_ps(l_value_01_first_0, l_value_01_second_0, threshold_mask_0);
  __m256 l_value_01_1 = _mm256_blendv_ps(l_value_01_first_1, l_value_01_second_1, threshold_mask_1);

  // Calculate l_value for bits 2 and 3.
  __m256 l_value_23_0 = _mm256_sub_ps(CONST_0_8, avx2::abs(l_value_01_first_0));
  __m256 l_value_23_1 = _mm256_sub_ps(CONST_0_8, avx2::abs(l_value_01_first_1));

  // Divide by noise.
  l_value_01_0 = _mm256_div_ps(l_value_01_0, noise_0_);
  l_value_01_1 = _mm256_div_ps(l_value_01_1, noise_1_);
  l_value_23_0 = _mm256_div_ps(l_value_23_0, noise_0_);
  l_value_23_1 = _mm256_div_ps(l_value_23_1, noise_1_);

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
                      avx2::quantize(l_value_0_, l_value_1_, l_value_2_, l_value_3_, range_limit));
}
#endif // HAVE_AVX2

static void
demodulate_soft_QPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 600;

  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  unsigned              symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 16 symbols.
  for (unsigned symbol_index_end = (symbols.size() / 16) * 16; symbol_index != symbol_index_end; symbol_index += 16) {
    demod_QPSK_avx2(llr_it, symbols_it, noise_it, RANGE_LIMIT_FLOAT);

    llr_it += 32;
    symbols_it += 16;
    noise_it += 16;
  }
#endif // HAVE_AVX2

  // Process remainder symbols with the generic algorithm.
  for (unsigned symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    cf_t this_symbol = *symbols_it++;
    *llr_it++        = demod_QPSK_symbol(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++        = demod_QPSK_symbol(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    ++noise_it;
  }
}

static log_likelihood_ratio demod_16QAM_symbol_01(float x, float noise_var, float range_limit)
{
  float l_value = 4 * M_SQRT1_10 * x;
  if (std::abs(x) > 2 * M_SQRT1_10) {
    l_value = 2 * l_value - std::copysign(0.8F, x);
  }

  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
};

static log_likelihood_ratio demod_16QAM_symbol_23(float x, float noise_var, float range_limit)
{
  float l_value = 0.8F - 4 * M_SQRT1_10 * std::abs(x);
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
};

static void
demodulate_soft_QAM16(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 300;

  const cf_t*           symbols_it   = symbols.begin();
  const float*          noise_it     = noise_vars.begin();
  log_likelihood_ratio* llr_it       = llrs.begin();
  unsigned              symbol_index = 0;

#ifdef HAVE_AVX2
  // For AVX2, it generates 32 LLRs simultaneously. The input is read in batches of 8 symbols.
  for (unsigned symbol_index_end = (symbols.size() / 8) * 8; symbol_index != symbol_index_end; symbol_index += 8) {
    demod_QAM16_avx2(llr_it, symbols_it, noise_it, RANGE_LIMIT_FLOAT);

    llr_it += 32;
    symbols_it += 8;
    noise_it += 8;
  }
#endif // HAVE_AVX2

  for (unsigned symbol_index_end = symbols.size(); symbol_index != symbol_index_end; ++symbol_index) {
    *llr_it++ = demod_16QAM_symbol_01(std::real(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_01(std::imag(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_23(std::real(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_16QAM_symbol_23(std::imag(*symbols_it), *noise_it, RANGE_LIMIT_FLOAT);
    ++symbols_it;
    ++noise_it;
  }
}

unsigned compute_interval_idx(float x, float interval_width, unsigned nof_intervals)
{
  int nof_intervals_int = static_cast<int>(nof_intervals);
  int idx               = static_cast<int>(std::floor(x / interval_width)) + nof_intervals_int / 2;

  return clamp(idx, 0, nof_intervals_int - 1);
}

static log_likelihood_ratio demod_64QAM_symbol_01(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 8;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {16 * M_SQRT1_42,
                                                                 12 * M_SQRT1_42,
                                                                 8 * M_SQRT1_42,
                                                                 4 * M_SQRT1_42,
                                                                 4 * M_SQRT1_42,
                                                                 8 * M_SQRT1_42,
                                                                 12 * M_SQRT1_42,
                                                                 16 * M_SQRT1_42};
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {
          24.0F / 21, 12.0F / 21, 4.0F / 21, 0.0F, 0.0F, -4.0F / 21, -12.0F / 21, -24.0F / 21};

  unsigned idx     = compute_interval_idx(x, 2 * M_SQRT1_42, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static log_likelihood_ratio demod_64QAM_symbol_23(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 8;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {8 * M_SQRT1_42,
                                                                 4 * M_SQRT1_42,
                                                                 4 * M_SQRT1_42,
                                                                 8 * M_SQRT1_42,
                                                                 -8 * M_SQRT1_42,
                                                                 -4 * M_SQRT1_42,
                                                                 -4 * M_SQRT1_42,
                                                                 -8 * M_SQRT1_42};
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {
          20.0F / 21, 8.0F / 21, 8.0F / 21, 12.0F / 21, 12.0F / 21, 8.0F / 21, 8.0F / 21, 20.0F / 21};

  unsigned idx     = compute_interval_idx(x, 2 * M_SQRT1_42, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static log_likelihood_ratio demod_64QAM_symbol_45(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 4;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {
              4 * M_SQRT1_42, -4 * M_SQRT1_42, 4 * M_SQRT1_42, -4 * M_SQRT1_42};
  static const std::array<float, NOF_INTERVALS> INTERCEPT = {12.0F / 21, -4.0F / 21, -4.0F / 21, 12.0F / 21};

  unsigned idx     = compute_interval_idx(x, 4 * M_SQRT1_42, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static void
demodulate_soft_QAM64(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 150;

  auto*       llr_it   = llrs.begin();
  const auto* noise_it = noise_vars.begin();
  for (const auto this_symbol : symbols) {
    *llr_it++ = demod_64QAM_symbol_01(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_64QAM_symbol_01(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_64QAM_symbol_23(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_64QAM_symbol_23(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_64QAM_symbol_45(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_64QAM_symbol_45(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    ++noise_it;
  }
}

static log_likelihood_ratio demod_256QAM_symbol_01(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 16;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {32 * M_SQRT1_170,
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
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {112.0F / 85,
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

  unsigned idx     = compute_interval_idx(x, 2 * M_SQRT1_170, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static log_likelihood_ratio demod_256QAM_symbol_23(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 16;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {16 * M_SQRT1_170,
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
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {88.0F / 85,
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

  unsigned idx     = compute_interval_idx(x, 2 * M_SQRT1_170, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static log_likelihood_ratio demod_256QAM_symbol_45(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 16;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {8 * M_SQRT1_170,
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
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {52.0F / 85,
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

  unsigned idx     = compute_interval_idx(x, 2 * M_SQRT1_170, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static log_likelihood_ratio demod_256QAM_symbol_67(float x, float noise_var, float range_limit)
{
  constexpr unsigned                            NOF_INTERVALS = 8;
  static const std::array<float, NOF_INTERVALS> SLOPE         = {4 * M_SQRT1_170,
                                                                 -4 * M_SQRT1_170,
                                                                 4 * M_SQRT1_170,
                                                                 -4 * M_SQRT1_170,
                                                                 4 * M_SQRT1_170,
                                                                 -4 * M_SQRT1_170,
                                                                 4 * M_SQRT1_170,
                                                                 -4 * M_SQRT1_170};
  static const std::array<float, NOF_INTERVALS> INTERCEPT     = {
          28.0F / 85, -20.0F / 85, 12.0F / 85, -4.0F / 85, -4.0F / 85, 12.0F / 85, -20.0F / 85, 28.0F / 85};

  unsigned idx     = compute_interval_idx(x, 4 * M_SQRT1_170, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
}

static void
demodulate_soft_QAM256(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 90;

  auto*       llr_it   = llrs.begin();
  const auto* noise_it = noise_vars.begin();
  for (const auto this_symbol : symbols) {
    *llr_it++ = demod_256QAM_symbol_01(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_01(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_23(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_23(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_45(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_45(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_67(std::real(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    *llr_it++ = demod_256QAM_symbol_67(std::imag(this_symbol), *noise_it, RANGE_LIMIT_FLOAT);
    ++noise_it;
  }
}

void demodulation_mapper_impl::demodulate_soft(span<log_likelihood_ratio> llrs,
                                               span<const cf_t>           symbols,
                                               span<const float>          noise_vars,
                                               modulation_scheme          mod)
{
  srsgnb_assert(symbols.size() == noise_vars.size(), "Inputs symbols and noise_vars must have the same length.");
  srsgnb_assert(symbols.size() * get_bits_per_symbol(mod) == llrs.size(), "Input and output lengths are incompatible.");
  srsgnb_assert(std::all_of(noise_vars.begin(), noise_vars.end(), [](float f) { return f > 0; }),
                "Input noise_vars must have positive entries.");

  switch (mod) {
    case modulation_scheme::BPSK:
      demodulate_soft_BPSK(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::PI_2_BPSK:
      demodulate_soft_PI_2_BPSK(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QPSK:
      demodulate_soft_QPSK(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QAM16:
      demodulate_soft_QAM16(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QAM64:
      demodulate_soft_QAM64(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QAM256:
      demodulate_soft_QAM256(llrs, symbols, noise_vars);
      break;
    default:
      assert(false);
  }
}
