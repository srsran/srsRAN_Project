/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "modulation_mapper_neon_impl.h"
#include <arm_neon.h>

using namespace srsran;

/// \brief Implements a generic QAM modulator using neon instruction sets.
///
/// It modulates 16 symbols in every call. Modulates the bits contained in \c input. Each byte in \c input corresponds
/// to a symbol.
///
/// \tparam QM Modulation order.
/// \param[out] output Pointer to store the modulated symbols.
/// \param[in]  input  Data to modulate. Only the QM least significant bits contain information.
template <unsigned QM>
void generic_modulator(ci8_t* output, uint8x16_t input)
{
  int8x16_t   offset = vdupq_n_s8(-1);
  int8x16x2_t result;
  result.val[0] = vdupq_n_s8(0);
  result.val[1] = vdupq_n_s8(0);

  for (unsigned j = 0, j_end = QM / 2; j != j_end; ++j) {
    result.val[0] = vaddq_s8(result.val[0], offset);
    result.val[1] = vaddq_s8(result.val[1], offset);
    offset        = vaddq_s8(offset, offset);

    uint8x16_t real_mask = vceqq_u8(vandq_u8(vdupq_n_u8((1U << (2U * j + 1U))), input), vdupq_n_u8(0));
    uint8x16_t imag_mask = vceqq_u8(vandq_u8(vdupq_n_u8((1U << (2U * j + 0U))), input), vdupq_n_u8(0));

    int8x16_t real_n = vnegq_s8(result.val[0]);
    int8x16_t imag_n = vnegq_s8(result.val[1]);
    result.val[0]    = vbslq_s8(real_mask, real_n, result.val[0]);
    result.val[1]    = vbslq_s8(imag_mask, imag_n, result.val[1]);
  }

  vst2q_s8(reinterpret_cast<int8_t*>(output), result);
}

float modulation_mapper_neon_impl::modulate_qpsk(span<ci8_t> symbols, const bit_buffer& input)
{
  const uint8_t* input_ptr   = input.get_buffer().data();
  ci8_t*         symbols_ptr = symbols.data();
  unsigned       i_symbol    = 0;

  // Performs modulation mapping in batches of 64 symbols.
  for (unsigned i_symbol_end = (symbols.size() / 64) * 64; i_symbol != i_symbol_end; i_symbol += 64) {
    // Load a batch of 16 bytes.
    uint8x16_t in_u8 = vld1q_u8(input_ptr);
    input_ptr += 16;

    // Calculate 2-bit indices:
    uint8x16x4_t indices_u8;
    indices_u8.val[0] = vshrq_n_u8(in_u8, 6);
    indices_u8.val[1] = vandq_u8(vshrq_n_u8(in_u8, 4), vdupq_n_u8(0b11));
    indices_u8.val[2] = vandq_u8(vshrq_n_u8(in_u8, 2), vdupq_n_u8(0b11));
    indices_u8.val[3] = vandq_u8(in_u8, vdupq_n_u8(0b11));

    // Interleave indices.
    std::array<uint8_t, 64> indices;
    vst4q_u8(indices.data(), indices_u8);

    // Modulate in 4 batches of 16 symbols each.
    for (unsigned i = 0; i != 4; ++i) {
      generic_modulator<2>(symbols_ptr, vld1q_u8(&indices[16 * i]));
      symbols_ptr += 16;
    }
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(2 * remainder), modulation_scheme::QPSK);
}

float modulation_mapper_neon_impl::modulate_qam16(span<ci8_t> symbols, const bit_buffer& input)
{
  const uint8_t* input_ptr   = input.get_buffer().data();
  ci8_t*         symbols_ptr = symbols.data();
  unsigned       i_symbol    = 0;

  // Performs modulation mapping in batches of 32 symbols.
  for (unsigned i_symbol_end = (symbols.size() / 32) * 32; i_symbol != i_symbol_end; i_symbol += 32) {
    // Load a batch of 16 bytes.
    uint8x16_t in_u8 = vld1q_u8(input_ptr);
    input_ptr += 16;

    // Calculate 4-bit indices:
    uint8x16x2_t indices_u8;
    indices_u8.val[0] = vshrq_n_u8(in_u8, 4);
    indices_u8.val[1] = vandq_u8(in_u8, vdupq_n_u8(0xf));

    // Interleave indices.
    std::array<uint8_t, 32> indices;
    vst2q_u8(indices.data(), indices_u8);

    // Modulate in 2 batches of 16 symbols each.
    for (unsigned i = 0; i != 2; ++i) {
      generic_modulator<4>(symbols_ptr, vld1q_u8(&indices[16 * i]));
      symbols_ptr += 16;
    }
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(4 * remainder), modulation_scheme::QAM16);
}

float modulation_mapper_neon_impl::modulate_qam64(span<ci8_t> symbols, const bit_buffer& input)
{
  const uint8_t* input_ptr   = input.get_buffer().data();
  ci8_t*         symbols_ptr = symbols.data();
  unsigned       i_symbol    = 0;

  // Performs modulation mapping in batches of 64 symbols.
  for (unsigned i_symbol_end = (symbols.size() / 64) * 64; i_symbol != i_symbol_end; i_symbol += 64) {
    // Load and deinterleave 3 groups of 16 bytes.
    uint8x16x3_t in_u8 = vld3q_u8(input_ptr);
    input_ptr += 48;

    // Extract deinterleaved registers.
    uint8x16_t byte0 = in_u8.val[0];
    uint8x16_t byte1 = in_u8.val[1];
    uint8x16_t byte2 = in_u8.val[2];

    // Calculate 6-bit indices:
    // index0 = byte0 >> 2U
    // index1 = ((byte0 & 0b11U) << 4U) | (byte1 >> 4U)
    // index2 = ((byte1 & 0b1111U) << 2U) | (byte2 >> 6U);
    // index3 = byte2 & 0b111111U;
    uint8x16x4_t indices_u8;
    indices_u8.val[0] = vshrq_n_u8(byte0, 2);
    indices_u8.val[1] = vorrq_u8(vshlq_n_u8(vandq_u8(byte0, vdupq_n_u8(0b11U)), 4U), vshrq_n_u8(byte1, 4));
    indices_u8.val[2] = vorrq_u8(vshlq_n_u8(vandq_u8(byte1, vdupq_n_u8(0b1111U)), 2U), vshrq_n_u8(byte2, 6));
    indices_u8.val[3] = vandq_u8(byte2, vdupq_n_u8(0b111111U));

    // Interleave indices.
    std::array<uint8_t, 64> indices;
    vst4q_u8(indices.data(), indices_u8);

    // Modulate in 4 batches of 16 symbols each.
    for (unsigned i = 0; i != 4; ++i) {
      generic_modulator<6>(symbols_ptr, vld1q_u8(&indices[16 * i]));
      symbols_ptr += 16;
    }
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(6 * remainder), modulation_scheme::QAM64);
}

float modulation_mapper_neon_impl::modulate_qam256(span<ci8_t> symbols, const bit_buffer& input)
{
  for (unsigned i_symbol = 0, i_symbol_end = (symbols.size() / 16) * 16; i_symbol != i_symbol_end; i_symbol += 16) {
    // Load input.
    uint8x16_t avx_in = vld1q_u8(input.get_buffer().data() + i_symbol);

    // Call generic QAM modulation mapper based on neon instruction sets.
    generic_modulator<8>(symbols.data() + i_symbol, avx_in);
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() % 64;
  return lut_modulator.modulate(symbols.last(remainder), input.last(8 * remainder), modulation_scheme::QAM256);
}

void modulation_mapper_neon_impl::modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  return lut_modulator.modulate(symbols, input, scheme);
}

float modulation_mapper_neon_impl::modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  if (scheme == modulation_scheme::QAM256) {
    return modulate_qam256(symbols, input);
  }

  if (scheme == modulation_scheme::QAM64) {
    return modulate_qam64(symbols, input);
  }

  if (scheme == modulation_scheme::QAM16) {
    return modulate_qam16(symbols, input);
  }

  if (scheme == modulation_scheme::QPSK) {
    return modulate_qpsk(symbols, input);
  }

  return lut_modulator.modulate(symbols, input, scheme);
}
