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

#include "demodulation_mapper_impl.h"
#include "demodulation_mapper_qam16.h"
#include "demodulation_mapper_qam256.h"
#include "demodulation_mapper_qam64.h"
#include "demodulation_mapper_qpsk.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

static log_likelihood_ratio demod_BPSK_symbol(cf_t z, float noise_var, float range_limit)
{
  // Note: "noise_var > 0" is false also when "noise_var" is NaN.
  if (!(noise_var > 0)) {
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
  srsran_assert(symbols.size() == noise_vars.size(), "Inputs symbols and noise_vars must have the same length.");
  srsran_assert(symbols.size() * get_bits_per_symbol(mod) == llrs.size(), "Input and output lengths are incompatible.");

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
      srsran_assertion_failure("Invalid modulation scheme.");
  }
}
