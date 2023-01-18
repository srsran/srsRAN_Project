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
#include "demodulation_mapper_qam16.h"
#include "demodulation_mapper_qam256.h"
#include "demodulation_mapper_qam64.h"
#include "demodulation_mapper_qpsk.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsgnb;

static log_likelihood_ratio demod_BPSK_symbol(cf_t z, float noise_var, float range_limit)
{
  if (!std::isnormal(noise_var)) {
    return 0;
  }
  float l_value = 2.0F * M_SQRT2f32 * (std::real(z) + std::imag(z)) / noise_var;
  return log_likelihood_ratio::quantize(l_value, range_limit);
};

static void
demodulate_soft_BPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 24;
  std::transform(symbols.begin(), symbols.end(), noise_vars.begin(), llrs.begin(), [](cf_t z, float n) {
    return demod_BPSK_symbol(z, n, RANGE_LIMIT_FLOAT);
  });
}

static void
demodulate_soft_PI_2_BPSK(span<log_likelihood_ratio> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 24;

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

void demodulation_mapper_impl::demodulate_soft(span<log_likelihood_ratio> llrs,
                                               span<const cf_t>           symbols,
                                               span<const float>          noise_vars,
                                               modulation_scheme          mod)
{
  srsgnb_assert(symbols.size() == noise_vars.size(), "Inputs symbols and noise_vars must have the same length.");
  srsgnb_assert(symbols.size() * get_bits_per_symbol(mod) == llrs.size(), "Input and output lengths are incompatible.");
  srsgnb_assert(std::all_of(noise_vars.begin(), noise_vars.end(), [](float f) { return (f >= 0); }),
                "Input noise_vars must have non-negative entries.");

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
      srsgnb_assertion_failure("Invalid modulation scheme.");
  }
}
