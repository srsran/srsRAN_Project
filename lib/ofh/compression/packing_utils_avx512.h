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

#pragma once

#include "avx512_helpers.h"
#include "srsran/support/error_handling.h"

namespace srsran {
namespace ofh {
namespace mm512 {

/// \brief Packs 16bit IQ values of the PRB as 9 bit values in big-endian format.
///
/// \param[out] comp_prb_buffer Buffer dedicated for storing compressed packed bytes of the PRB.
/// \param[in] reg              AVX512 register storing 16bit IQ samples of the PRB.
inline void avx512_pack_prb_9b_big_endian(span<uint8_t> comp_prb_buffer, __m512i reg)
{
  static constexpr unsigned BYTES_PER_PRB_9BIT_COMPRESSION = 27;
  static constexpr unsigned bytes_per_lane                 = BYTES_PER_PRB_9BIT_COMPRESSION / 3;
  static constexpr unsigned lane_write_mask                = 0x01ff;

  srsran_assert(comp_prb_buffer.size() == BYTES_PER_PRB_9BIT_COMPRESSION,
                "Output buffer has incorrect size for packing compressed samples");

  // Input IQ samples need to be shifted in order to align bits before final packing.
  // 0:  i0 0  0  0  0  0  0  0   i8 i7 i6 i5 i4 i3 i2 i1   <- rotate right by 1 (shift left by 7, swap bytes later)
  // 1:  0 q8 q7 q6 q5 q4 q3 q2   q1 q0  0  0  0  0  0  0   <- shift left by 6
  // 2:  0  0 i8 i7 i6 i5 i4 i3   i2 i1 i0  0  0  0  0  0   <- shift left by 5
  // 3:  0  0  0 q8 q7 q6 q5 q4   q3 q2 q1 q0  0  0  0  0   <- shift left by 4
  // 4:  0  0  0  0 i8 i7 i6 i5   i4 i3 i2 i1 i0  0  0  0   <- shift left by 3
  // 5:  0  0  0  0  0 q8 q7 q6   q5 q4 q3 q2 q1 q0  0  0   <- shift left by 2
  // 6:  0  0  0  0  0  0 i8 i7   i6 i5 i4 i3 i2 i1 i0  0   <- shift left by 1
  // 7:  0  0  0  0  0  0  0 q8   q7 q6 q5 q4 q3 q2 q1 q0   <- no shift
  // ... the same pattern in every 128bit lane ...

  const __m512i shiftv_epi16 =
      _mm512_set_epi16(0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7);
  __m512i iq_shl_epi16 = _mm512_sllv_epi16(reg, shiftv_epi16);

  // Mask 16bit words to keep only 9 shifted bits.
  const __m512i mask_epi16 = _mm512_set_epi64(0x01ff03fe07fc0ff8,
                                              0x1ff03fe07fc0ff80,
                                              0x01ff03fe07fc0ff8,
                                              0x1ff03fe07fc0ff80,
                                              0x01ff03fe07fc0ff8,
                                              0x1ff03fe07fc0ff80,
                                              0x01ff03fe07fc0ff8,
                                              0x1ff03fe07fc0ff80);
  iq_shl_epi16             = _mm512_and_si512(iq_shl_epi16, mask_epi16);

  // Shuffle it and create two new vectors that can be OR'ed to produce final result. Temporal vectors look as follows:
  // 0  0  0  0  0  0  0  0  | i0 0  0  0  0  0  0  0 | q1 q0  0  0  0  0  0  0 | i2 i1 i0  0  0  0  0  0 | ...
  // i8 i7 i6 i5 i4 i3 i2 i1 | 0 q8 q7 q6 q5 q4 q3 q2 |  0  0 i8 i7 i6 i5 i4 i3 | 0  0  0  q8 q7 q6 q5 q4 | ...
  __m512i tmp_iq_0_epi8 = _mm512_shuffle_epi8(iq_shl_epi16,
                                              _mm512_setr_epi64(0x0c0a0806040200ff,
                                                                0xffffffffffffff0e,
                                                                0x0c0a0806040200ff,
                                                                0xffffffffffffff0e,
                                                                0x0c0a0806040200ff,
                                                                0xffffffffffffff0e,
                                                                0xffffffffffffffff,
                                                                0xffffffffffffffff));
  __m512i tmp_iq_1_epi8 = _mm512_shuffle_epi8(iq_shl_epi16,
                                              _mm512_setr_epi64(0x0f0d0b0907050301,
                                                                0xffffffffffffffff,
                                                                0x0f0d0b0907050301,
                                                                0xffffffffffffffff,
                                                                0x0f0d0b0907050301,
                                                                0xffffffffffffffff,
                                                                0xffffffffffffffff,
                                                                0xffffffffffffffff));

  // Perform 'bitwise OR'.
  __m512i iq_packed_epi8 = _mm512_or_si512(tmp_iq_0_epi8, tmp_iq_1_epi8);

  // Store first 9 bytes of the first three 128bit lanes of the AVX512 register.
  uint8_t* data = comp_prb_buffer.data();
  _mm_mask_storeu_epi8(data, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 0));
  _mm_mask_storeu_epi8(data + bytes_per_lane, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 1));
  _mm_mask_storeu_epi8(data + bytes_per_lane * 2, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 2));
}

/// \brief Packs 16bit IQ values of the PRB as 14 bit values in big-endian format.
///
/// \param[out] comp_prb_buffer Buffer dedicated for storing compressed packed bytes of the PRB.
/// \param[in] reg              AVX512 register storing 16bit IQ samples of the PRB.
inline void avx512_pack_prb_14b_big_endian(span<uint8_t> comp_prb_buffer, __m512i reg)
{
  // Number of bytes used by 1 packed PRB with IQ samples compressed to 14 bits.
  static constexpr unsigned BYTES_PER_PRB_14BIT_COMPRESSION = 42;
  static constexpr unsigned bytes_per_lane                  = BYTES_PER_PRB_14BIT_COMPRESSION / 3;
  static constexpr unsigned lane_write_mask                 = 0x3fff;

  srsran_assert(comp_prb_buffer.size() == BYTES_PER_PRB_14BIT_COMPRESSION,
                "Output buffer has incorrect size for packing compressed samples");

  // We will use logical operations for 32bit words. I and Q components represented as 32bit words:
  // |            byte3          |            byte2        |               byte1        |           byte0          |
  // | s s q13 q12 q11 q10 q9 q8 | q7 q6 q5 q4 q3 q2 q1 q0 | s  s i13 i12 i11 i10 i9 i8 | i7 i6 i5 i4 i3 i2  i1 i0 |
  // where s is a sign bit.
  //
  // The results of the following operations are written down below:
  // - shift32 left first word by 2 and mask.
  // - shift32 right first word by 4.
  // - shift32 left second word by 14 and mask.
  // - shift32 right second word by 16.
  //
  // | 0 0 0 0 0   0   0   0   | 0   0   0  0  0  0  0  0  |i13 i12 i11 i10 i9  i8  i7 i6 | i5 i4 i3 i2 i1 i0 0  0 |
  // | 0 0 0 0 0   0   q13 q12 | q11 q10 q9 q8 q7 q6 q5 q4 |q3  q2  q1  q0  0   0   0  0  | 0  0  0  0  0  0  0  0 |
  // | 0 0 0 0 i13 i12 i11 i10 | i9  i8  i7 i6 i5 i4 i3 i2 |i1  i0  0   0   0   0   0  0  | 0  0  0  0  0  0  0  0 |
  // | 0 0 0 0 0   0   0   0   | 0   0   0  0  0  0  0  0  |0   0   q13 q12 q11 q10 q9 q8 | q7 q6 q5 q4 q3 q2 q1 q0|

  // Shift left by 2 and 14 bits and mask the result.
  const __m512i k_shl_epi32      = _mm512_setr_epi32(2, 14, 2, 14, 2, 14, 2, 14, 2, 14, 2, 14, 2, 14, 2, 14);
  const __m512i mask_sll_epi16   = _mm512_setr_epi32(0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000,
                                                   0xffff,
                                                   0xfffc000);
  __m512i       in_reg_sll_epi32 = _mm512_and_si512(_mm512_sllv_epi32(reg, k_shl_epi32), mask_sll_epi16);

  // Shift right by 4 and 16 bits and mask the result.
  const __m512i k_shr_epi32      = _mm512_setr_epi32(4, 16, 4, 16, 4, 16, 4, 16, 4, 16, 4, 16, 4, 16, 4, 16);
  const __m512i mask_srl_epi32   = _mm512_setr_epi32(0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff,
                                                   0x3fff000,
                                                   0x3fff);
  __m512i       in_reg_srl_epi32 = _mm512_and_si512(_mm512_srlv_epi32(reg, k_shr_epi32), mask_srl_epi32);

  // Create new vectors by shuffling left and right shifted values. Two resulting vectors can be ORed and the result
  // will be stored in a big-endian format.
  __m512i shuffled_sll_epi8 = _mm512_shuffle_epi8(in_reg_sll_epi32,
                                                  _mm512_setr_epi64(0x0904050607ff0001,
                                                                    0xffff0c0d0e0fff08,
                                                                    0x0904050607ff0001,
                                                                    0xffff0c0d0e0fff08,
                                                                    0x0904050607ff0001,
                                                                    0xffff0c0d0e0fff08,
                                                                    0x0904050607ff0001,
                                                                    0xffff0c0d0e0fff08));
  __m512i shuffled_srl_epi8 = _mm512_shuffle_epi8(in_reg_srl_epi32,
                                                  _mm512_setr_epi64(0x0804050601020300,
                                                                    0xffff0c0d0e090a0b,
                                                                    0x0804050601020300,
                                                                    0xffff0c0d0e090a0b,
                                                                    0x0804050601020300,
                                                                    0xffff0c0d0e090a0b,
                                                                    0x0804050601020300,
                                                                    0xffff0c0d0e090a0b));
  // Perform logical OR between the two vectors.
  __m512i result_epi8 = _mm512_or_si512(shuffled_sll_epi8, shuffled_srl_epi8);

  // Store first 14 bytes of the first three 128bit lanes of the AVX512 register storing the packing results.
  uint8_t* data = comp_prb_buffer.data();
  _mm_mask_storeu_epi8(data, lane_write_mask, _mm512_extracti64x2_epi64(result_epi8, 0));
  _mm_mask_storeu_epi8(data + bytes_per_lane, lane_write_mask, _mm512_extracti64x2_epi64(result_epi8, 1));
  _mm_mask_storeu_epi8(data + bytes_per_lane * 2, lane_write_mask, _mm512_extracti64x2_epi64(result_epi8, 2));
}

/// \brief Packs 16bit IQ values of the PRB using big-endian format.
///
/// \param[out] comp_prb_buffer Buffer dedicated for storing compressed packed bytes of the PRB.
/// \param[in] reg              AVX512 register storing 16bit IQ samples of the PRB.
inline void avx512_pack_prb_16b_big_endian(span<uint8_t> comp_prb_buffer, __m512i reg)
{
  // Number of bytes used by 1 packed PRB with IQ samples compressed to 16 bits.
  static constexpr unsigned BYTES_PER_PRB_16BIT_COMPRESSION = 48;

  static constexpr unsigned write_mask = 0xffffff;

  srsran_assert(comp_prb_buffer.size() == BYTES_PER_PRB_16BIT_COMPRESSION,
                "Output buffer has incorrect size for packing compressed samples");

  // Input contains 24 16 bit Iand Q samples.
  uint8_t* data = comp_prb_buffer.data();

  // Swap bytes to convert from big-endian format and write them directly to the output memory.
  const __m512i shuffle_mask_epi8 = _mm512_setr_epi64(0x0607040502030001,
                                                      0x0e0f0c0d0a0b0809,
                                                      0x1617141512131011,
                                                      0x1e1f1c1d1a1b1819,
                                                      0x2627242522232021,
                                                      0x2e2f2c2d2a2b2829,
                                                      0x3637343532333031,
                                                      0x3e3f3c3d3a3b3839);

  __m512i reg_swp_epi16 = _mm512_shuffle_epi8(reg, shuffle_mask_epi8);
  _mm512_mask_storeu_epi16(data, write_mask, reg_swp_epi16);
}

/// \brief Packs 16bit IQ values of a resource block using the specified width and big-endian format.
///
/// \param[out] comp_prb_buffer Buffer dedicated for storing compressed packed bytes of the PRB.
/// \param[in]  reg             AVX512 register storing 16bit IQ pairs of the PRB.
/// \param[in] iq_width         Bit width of the resulting packed IQ samples.
inline void pack_prb_big_endian(span<uint8_t> comp_prb_buffer, __m512i reg, unsigned iq_width)
{
  if (iq_width == 9) {
    return avx512_pack_prb_9b_big_endian(comp_prb_buffer, reg);
  }
  if (iq_width == 14) {
    return avx512_pack_prb_14b_big_endian(comp_prb_buffer, reg);
  }
  if (iq_width == 16) {
    return avx512_pack_prb_16b_big_endian(comp_prb_buffer, reg);
  }
  report_fatal_error("Unsupported bit width");
}

/// \brief Unpacks packed 9bit IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of 27 packed bytes.
///
/// \note The \c unpacked_iq_data parameter should be sized to store 32 output IQ samples: it is 24 IQ samples of one RB
/// rounded up to 64-byte boundary required by AVX512 intrinsics.
inline void avx512_unpack_prb_9b_be(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data)
{
  // Load input, 27 bytes (fits in one AVX512 register).
  static constexpr __mmask64 mask             = (1UL << 27) - 1UL;
  __m512i                    packed_data_epi8 = _mm512_maskz_loadu_epi8(mask, packed_data.data());

  // Duplicate input words (it is required since below in the code every byte will be used twice:
  // to provide MSB bits of the current IQ sample and LSB bits of the previous IQ sample).
  __m512i packed_data_0_epi8 = _mm512_mask_permutexvar_epi64(
      _mm512_set1_epi64(0), 0xff, _mm512_setr_epi64(0, 1, 1, 2, 2, 3, 0, 0), packed_data_epi8);

  // Swap bytes for little-endian representation. Each pair of bytes contains all bits of a single I or Q sample.
  const __m512i shuffle_mask_epi8 = _mm512_setr_epi64(0x0304020301020001,
                                                      0x0708060705060405,
                                                      0x0405030402030102,
                                                      0x0809070806070506,
                                                      0x0506040503040203,
                                                      0x090a080907080607,
                                                      0xffffffffffffffff,
                                                      0xffffffffffffffff);

  __m512i packed_data_0_le_epi16 = _mm512_shuffle_epi8(packed_data_0_epi8, shuffle_mask_epi8);

  // Shift left to align to 16bit boundary, then shift right to sign-extend values.
  const __m512i shl_mask_epi16 =
      _mm512_set_epi16(7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0);
  __m512i unpacked_data_epi16 = _mm512_srai_epi16(_mm512_sllv_epi16(packed_data_0_le_epi16, shl_mask_epi16), 7);

  // Write results to the output buffer.
  _mm512_mask_storeu_epi16(unpacked_iq_data.data(), 0x00ffffff, unpacked_data_epi16);
}

/// \brief Unpacks packed 14bit IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of 42 packed bytes.
inline void avx512_unpack_prb_14b_be(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data)
{
  // Number of packed bytes stored in the given array.
  static constexpr unsigned BYTES_PER_PRB_14BIT_COMPRESSION = 42;
  // Load only 42 bytes into the 64 bytes long AVX512 register.
  static constexpr __mmask64 mask = (1UL << BYTES_PER_PRB_14BIT_COMPRESSION) - 1UL;

  // Load input.
  __m512i packed_data_epi8 = _mm512_maskz_loadu_epi8(mask, packed_data.data());

  // Consider input bytes represented as 64-bit words:
  // b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0
  // As a first step we shuffle input data so that every byte is used twice.
  // The resulting 16-bit words, except two of them, will contain all bits of either I or Q component.
  //
  // Let's denote shuffled b5_b6 | b3_b4 | b1_b2 | b0_b1 vector as in_shuffled_1_epi16, where
  // b0_b1: | i13 i12 i11 i10 i9  i8  i7  i6  | i5  i4  i3 i2 i1 i0 q13 q12 |  Has all bits of I0 component.
  // b1_b2: | i5  i4  i3  i2  i1  i0  q13 q12 | q11 q10 q9 q8 q7 q6 q5  q4  |  Missing q3 q2 q1 q0 of Q0 component (b3).
  // b3_b4: | q3  q2  q1  q0  i13 i12 i11 i10 | i9  i8  i7 i6 i5 i4 i3  i2  |  Missing i1 i0 of I1 component (b5).
  // b5_b6: | i1  i0  q13 q12 q11 q10 q9  q8  | q7  q6  q5 q4 q3 q2 q1  q0  |  Has all bits of Q1 component.
  const __m512i shuffle_mask1_epi8 = _mm512_setr_epi64(0x0506030401020001,
                                                       0x0c0d0a0b08090708,
                                                       0x131411120f100e0f,
                                                       0x1a1b181916171516,
                                                       0x21221f201d1e1c1d,
                                                       0x2829262724252324,
                                                       0x2f302d2e2b2c2a2b,
                                                       0x3637343532333132);

  __m512i tmp_in_shuffled_1_epi16 = _mm512_permutexvar_epi8(shuffle_mask1_epi8, packed_data_epi8);

  // Shift left to align with 16-bit boundary, then shift right to sign-extend values.
  const __m512i shl_mask_epi16 =
      _mm512_set_epi16(2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4, 6, 0);
  __m512i in_shuffled_1_epi16 = _mm512_srai_epi16(_mm512_sllv_epi16(tmp_in_shuffled_1_epi16, shl_mask_epi16), 2);

  // Create a vector of missing LSB bits,
  // take them from input b3 and b5 of every 64-bit word:
  //
  // b3: | q3 q2 q1  q0  i13 i12 i11 i10 |  Right shift by 4 to put at LSB positions.
  // b5: | i1 i0 q13 q12 q11 q10 q9  q8  |  Right shift by 6 to put at LSB positions.
  const __m512i shuffle_mask2_epi8 = _mm512_setr_epi64(0x0000000500030000,
                                                       0x0000000c000a0000,
                                                       0x0000001300110000,
                                                       0x0000001a00180000,
                                                       0x00000021001f0000,
                                                       0x0000002800260000,
                                                       0x0000002f002d0000,
                                                       0x0000003600340000);

  __m512i tmp_in_shuffled_2_epi16 = _mm512_permutexvar_epi8(shuffle_mask2_epi8, packed_data_epi8);

  // Shift right permuted bytes to align with in_shuffled_1_epi16.
  const __m512i shift_right_mask_epi16 = _mm512_set1_epi64(0x0000000600040000);
  __m512i       in_shuffled_2_epi16    = _mm512_srlv_epi16(tmp_in_shuffled_2_epi16, shift_right_mask_epi16);

  // Finally, select bits between the two vectors.
  const __m512i select_mask_epi64 = _mm512_set1_epi64(0x00000003000f0000);
  __m512i       unpacked_data_epi64 =
      _mm512_ternarylogic_epi64(in_shuffled_1_epi16, in_shuffled_2_epi16, select_mask_epi64, 0xd8);

  // Write results to the output buffer.
  _mm512_mask_storeu_epi16(unpacked_iq_data.data(), 0x00ffffff, unpacked_data_epi64);
}

/// \brief Unpacks packed 16bit IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of 48 packed bytes.
inline void avx512_unpack_prb_16b_be(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data)
{
  // Load input, 48 bytes (fits in one AVX512 register).
  const __mmask32 rw_mask           = 0x00ffffff;
  __m512i         packed_data_epi16 = _mm512_maskz_loadu_epi16(rw_mask, packed_data.data());
  // Swap bytes for Little-endian representation.
  const __m512i shuffle_mask_epi8 = _mm512_setr_epi64(0x0607040502030001,
                                                      0x0e0f0c0d0a0b0809,
                                                      0x1617141512131011,
                                                      0x1e1f1c1d1a1b1819,
                                                      0x2627242522232021,
                                                      0x2e2f2c2d2a2b2829,
                                                      0x3637343532333031,
                                                      0x3e3f3c3d3a3b3839);

  __m512i unpacked_data_epi16 = _mm512_shuffle_epi8(packed_data_epi16, shuffle_mask_epi8);
  _mm512_mask_storeu_epi16(unpacked_iq_data.data(), rw_mask, unpacked_data_epi16);
}

/// \brief Unpacks packed IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 32 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of input packed bytes.
/// \param[in] iq_width          Bit width of the packed IQ samples.
///
/// \note The \c unpacked_iq_data parameter should be sized to store 32 output IQ samples: it is 24 IQ samples of one RB
/// rounded up to 64-byte boundary required by AVX512 intrinsics.
inline void unpack_prb_big_endian(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data, unsigned iq_width)
{
  if (iq_width == 9) {
    return avx512_unpack_prb_9b_be(unpacked_iq_data, packed_data);
  }
  if (iq_width == 14) {
    return avx512_unpack_prb_14b_be(unpacked_iq_data, packed_data);
  }
  if (iq_width == 16) {
    return avx512_unpack_prb_16b_be(unpacked_iq_data, packed_data);
  }
  report_fatal_error("Unsupported bit width");
}

/// \brief Checks whether the requested bit width is supported by the AVX512 implementation.
/// \param[in] iq_width Requested bit width.
///
/// \return True in case packing/unpacking with the requested bit width is supported.
inline bool iq_width_packing_supported(unsigned iq_width)
{
  return ((iq_width == 9) || (iq_width == 14) || (iq_width == 16));
}

} // namespace mm512
} // namespace ofh
} // namespace srsran
