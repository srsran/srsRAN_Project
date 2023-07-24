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

#include "packing_utils_avx2.h"
#include "srsran/adt/span.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ofh;

/// Number of bytes used by 1 packed resource block with IQ samples compressed to 9 bits.
static constexpr unsigned BYTES_PER_PRB_9BIT_COMPRESSION = 27;

bool mm256::iq_width_packing_supported(unsigned iq_width)
{
  if (iq_width == 9) {
    return true;
  }
  return false;
}

/// \brief Packs 16bit IQ samples in one AVX2 register using 9bit big-endian format.
///
/// \param[in] cmp_data_epi16 AVX2 register storing 16bit IQ samples.
/// \return An AVX2 register containing 18 packed bytes.
///
/// \note Each 128bit lane of the input contains eight I and Q values, with each value consisting of 9 bits of data (the
/// LSB ones) and 7 bits of padding. The function packs the eight IQ samples (only the data bits, discarding the
/// padding) into the first 9 bytes of the respective 128bit lane of the output register (the remaining 7 bytes are
/// padding). Thus, the return value contains 18 packed data bytes in total.
static __m256i pack_avx2_register_9b_be(__m256i cmp_data_epi16)
{
  // Input IQ samples need to be shifted in order to align bits before final packing.
  // 0:  i0 0  0  0  0  0  0  0   i8 i7 i6 i5 i4 i3 i2 i1   <- rotate right by 1 (shift left by 7, swap bytes later)
  // 1:  0 q8 q7 q6 q5 q4 q3 q2   q1 q0  0  0  0  0  0  0   <- shift left by 6
  // 2:  0  0 i8 i7 i6 i5 i4 i3   i2 i1 i0  0  0  0  0  0   <- shift left by 5
  // 3:  0  0  0 q8 q7 q6 q5 q4   q3 q2 q1 q0  0  0  0  0   <- shift left by 4
  // 4:  0  0  0  0 i8 i7 i6 i5   i4 i3 i2 i1 i0  0  0  0   <- shift left by 3
  // 5:  0  0  0  0  0 q8 q7 q6   q5 q4 q3 q2 q1 q0  0  0   <- shift left by 2
  // 6:  0  0  0  0  0  0 i8 i7   i6 i5 i4 i3 i2 i1 i0  0   <- shift left by 1
  // 7:  0  0  0  0  0  0  0 q8   q7 q6 q5 q4 q3 q2 q1 q0   <- no shift
  // ... the same pattern ...
  //
  // Note, AVX2 only allows shifting 32bit words using vector of indexes, so we do it twice.
  const __m256i shift_0_epi32      = _mm256_setr_epi32(7, 5, 3, 1, 7, 5, 3, 1);
  const __m256i shift_1_epi32      = _mm256_setr_epi32(6, 4, 2, 0, 6, 4, 2, 0);
  __m256i       cmp_data_sh0_epi16 = _mm256_sllv_epi32(cmp_data_epi16, shift_0_epi32);
  __m256i       cmp_data_sh1_epi16 = _mm256_sllv_epi32(cmp_data_epi16, shift_1_epi32);
  // Mask 16bit words to keep only 9 shifted bits.
  const __m256i mask_even_epi16 =
      _mm256_setr_epi32(0x0000ff80, 0x00003fe0, 0x00000ff8, 0x000003fe, 0x0000ff80, 0x00003fe0, 0x00000ff8, 0x000003fe);
  const __m256i mask_odd_epi16 =
      _mm256_setr_epi32(0x7fc00000, 0x1ff00000, 0x07fc0000, 0x01ff0000, 0x7fc00000, 0x1ff00000, 0x07fc0000, 0x01ff0000);
  cmp_data_sh0_epi16 = _mm256_and_si256(cmp_data_sh0_epi16, mask_even_epi16);
  cmp_data_sh1_epi16 = _mm256_and_si256(cmp_data_sh1_epi16, mask_odd_epi16);

  // Merge two vectors to get vector of shifted IQ samples as explained above.
  __m256i cmp_data_shifted_epi16 = _mm256_or_si256(cmp_data_sh0_epi16, cmp_data_sh1_epi16);

  // Shuffle it and create two new vectors that can be OR'ed to produce the final result.
  __m256i cmp_data_v0_epi16 = _mm256_shuffle_epi8(
      cmp_data_shifted_epi16,
      _mm256_setr_epi64x(0x0c0a0806040200ff, 0xffffffffffffff0e, 0x0c0a0806040200ff, 0xffffffffffffff0e));
  __m256i cmp_data_v1_epi16 = _mm256_shuffle_epi8(
      cmp_data_shifted_epi16,
      _mm256_setr_epi64x(0x0f0d0b0907050301, 0xffffffffffffffff, 0x0f0d0b0907050301, 0xffffffffffffffff));
  // Perform 'bitwise OR' and return the result.
  return _mm256_or_si256(cmp_data_v0_epi16, cmp_data_v1_epi16);
}

/// \brief Packs 16bit IQ values of the two input RBs using 9bit big-endian format.
///
/// The following diagram shows the input format. Here RBx stands for one unique RE (pair of IQ samples, 32 bits long)
/// pertaining to a respective RB):
/// |       |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- |
/// | \c r0: | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 |
/// | \c r1: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c r2: | RB1 RB1 | RB1 RB1 | RB1 RB1 | RB1 RB1 |
///
/// \param[out] c_prbs Span of two compressed PRBs.
/// \param[in] r0      AVX2 register storing 16bit IQ samples of the first RB.
/// \param[in] r1      AVX2 register storing 16bit IQ samples of the first and second RB.
/// \param[in] r2      AVX2 register storing 16bit IQ samples of the second RB.
static void avx2_pack_prbs_9b_big_endian(span<compressed_prb> c_prbs, __m256i r0, __m256i r1, __m256i r2)
{
  srsran_assert(c_prbs.size() == 2, "Output span must contain 2 resource blocks");

  // Returns first 18 packed bytes out of 27 for the first RB (9 bytes in each 128bit lane - same applies below).
  __m256i reg0_packed_epi8 = pack_avx2_register_9b_be(r0);
  // Returns last 9 packed bytes out of 27 for the first RB and first 9 packed bytes for the second RB.
  __m256i reg1_packed_epi8 = pack_avx2_register_9b_be(r1);
  // Returns last 18 packed bytes out of 27 for the second RB.
  __m256i reg2_packed_epi8 = pack_avx2_register_9b_be(r2);

  // AVX2 doesn't provide instruction to store specific bytes of the lanes using a mask, so we create new vectors,
  // and use a regular store intrinsic.
  //
  // Next example explains the bytes permutations performed below for each RB to group its packed IQs.
  // r0:       {0xe959c2834776bd64, 0x8b, 0xdc0c7ca5b0ab3a56, 0x31}
  // r1:       {0xb1e93964f248fe46, 0xaf, 0x4f0f368e76fc7a9e, 0x12}
  // r0_tmp0:  {0xe959c2834776bd64, 0x000000000000008b, 0x0c7ca5b0ab3a5600, 0x00000000000031dc}
  // r1_tmp0:  {0x3964f248fe460000, 0x0000000000afb1e9, 0x0000000000000000, 0x0000000000000000}
  // r0_tmp1:  {0xe959c2834776bd64, 0x0c7ca5b0ab3a5600, 0x00000000000031dc, 0x0000000000000000}
  // r1_tmp1:  {0x0000000000000000, 0x0000000000000000, 0x3964f248fe460000, 0x0000000000afb1e9}
  // rb0_tmp0: {0xe959c2834776bd64, 0x0c7ca5b0ab3a568b, XXXXXXXXXXXXXXXXXX, XXXXXXXXXXXXXXXXXX}
  // rb0_tmp1: {XXXXXXXXXXXXXXXXXX, XXXXXXXXXXXXXXXXXX, 0x3964f248fe4631dc, 0x0000000000afb1e9}
  // select 64bit words between rb0_tmp0 and rb0_tmp1:
  // rb0:      {0xe959c2834776bd64, 0x0c7ca5b0ab3a568b, 0x3964f248fe4631dc, 0x0000000000afb1e9}

  // Pack the first input RB.
  const __m256i shuffle_mask_0 =
      _mm256_setr_epi64x(0x0706050403020100, 0x0f0e0d0c0b0a0908, 0x06050403020100ff, 0xffffffffffff0807);
  __m256i reg0_tmp0_epi8 = _mm256_shuffle_epi8(reg0_packed_epi8, shuffle_mask_0);

  const __m256i shuffle_mask_1 =
      _mm256_setr_epi64x(0x050403020100ffff, 0xffffffffff080706, 0xffffffffffffffff, 0xffffffffffffffff);
  __m256i reg1_tmp0_epi8 = _mm256_shuffle_epi8(reg1_packed_epi8, shuffle_mask_1);

  __m256i reg0_tmp1_epi8 = _mm256_permutevar8x32_epi32(reg0_tmp0_epi8, _mm256_setr_epi32(0, 1, 4, 5, 6, 7, 3, 3));
  __m256i reg1_tmp1_epi8 = _mm256_permute4x64_epi64(reg1_tmp0_epi8, 0b01001110);

  const __m256i sel_mask_0_epi8 =
      _mm256_setr_epi64x(0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff);
  __m256i prb0_tmp0_epi8 = _mm256_or_si256(reg0_tmp0_epi8, reg0_tmp1_epi8);
  __m256i prb0_tmp1_epi8 = _mm256_or_si256(reg0_tmp1_epi8, reg1_tmp1_epi8);
  __m256i prb0_epi8      = _mm256_blendv_epi8(prb0_tmp0_epi8, prb0_tmp1_epi8, sel_mask_0_epi8);

  // Pack the second input RB.
  const __m256i shuffle_mask_2 =
      _mm256_setr_epi64x(0xffffffffffff0807, 0x06050403020100ff, 0x050403020100ffff, 0xffffffffff080706);
  __m256i reg1_tmp2_epi8 = _mm256_permutevar8x32_epi32(reg1_packed_epi8, _mm256_setr_epi32(4, 5, 6, 7, 3, 3, 3, 3));
  __m256i reg2_tmp0_epi8 = _mm256_shuffle_epi8(reg2_packed_epi8, shuffle_mask_2);
  __m256i reg2_tmp1_epi8 = _mm256_permutevar8x32_epi32(reg2_tmp0_epi8, _mm256_setr_epi32(1, 1, 2, 3, 0, 1, 6, 7));

  const __m256i sel_mask_1_epi8 =
      _mm256_setr_epi64x(0x0000000000000000, 0x0000000000000000, 0xffffffffffffffff, 0xffffffffffffffff);
  __m256i prb1_tmp0_epi8 = _mm256_or_si256(reg1_tmp2_epi8, reg2_tmp1_epi8);
  __m256i prb1_tmp1_epi8 = _mm256_or_si256(reg2_tmp0_epi8, reg2_tmp1_epi8);
  __m256i prb1_epi8      = _mm256_blendv_epi8(prb1_tmp0_epi8, prb1_tmp1_epi8, sel_mask_1_epi8);

  // Write resource blocks to the memory.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(c_prbs[0].get_byte_buffer().data()), prb0_epi8);
  c_prbs[0].set_stored_size(BYTES_PER_PRB_9BIT_COMPRESSION);

  _mm256_storeu_si256(reinterpret_cast<__m256i*>(c_prbs[1].get_byte_buffer().data()), prb1_epi8);
  c_prbs[1].set_stored_size(BYTES_PER_PRB_9BIT_COMPRESSION);
}

void mm256::pack_prbs_big_endian(span<compressed_prb> c_prbs, __m256i r0, __m256i r1, __m256i r2, unsigned iq_width)
{
  switch (iq_width) {
    case 9:
      avx2_pack_prbs_9b_big_endian(c_prbs, r0, r1, r2);
      break;
    default:
      report_fatal_error("Unsupported bit width requested");
  }
}

/// \brief Unpacks packed 9bit IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of 27 packed bytes.
///
/// \note The \c unpacked_iq_data parameter should be sized to store 32 output IQ samples: it is 24 IQ samples of one RB
/// rounded up to 32-byte boundary required by AVX2 intrinsics.
static void unpack_prb_9b_be(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data)
{
  constexpr size_t avx2_size_short_words = 16;
  srsran_assert(unpacked_iq_data.size() >= avx2_size_short_words * 2, "Wrong unpacked data span size");

  // Load input, 27 bytes (fits in one AVX2 register).
  __m256i packed_data_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(packed_data.data()));

  // Duplicate input words (it is required since below in the code every byte will be used twice:
  // to provide MSB bits of the current IQ sample and LSB bits of the previous IQ sample).
  __m256i packed_data_0_epi8 = _mm256_permute4x64_epi64(packed_data_epi8, 0b10010100);
  __m256i packed_data_1_epi8 = _mm256_permute4x64_epi64(packed_data_epi8, 0b00001110);

  // Swap bytes for little-endian representation.
  // As a result each pair of bytes will contain all bits of a single I or Q sample.
  const __m256i shuffle_mask_0_epi8 =
      _mm256_setr_epi64x(0x0304020301020001, 0x0708060705060405, 0x0405030402030102, 0x0809070806070506);
  const __m256i shuffle_mask_1_epi8 =
      _mm256_setr_epi64x(0x0506040503040203, 0x090a080907080607, 0xffffffffffffffff, 0xffffffffffffffff);
  __m256i packed_data_0_le_epi8 = _mm256_shuffle_epi8(packed_data_0_epi8, shuffle_mask_0_epi8);
  __m256i packed_data_1_le_epi8 = _mm256_shuffle_epi8(packed_data_1_epi8, shuffle_mask_1_epi8);

  // Do logical shift left, then arithmetical shift right in order to sign extend values.
  // Note, AVX2 only allows to shift 32bit words using vector of indexes, so we do it twice.
  const __m256i shl_0_epi32 = _mm256_setr_epi32(0, 2, 4, 6, 0, 2, 4, 6);
  const __m256i shl_1_epi32 = _mm256_setr_epi32(1, 3, 5, 7, 1, 3, 5, 7);
  // Mask 16bit words to use only shifted bits.
  const __m256i mask_even_epi16 = _mm256_set1_epi32(0x0000ffff);
  const __m256i mask_odd_epi16  = _mm256_set1_epi32(0xffff0000);

  __m256i unpacked_data_00_epi16 = _mm256_sllv_epi32(packed_data_0_le_epi8, shl_0_epi32);
  __m256i unpacked_data_01_epi16 = _mm256_sllv_epi32(packed_data_0_le_epi8, shl_1_epi32);
  __m256i unpacked_data_10_epi16 = _mm256_sllv_epi32(packed_data_1_le_epi8, shl_0_epi32);
  __m256i unpacked_data_11_epi16 = _mm256_sllv_epi32(packed_data_1_le_epi8, shl_1_epi32);
  unpacked_data_00_epi16         = _mm256_and_si256(unpacked_data_00_epi16, mask_even_epi16);
  unpacked_data_01_epi16         = _mm256_and_si256(unpacked_data_01_epi16, mask_odd_epi16);
  unpacked_data_10_epi16         = _mm256_and_si256(unpacked_data_10_epi16, mask_even_epi16);
  unpacked_data_11_epi16         = _mm256_and_si256(unpacked_data_11_epi16, mask_odd_epi16);

  // Merge two vectors to get vector of shifted 16bit IQ samples, right-shift it to sign-extend values.
  __m256i unpacked_data_0_epi16 = _mm256_srai_epi16(_mm256_or_si256(unpacked_data_00_epi16, unpacked_data_01_epi16), 7);
  __m256i unpacked_data_1_epi16 = _mm256_srai_epi16(_mm256_or_si256(unpacked_data_10_epi16, unpacked_data_11_epi16), 7);

  // Write results.
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(unpacked_iq_data.data()), unpacked_data_0_epi16);
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(unpacked_iq_data.subspan(16, 8).data()), unpacked_data_1_epi16);
}

void mm256::unpack_prb_big_endian(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data, unsigned iq_width)
{
  switch (iq_width) {
    case 9:
      unpack_prb_9b_be(unpacked_iq_data, packed_data);
      break;
    default:
      report_fatal_error("Unsupported bit width requested");
  }
}
