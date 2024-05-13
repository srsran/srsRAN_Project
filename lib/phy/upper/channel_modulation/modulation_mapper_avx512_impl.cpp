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

#include "modulation_mapper_avx512_impl.h"
#include "srsran/srsvec/bit.h"
#include <immintrin.h>

using namespace srsran;

/// \brief Implements a generic QAM modulator using AVX512 instruction sets.
///
/// It modulates 64 symbols in every call. Modulates the bits contained in \c input. Each byte in \c input corresponds
/// to a symbol.
///
/// \tparam QM Modulation order.
/// \param[out] output Pointer to store the modulated symbols.
/// \param[in]  input  Data to modulate. Only the QM least significant bits contain information.
template <unsigned QM>
void generic_modulator(ci8_t* output, __m512i input)
{
  // Permute indexes for interleaving the first 64 resultant values.
  static constexpr int8_t idx_[64] = {0,  64, 1,  65, 2,  66, 3,  67, 4,  68, 5,  69, 6,  70, 7,  71,
                                      8,  72, 9,  73, 10, 74, 11, 75, 12, 76, 13, 77, 14, 78, 15, 79,
                                      16, 80, 17, 81, 18, 82, 19, 83, 20, 84, 21, 85, 22, 86, 23, 87,
                                      24, 88, 25, 89, 26, 90, 27, 91, 28, 92, 29, 93, 30, 94, 31, 95};
  // Permute indexes for interleaving the last 64 resultant values.
  static constexpr int8_t idx2_[64] = {32, 96,  33, 97,  34, 98,  35, 99,  36, 100, 37, 101, 38, 102, 39, 103,
                                       40, 104, 41, 105, 42, 106, 43, 107, 44, 108, 45, 109, 46, 110, 47, 111,
                                       48, 112, 49, 113, 50, 114, 51, 115, 52, 116, 53, 117, 54, 118, 55, 119,
                                       56, 120, 57, 121, 58, 122, 59, 123, 60, 124, 61, 125, 62, 126, 63, 127};

  // Load permute indexes into AVX512 registers.
  __m512i idx  = _mm512_loadu_si512(idx_);
  __m512i idx2 = _mm512_loadu_si512(idx2_);

  __m512i offset = _mm512_set1_epi8(-1);
  __m512i real   = _mm512_setzero_si512();
  __m512i imag   = _mm512_setzero_si512();
  for (unsigned j = 0, j_end = QM / 2; j != j_end; ++j) {
    real   = _mm512_add_epi8(real, offset);
    imag   = _mm512_add_epi8(imag, offset);
    offset = _mm512_add_epi8(offset, offset);

    __mmask64 real_mask = _mm512_cmp_epi8_mask(
        _mm512_and_si512(_mm512_set1_epi8((1U << (2U * j + 1U))), input), _mm512_setzero_si512(), _MM_CMPINT_EQ);
    __mmask64 imag_mask = _mm512_cmp_epi8_mask(
        _mm512_and_si512(_mm512_set1_epi8((1U << (2U * j + 0U))), input), _mm512_setzero_si512(), _MM_CMPINT_EQ);

    __m512i real_n = _mm512_sub_epi8(_mm512_setzero_si512(), real);
    __m512i imag_n = _mm512_sub_epi8(_mm512_setzero_si512(), imag);
    real           = _mm512_mask_blend_epi8(real_mask, real, real_n);
    imag           = _mm512_mask_blend_epi8(imag_mask, imag, imag_n);
  }

  __m512i s1 = _mm512_permutex2var_epi8(real, idx, imag);
  __m512i s2 = _mm512_permutex2var_epi8(real, idx2, imag);

  _mm512_storeu_si512(output, s1);
  _mm512_storeu_si512(output + 32, s2);
}

float modulation_mapper_avx512_impl::modulate_qam64(span<ci8_t> symbols, const bit_buffer& input)
{
  // Permute indexes for the different permutations.
  static constexpr int8_t shuffle_idx0[64] = {
      0,  65,  66,  66, 3,  69,  70,  69,  6,  73,  74,  72,  9,  77,  78,  75,  12,  81,  82,  78,  15,  85,
      86, 81,  18,  89, 90, 84,  21,  93,  94, 87,  24,  97,  98, 90,  27,  101, 102, 93,  30,  105, 106, 96,
      33, 109, 110, 99, 36, 113, 114, 102, 39, 117, 118, 105, 42, 121, 122, 108, 45,  125, 126, 111};
  static constexpr int8_t shuffle_idx1[64] = {
      64,  0,  66,  67,  68,  2,  70,  71,  72,  4,  74,  75,  76,  6,  78,  79,  80,  8,   82,  83, 84,  10,
      86,  87, 88,  12,  90,  91, 92,  14,  94,  95, 96,  16,  98,  99, 100, 18,  102, 103, 104, 20, 106, 107,
      108, 22, 110, 111, 112, 24, 114, 115, 116, 26, 118, 119, 120, 28, 122, 123, 124, 30,  126, 127};
  static constexpr int8_t shuffle_idx2[64] = {
      64,  65,  0,  67,  68,  69,  2,  71,  72,  73,  4,  75,  76,  77,  6,   79,  80,  81,  8,   83,  84, 85,
      10,  87,  88, 89,  12,  91,  92, 93,  14,  95,  96, 97,  16,  99,  100, 101, 18,  103, 104, 105, 20, 107,
      108, 109, 22, 111, 112, 113, 24, 115, 116, 117, 26, 119, 120, 121, 28,  123, 124, 125, 30,  127};
  static constexpr int8_t vbswap_idx1[64] = {1,  0,  4,  3,  7,  6,  10, 9,  13, 12, 16, 15, 19, 18, 22, 21,
                                             25, 24, 28, 27, 31, 30, 34, 33, 37, 36, 40, 39, 43, 42, 46, 45,
                                             49, 48, 52, 51, 55, 54, 58, 57, 61, 60, 64, 63, 67, 66, 70, 69,
                                             73, 72, 76, 75, 79, 78, 82, 81, 85, 84, 88, 87, 91, 90, 94, 93};
  static constexpr int8_t vbswap_idx2[64] = {2,  1,  5,  4,  8,  7,  11, 10, 14, 13, 17, 16, 20, 19, 23, 22,
                                             26, 25, 29, 28, 32, 31, 35, 34, 38, 37, 41, 40, 44, 43, 47, 46,
                                             50, 49, 53, 52, 56, 55, 59, 58, 62, 61, 65, 64, 68, 67, 71, 70,
                                             74, 73, 77, 76, 80, 79, 83, 82, 86, 85, 89, 88, 92, 91, 95, 94};

  // Load permute indexes into AVX512 registers.
  __m512i idx0       = _mm512_loadu_si512(shuffle_idx0);
  __m512i idx1       = _mm512_loadu_si512(shuffle_idx1);
  __m512i idx2       = _mm512_loadu_si512(shuffle_idx2);
  __m512i bswap_idx1 = _mm512_loadu_si512(vbswap_idx1);
  __m512i bswap_idx2 = _mm512_loadu_si512(vbswap_idx2);

  const uint8_t* input_ptr    = input.get_buffer().data();
  unsigned       i_symbol     = 0;
  unsigned       i_symbol_end = 0;
  if (symbols.size() >= 64) {
    // The input stride is 48 bytes but the register is 64 bytes. Skip the last 64 symbols to avoid reading out of the
    // input bounds.
    i_symbol_end = ((symbols.size() - 64) / 64) * 64;
  }

  for (; i_symbol != i_symbol_end; i_symbol += 64) {
    // Load all data for 64 symbols.
    __m512i avx_in = _mm512_loadu_si512(input_ptr + (i_symbol * 6) / 8);

    // Shift input two positions to the right.
    __m512i shift2 = _mm512_srli_epi64(avx_in, 2);

    // Rearrange bytes into 16-bit integers, little-endian representation. Skip one byte after every pair of
    // selected bytes:
    // | 1 | 0 | 4 | 3 | 7 | 6 | ...
    // Then, shift four positions to the right.
    __m512i shift4 = _mm512_permutexvar_epi8(bswap_idx1, avx_in);
    shift4         = _mm512_srli_epi16(shift4, 4);

    // Rearrange bytes into 16-bit integers, little-endian representation. Skip one byte before every pair of
    // selected bytes:
    // | 2 | 1 | 5 | 4 | 8 | 7 | ...
    // Then, shift six positions to the right.
    __m512i shift6 = _mm512_permutexvar_epi8(bswap_idx2, avx_in);
    shift6         = _mm512_srli_epi16(shift6, 6);

    // It combines the bytes from all shifts in groups of 4 bytes where:
    // - position 0 is selected from shift2 as the first byte of every three bytes;
    // - position 1 is selected from shift4 as the first byte of every two bytes;
    // - position 2 is selected from shift6 as the first byte of every two bytes; and
    // - position 3 is selected from avx_in the third byte of every three bytes.
    avx_in = _mm512_permutex2var_epi8(shift2, idx0, avx_in);
    avx_in = _mm512_permutex2var_epi8(shift4, idx1, avx_in);
    avx_in = _mm512_permutex2var_epi8(shift6, idx2, avx_in);

    // Select only the 6 LSB of every byte.
    avx_in = _mm512_and_si512(avx_in, _mm512_set1_epi8(0b111111));

    // Call generic QAM modulation mapper based on AVX512 instruction sets.
    generic_modulator<6>(symbols.data() + i_symbol, avx_in);
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() - i_symbol;
  return lut_modulator.modulate(symbols.last(remainder), input.last(6 * remainder), modulation_scheme::QAM64);
}

float modulation_mapper_avx512_impl::modulate_qam256(span<ci8_t> symbols, const bit_buffer& input)
{
  for (unsigned i_symbol = 0, i_symbol_end = (symbols.size() / 64) * 64; i_symbol != i_symbol_end; i_symbol += 64) {
    // Load input.
    __m512i avx_in = _mm512_loadu_si512(input.get_buffer().data() + i_symbol);

    // Call generic QAM modulation mapper based on AVX512 instruction sets.
    generic_modulator<8>(symbols.data() + i_symbol, avx_in);
  }

  // Process the remaining symbols using the LUT implementation.
  unsigned remainder = symbols.size() % 64;
  return lut_modulator.modulate(symbols.last(remainder), input.last(8 * remainder), modulation_scheme::QAM256);
}

void modulation_mapper_avx512_impl::modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  return lut_modulator.modulate(symbols, input, scheme);
}

float modulation_mapper_avx512_impl::modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme)
{
  if (scheme == modulation_scheme::QAM256) {
    return modulate_qam256(symbols, input);
  }

  if (scheme == modulation_scheme::QAM64) {
    return modulate_qam64(symbols, input);
  }

  return lut_modulator.modulate(symbols, input, scheme);
}
