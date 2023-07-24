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

#include "packing_utils_avx512.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ofh;

/// Number of bytes used by 1 packed PRB with IQ samples compressed to 9 bits.
static constexpr unsigned BYTES_PER_PRB_9BIT_COMPRESSION = 27;

bool mm512::iq_width_packing_supported(unsigned iq_width)
{
  if (iq_width == 9) {
    return true;
  }
  return false;
}

/// \brief Packs 16bit IQ values of the PRB using given bit width and big-endian format.
///
/// \param[out] c_prb Compressed PRB object storing packed bytes.
/// \param[in] reg    AVX512 register storing 16bit IQ samples of the PRB.
static void avx512_pack_prb_9b_big_endian(compressed_prb& c_prb, __m512i reg)
{
  static constexpr unsigned bytes_per_lane  = BYTES_PER_PRB_9BIT_COMPRESSION / 3;
  static constexpr unsigned lane_write_mask = 0x01ff;

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
  uint8_t* data = c_prb.get_byte_buffer().data();
  _mm_mask_storeu_epi8(data, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 0));
  _mm_mask_storeu_epi8(data + bytes_per_lane, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 1));
  _mm_mask_storeu_epi8(data + bytes_per_lane * 2, lane_write_mask, _mm512_extracti64x2_epi64(iq_packed_epi8, 2));

  c_prb.set_stored_size(BYTES_PER_PRB_9BIT_COMPRESSION);
}

void mm512::pack_prb_big_endian(compressed_prb& c_prb, __m512i reg, unsigned iq_width)
{
  if (iq_width == 9) {
    return avx512_pack_prb_9b_big_endian(c_prb, reg);
  }
  report_fatal_error("Unsupported bit width requested");
}

/// \brief Unpacks packed 9bit IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of 27 packed bytes.
///
/// \note The \c unpacked_iq_data parameter should be sized to store 32 output IQ samples: it is 24 IQ samples of one RB
/// rounded up to 64-byte boundary required by AVX512 intrinsics.
static void avx512_unpack_prb_9b_be(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data)
{
  // Load input, 27 bytes (fits in one AVX512 register).
  __m512i packed_data_epi8 = _mm512_mask_loadu_epi8(_mm512_set1_epi64(0), 0x1fffffff, packed_data.data());

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
  _mm512_mask_storeu_epi16(unpacked_iq_data.data(), 0xffffffff, unpacked_data_epi16);
}

void mm512::unpack_prb_big_endian(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data, unsigned iq_width)
{
  if (iq_width == 9) {
    return avx512_unpack_prb_9b_be(unpacked_iq_data, packed_data);
  }
  report_fatal_error("Unsupported bit width requested");
}
