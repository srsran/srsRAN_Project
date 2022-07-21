/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "modulation_mapper_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/sc_prod.h"

#ifdef HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

using namespace srsgnb;

// Generic optimized modulator for any modulation order QM > 2 and QM % 2 == 0.
template <unsigned QM>
struct modulator_table_s {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  std::array<cf_t, pow2(QM)> table;

  // Constructs the Look-up table.
  constexpr modulator_table_s()
  {
    // see TS38.211 Section 5.1.
    for (uint32_t i = 0, i_end = pow2(QM); i != i_end; ++i) {
      float offset = -1;
      float real   = 0;
      float imag   = 0;
      for (uint32_t j = 0, j_end = QM / 2; j != j_end; ++j) {
        real += offset;
        imag += offset;
        offset *= 2;

        real *= ((i & (1U << (2U * j + 1U))) != 0) ? +1 : -1;
        imag *= ((i & (1U << (2U * j + 0U))) != 0) ? +1 : -1;
      }
      table[i] = {real, imag};
    }

    // Calculate average power to calculate scaling for having a power average of one.
    float avg_power = srsvec::average_power(table);
    srsgnb_assert(std::isnormal(avg_power), "Corrupted modulation average power.");

    // Perform scaling.
    float scaling = std::sqrt(1 / avg_power);
    srsvec::sc_prod(table, scaling, table);
  }

  // Modulates the input bits.
  constexpr void modulate(span<const uint8_t>& input, span<cf_t> symbols) const
  {
    for (cf_t& symbol : symbols) {
#ifdef HAVE_SSE
      // Get 8 Bit in an MMX register.
      __m64 mask = _mm_cmpgt_pi8(*((__m64*)input.data()), _mm_set1_pi8(0));

      // Get mask and write
      unsigned index = _mm_movemask_pi8(mask);

      // Reverse bits.
      index = reverse_byte(index);

      // Advance input pointer
      input = input.last(input.size() - QM);

      // Mask the bits of interest.
      index = index >> (8 - QM);
#else
      // Packs the next QM bits.
      unsigned index = 0;
      for (unsigned i = 0; i != QM; ++i) {
        index |= (unsigned)input[i] << (QM - i - 1U);
      }

      // Advance pointer
      input = input.last(input.size() - QM);
#endif // HAVE_SSE

      // Assign symbol from table
      symbol = table[index];
    }
    srsgnb_assert(input.empty(), "Expected full consumption of the input data.");
  }
};

// Generic optimized modulator for BSPK.
struct modulator_table_bpsk {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  const std::array<cf_t, 2> table = {{{M_SQRT1_2, M_SQRT1_2}, {-M_SQRT1_2, -M_SQRT1_2}}};

  // Modulates the input bits.
  void modulate(span<const uint8_t>& input, span<cf_t> symbols) const
  {
    std::transform(input.begin(), input.end(), symbols.begin(), [this](uint8_t bit) { return table[bit]; });
  }
};

// Generic optimized modulator for π/2-BPSK.
struct modulator_table_pi_2_bpsk {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  const std::array<cf_t, 2> table_even = {{{M_SQRT1_2, M_SQRT1_2}, {-M_SQRT1_2, -M_SQRT1_2}}};
  const std::array<cf_t, 2> table_odd  = {{{-M_SQRT1_2, M_SQRT1_2}, {M_SQRT1_2, -M_SQRT1_2}}};

  // Modulates the input bits.
  void modulate(span<const uint8_t>& input, span<cf_t> symbols) const
  {
    std::transform(input.begin(), input.end(), symbols.begin(), [this, n = 0](uint8_t bit) mutable {
      return (n++ & 1U) ? table_odd[bit] : table_even[bit];
    });
  }
};

// Modulation tables.
static const modulator_table_pi_2_bpsk pi_2_bpsk_modulator;
static const modulator_table_bpsk      bpsk_modulator;
static const modulator_table_s<2>      qpsk_modulator;
static const modulator_table_s<4>      qam16_modulator;
static const modulator_table_s<6>      qam64_modulator;
static const modulator_table_s<8>      qam256_modulator;

void modulation_mapper_impl::modulate(srsgnb::span<const uint8_t> input,
                                      srsgnb::span<srsgnb::cf_t>  symbols,
                                      srsgnb::modulation_scheme   scheme)
{
  srsgnb_assert(input.size() == get_bits_per_symbol(scheme) * symbols.size(),
                "The number of bits {} is not consistent with the number of symbols {} for modulation scheme {}.",
                input.size(),
                symbols.size(),
                scheme);

  switch (scheme) {
    case modulation_scheme::PI_2_BPSK:
      pi_2_bpsk_modulator.modulate(input, symbols);
      break;
    case modulation_scheme::BPSK:
      bpsk_modulator.modulate(input, symbols);
      break;
    case modulation_scheme::QPSK:
      qpsk_modulator.modulate(input, symbols);
      break;
    case modulation_scheme::QAM16:
      qam16_modulator.modulate(input, symbols);
      break;
    case modulation_scheme::QAM64:
      qam64_modulator.modulate(input, symbols);
      break;
    case modulation_scheme::QAM256:
      qam256_modulator.modulate(input, symbols);
      break;
  }
}
