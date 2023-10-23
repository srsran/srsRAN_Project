/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "modulation_mapper_impl.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

// Generic optimized modulator for any modulation order QM > 2 and QM % 2 == 0.
template <unsigned QM>
struct modulator_table_s {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  std::array<cf_t, pow2(QM)>  cf_table;
  std::array<ci8_t, pow2(QM)> ci8_table;
  float                       scaling;

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
      cf_table[i]  = {real, imag};
      ci8_table[i] = to_ci8(cf_table[i]);
    }

    // Calculate average power to calculate scaling for having a power average of one.
    float avg_power = srsvec::average_power(cf_table);
    srsran_assert(std::isnormal(avg_power), "Corrupted modulation average power.");

    // Perform scaling.
    scaling = std::sqrt(1 / avg_power);
    srsvec::sc_prod(cf_table, scaling, cf_table);
  }

  // Modulates the input bits.
  template <typename Type>
  inline constexpr void
  modulate(span<Type> symbols, const bit_buffer& input, const std::array<Type, pow2(QM)>& table) const
  {
    unsigned i_symbol = 0;

    // Optimized version for QPSK.
    if (QM == 2) {
      for (unsigned i_byte = 0, i_symbol_end = (symbols.size() / 4) * 4; i_symbol != i_symbol_end;) {
        uint8_t byte        = input.get_byte(i_byte++);
        uint8_t index0      = (byte >> 6U) & 0b11;
        uint8_t index1      = (byte >> 4U) & 0b11;
        uint8_t index2      = (byte >> 2U) & 0b11;
        uint8_t index3      = byte & 0b11;
        symbols[i_symbol++] = table[index0];
        symbols[i_symbol++] = table[index1];
        symbols[i_symbol++] = table[index2];
        symbols[i_symbol++] = table[index3];
      }
    }

    // Optimized version for 16-QAM.
    if (QM == 4) {
      for (unsigned i_byte = 0, i_symbol_end = (symbols.size() / 2) * 2; i_symbol != i_symbol_end;) {
        uint8_t byte        = input.get_byte(i_byte++);
        uint8_t index0      = byte >> 4U;
        uint8_t index1      = byte & 0b1111;
        symbols[i_symbol++] = table[index0];
        symbols[i_symbol++] = table[index1];
      }
    }

    // Optimized version for 64-QAM.
    if (QM == 6) {
      for (unsigned i_byte = 0, i_symbol_end = (symbols.size() / 4) * 4; i_symbol != i_symbol_end;) {
        uint8_t byte0 = input.get_byte(i_byte++);
        uint8_t byte1 = input.get_byte(i_byte++);
        uint8_t byte2 = input.get_byte(i_byte++);

        uint8_t index0 = byte0 >> 2U;
        uint8_t index1 = ((byte0 & 0b11U) << 4U) | (byte1 >> 4U);
        uint8_t index2 = ((byte1 & 0b1111U) << 2U) | (byte2 >> 6U);
        uint8_t index3 = byte2 & 0b111111U;

        // Get the symbol from the table.
        symbols[i_symbol++] = table[index0];
        symbols[i_symbol++] = table[index1];
        symbols[i_symbol++] = table[index2];
        symbols[i_symbol++] = table[index3];
      }
    }

    // Optimized version for 256-QAM.
    if (QM == 8) {
      for (unsigned i_byte = 0, i_symbol_end = symbols.size(); i_symbol != i_symbol_end;) {
        uint8_t byte        = input.get_byte(i_byte++);
        symbols[i_symbol++] = table[byte];
      }
    }

    // Generic implementation.
    for (unsigned i_symbol_end = symbols.size(); i_symbol != i_symbol_end; ++i_symbol) {
      // Calculate modulation table index.
      unsigned index = input.extract(QM * i_symbol, QM);

      // Get the symbol from the table.
      symbols[i_symbol] = table[index];
    }
  }

  // Modulates the input bits.
  constexpr void modulate(span<cf_t> symbols, const bit_buffer& input) const { modulate(symbols, input, cf_table); }

  // Modulates the input bits.
  constexpr float modulate(span<ci8_t> symbols, const bit_buffer& input) const
  {
    modulate(symbols, input, ci8_table);
    return scaling;
  }
};

// Generic optimized modulator for BSPK.
struct modulator_table_bpsk {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  const std::array<cf_t, 2>  cf_table  = {{{M_SQRT1_2, M_SQRT1_2}, {-M_SQRT1_2, -M_SQRT1_2}}};
  const std::array<ci8_t, 2> ci8_table = {{{1, 1}, {-1, -1}}};

  // Modulates the input bits.
  void modulate(span<cf_t> symbols, const bit_buffer& input) const
  {
    for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
      unsigned index = input.extract<unsigned>(i_bit, 1);
      symbols[i_bit] = cf_table[index];
    }
  }

  // Modulates the input bits.
  float modulate(span<ci8_t> symbols, const bit_buffer& input) const
  {
    for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
      unsigned index = input.extract<unsigned>(i_bit, 1);
      symbols[i_bit] = ci8_table[index];
    }
    return M_SQRT1_2;
  }
};

// Generic optimized modulator for π/2-BPSK.
struct modulator_table_pi_2_bpsk {
  // The indexing provides the complex symbol corresponding to the binary expansion of the index.
  const std::array<cf_t, 2>  cf_table_even  = {{{M_SQRT1_2, M_SQRT1_2}, {-M_SQRT1_2, -M_SQRT1_2}}};
  const std::array<cf_t, 2>  cf_table_odd   = {{{-M_SQRT1_2, M_SQRT1_2}, {M_SQRT1_2, -M_SQRT1_2}}};
  const std::array<ci8_t, 2> ci8_table_even = {{{1, 1}, {-1, -1}}};
  const std::array<ci8_t, 2> ci8_table_odd  = {{{-1, 1}, {1, -1}}};

  // Modulates the input bits.
  void modulate(span<cf_t> symbols, const bit_buffer& input) const
  {
    for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
      uint8_t index  = input.extract(i_bit, 1);
      symbols[i_bit] = (i_bit & 1U) ? cf_table_odd[index] : cf_table_even[index];
    }
  }

  // Modulates the input bits.
  float modulate(span<ci8_t> symbols, const bit_buffer& input) const
  {
    for (unsigned i_bit = 0, i_bit_end = input.size(); i_bit != i_bit_end; ++i_bit) {
      uint8_t index  = input.extract(i_bit, 1);
      symbols[i_bit] = (i_bit & 1U) ? ci8_table_odd[index] : ci8_table_even[index];
    }
    return M_SQRT1_2;
  }
};

// Modulation tables.
static const modulator_table_pi_2_bpsk pi_2_bpsk_modulator;
static const modulator_table_bpsk      bpsk_modulator;
static const modulator_table_s<2>      qpsk_modulator;
static const modulator_table_s<4>      qam16_modulator;
static const modulator_table_s<6>      qam64_modulator;
static const modulator_table_s<8>      qam256_modulator;

void modulation_mapper_impl::modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  srsran_assert(input.size() == get_bits_per_symbol(scheme) * symbols.size(),
                "The number of bits {} is not consistent with the number of symbols {} for modulation scheme {}.",
                input.size(),
                symbols.size(),
                scheme);

  switch (scheme) {
    case modulation_scheme::PI_2_BPSK:
      pi_2_bpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::BPSK:
      bpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QPSK:
      qpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM16:
      qam16_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM64:
      qam64_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM256:
      qam256_modulator.modulate(symbols, input);
      break;
    default:
      srsran_assertion_failure("Invalid modulation scheme.");
  }
}

float srsran::modulation_mapper::get_modulation_scaling(modulation_scheme modulation)
{
  switch (modulation) {
    case modulation_scheme::PI_2_BPSK:
      return M_SQRT1_2;
    case modulation_scheme::BPSK:
      return M_SQRT1_2;
    case modulation_scheme::QPSK:
      return qpsk_modulator.scaling;
    case modulation_scheme::QAM16:
      return qam16_modulator.scaling;
    case modulation_scheme::QAM64:
      return qam64_modulator.scaling;
    case modulation_scheme::QAM256:
    default:
      return qam256_modulator.scaling;
  }
}

float modulation_mapper_impl::modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  srsran_assert(input.size() == get_bits_per_symbol(scheme) * symbols.size(),
                "The number of bits {} is not consistent with the number of symbols {} for modulation scheme {}.",
                input.size(),
                symbols.size(),
                scheme);

  float scaling = 0.0;

  switch (scheme) {
    case modulation_scheme::PI_2_BPSK:
      scaling = pi_2_bpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::BPSK:
      scaling = bpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QPSK:
      scaling = qpsk_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM16:
      scaling = qam16_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM64:
      scaling = qam64_modulator.modulate(symbols, input);
      break;
    case modulation_scheme::QAM256:
      scaling = qam256_modulator.modulate(symbols, input);
      break;
    default:
      srsran_assertion_failure("Invalid modulation scheme.");
  }

  return scaling;
}
