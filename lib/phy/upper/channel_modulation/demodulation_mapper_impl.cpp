#include "demodulation_mapper_impl.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;

static void demodulate_soft_BPSK(span<int8_t> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float range_limit_float = 200;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float range_limit_int = 64;

  auto demod_sym = [](cf_t z, float n) {
    float l_value = 2.0F * M_SQRT2f32 * (std::real(z) + std::imag(z)) / n;
    l_value       = (std::abs(l_value) <= range_limit_float) ? l_value : std::copysign(range_limit_float, l_value);
    return static_cast<int8_t>(std::round(l_value * range_limit_int / range_limit_float));
  };

  std::transform(symbols.begin(), symbols.end(), noise_vars.begin(), llrs.begin(), demod_sym);
}

static void demodulate_soft_QPSK(span<int8_t> llrs, span<const cf_t> symbols, span<const float> noise_vars)
{
  // Maximum (absolute) value considered for quantization. Larger values will be clipped.
  constexpr float range_limit_float = 200;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float range_limit_int = 64;

  auto demod_sym = [](float z, float n) {
    float l_value = 2.0F * M_SQRT2f32 * z / n;
    l_value       = (std::abs(l_value) <= range_limit_float) ? l_value : std::copysign(range_limit_float, l_value);
    return static_cast<int8_t>(std::round(l_value * range_limit_int / range_limit_float));
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
  constexpr float range_limit_float = 100;
  // Equivalent of range_limit_float in fixed point representation.
  constexpr float range_limit_int = 64;

  const float M_SQRT1_10 = 1.0F / std::sqrt(10.0F);

  auto demod_sym_01 = [M_SQRT1_10](float z, float n) {
    float l_value = 4 * M_SQRT1_10 * z;
    l_value       = (std::abs(z) <= 2 * M_SQRT1_10) ? l_value : 2 * l_value - std::copysign(0.8F, z);
    l_value /= n;
    l_value = (std::abs(l_value) <= range_limit_float) ? l_value : std::copysign(range_limit_float, l_value);
    return static_cast<int8_t>(std::round(l_value * range_limit_int / range_limit_float));
  };

  auto demod_sym_23 = [M_SQRT1_10](float z, float n) {
    float l_value = 0.8F - 4 * M_SQRT1_10 * std::abs(z);
    l_value /= n;
    l_value = (std::abs(l_value) <= range_limit_float) ? l_value : std::copysign(range_limit_float, l_value);
    return static_cast<int8_t>(std::round(l_value * range_limit_int / range_limit_float));
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
  unsigned bits_per_symbol = static_cast<unsigned>(mod);
  srsran_assert(symbols.size() * bits_per_symbol == llrs.size(), "Input and output lengths are incompatible.");
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
