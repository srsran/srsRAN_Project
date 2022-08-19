/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "demodulation_mapper_qam256.h"
#include "demodulation_mapper_intervals.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

using namespace srsgnb;

namespace {
// Square root of 1/170.
const float M_SQRT1_170 = 1.0F / std::sqrt(170.0F);

// Maximum (absolute) value considered for quantization. Larger values will be clipped.
constexpr float RANGE_LIMIT_FLOAT = 90;

inline log_likelihood_ratio demod_256QAM_symbol_01(float x, float noise_var)
{
  constexpr unsigned                     NOF_INTERVALS = 16;
  const std::array<float, NOF_INTERVALS> SLOPE         = {32 * M_SQRT1_170,
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
  const std::array<float, NOF_INTERVALS> INTERCEPT     = {112.0F / 85,
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
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

inline log_likelihood_ratio demod_256QAM_symbol_23(float x, float noise_var)
{
  constexpr unsigned                     NOF_INTERVALS = 16;
  const std::array<float, NOF_INTERVALS> SLOPE         = {16 * M_SQRT1_170,
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
  const std::array<float, NOF_INTERVALS> INTERCEPT     = {88.0F / 85,
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
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

inline log_likelihood_ratio demod_256QAM_symbol_45(float x, float noise_var)
{
  constexpr unsigned                     NOF_INTERVALS = 16;
  const std::array<float, NOF_INTERVALS> SLOPE         = {8 * M_SQRT1_170,
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
  const std::array<float, NOF_INTERVALS> INTERCEPT     = {52.0F / 85,
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
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

inline log_likelihood_ratio demod_256QAM_symbol_67(float x, float noise_var)
{
  constexpr unsigned                     NOF_INTERVALS = 8;
  const std::array<float, NOF_INTERVALS> SLOPE         = {4 * M_SQRT1_170,
                                                          -4 * M_SQRT1_170,
                                                          4 * M_SQRT1_170,
                                                          -4 * M_SQRT1_170,
                                                          4 * M_SQRT1_170,
                                                          -4 * M_SQRT1_170,
                                                          4 * M_SQRT1_170,
                                                          -4 * M_SQRT1_170};
  const std::array<float, NOF_INTERVALS> INTERCEPT     = {
          28.0F / 85, -20.0F / 85, 12.0F / 85, -4.0F / 85, -4.0F / 85, 12.0F / 85, -20.0F / 85, 28.0F / 85};

  unsigned idx     = compute_interval_idx(x, 4 * M_SQRT1_170, NOF_INTERVALS);
  float    l_value = SLOPE[idx] * x + INTERCEPT[idx];
  l_value /= noise_var;
  return log_likelihood_ratio::quantize(l_value, RANGE_LIMIT_FLOAT);
}

} // namespace

void srsgnb::demodulate_soft_QAM256(span<log_likelihood_ratio> llrs,
                                    span<const cf_t>           symbols,
                                    span<const float>          noise_vars)
{
  auto*       llr_it   = llrs.begin();
  const auto* noise_it = noise_vars.begin();
  for (const auto this_symbol : symbols) {
    *llr_it++ = demod_256QAM_symbol_01(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_01(std::imag(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_23(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_23(std::imag(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_45(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_45(std::imag(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_67(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_256QAM_symbol_67(std::imag(this_symbol), *noise_it);
    ++noise_it;
  }
}
