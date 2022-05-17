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
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;

static void demodulate_soft_BPSK(span<int8_t> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 200;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float RANGE_LIMIT_INT = 64;

  auto demod_sym = [](cf_t z, float n) {
    float l_value = 2.0F * M_SQRT2f32 * (std::real(z) + std::imag(z)) / n;
    return clip_and_quantize(l_value, RANGE_LIMIT_FLOAT, RANGE_LIMIT_INT);
  };

  std::transform(symbols.begin(), symbols.end(), noise_vars.begin(), llrs.begin(), demod_sym);
}

static void demodulate_soft_QPSK(span<int8_t> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 200;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float RANGE_LIMIT_INT = 64;

  auto demod_sym = [](float z, float n) {
    float l_value = 2.0F * M_SQRT2f32 * z / n;
    return clip_and_quantize(l_value, RANGE_LIMIT_FLOAT, RANGE_LIMIT_INT);
  };

  auto*       llr_it   = llrs.begin();
  const auto* noise_it = noise_vars.begin();
  for (const auto this_symbol : symbols) {
    *llr_it++ = demod_sym(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_sym(std::imag(this_symbol), *noise_it);
    ++noise_it;
  }
}

static void demodulate_soft_QAM16(span<int8_t> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float RANGE_LIMIT_FLOAT = 100;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float RANGE_LIMIT_INT = 64;

  const float M_SQRT1_10 = 1.0F / std::sqrt(10.0F);

  auto demod_sym_01 = [M_SQRT1_10](float z, float n) {
    float l_value = 4 * M_SQRT1_10 * z;
    l_value       = (std::abs(z) <= 2 * M_SQRT1_10) ? l_value : 2 * l_value - std::copysign(0.8F, z);
    l_value /= n;
    return clip_and_quantize(l_value, RANGE_LIMIT_FLOAT, RANGE_LIMIT_INT);
  };

  auto demod_sym_23 = [M_SQRT1_10](float z, float n) {
    float l_value = 0.8F - 4 * M_SQRT1_10 * std::abs(z);
    l_value /= n;
    return clip_and_quantize(l_value, RANGE_LIMIT_FLOAT, RANGE_LIMIT_INT);
  };

  auto*       llr_it   = llrs.begin();
  const auto* noise_it = noise_vars.begin();
  for (const auto this_symbol : symbols) {
    *llr_it++ = demod_sym_01(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_sym_01(std::imag(this_symbol), *noise_it);
    *llr_it++ = demod_sym_23(std::real(this_symbol), *noise_it);
    *llr_it++ = demod_sym_23(std::imag(this_symbol), *noise_it);
    ++noise_it;
  }
}

void demodulation_mapper_impl::demodulate_soft(span<int8_t>      llrs,
                                               span<const cf_t>  symbols,
                                               span<const float> noise_vars,
                                               modulation_scheme mod)
{
  srsran_assert(symbols.size() == noise_vars.size(), "Inputs symbols and noise_vars must have the same length.");
  srsran_assert(symbols.size() * get_bits_per_symbol(mod) == llrs.size(), "Input and output lengths are incompatible.");
  srsran_assert(std::all_of(noise_vars.begin(), noise_vars.end(), [](float f) { return f > 0; }),
                "Input noise_vars must have positive entries.");

  switch (mod) {
    case modulation_scheme::BPSK:
      demodulate_soft_BPSK(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QPSK:
      demodulate_soft_QPSK(llrs, symbols, noise_vars);
      break;
    case modulation_scheme::QAM16:
      demodulate_soft_QAM16(llrs, symbols, noise_vars);
      break;
    default:
      // TODO(david): finish QAM64 and QAM256. Also, double-check quantization.
      assert(false);
  }
}

std::unique_ptr<demodulation_mapper> srsgnb::create_demodulation_mapper()
{
  return std::make_unique<demodulation_mapper_impl>();
}
