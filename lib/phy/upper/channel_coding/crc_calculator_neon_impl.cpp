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

#include "crc_calculator_neon_impl.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include <arm_neon.h>

using namespace srsran;

namespace {

// Collects algorithm parameters.
struct crc_pclmulqdq_parameters {
  // K1 = remainder X^128 / P(X)
  int64_t k1;
  // K2 = remainder X^192 / P(X)
  int64_t k2;
  // K3 = remainder X^64 / P(X)
  int64_t k3;
  // q = quotient X^64 / P(X)
  int64_t q;
  // p = polynomial / P(X)
  int64_t p;
};

} // namespace

static constexpr crc_pclmulqdq_parameters crc24A_ctx = {0x64e4d700, 0x2c8c9d00, 0xd9fe8c00, 0xf845fe24, 0x864cfb00};
static constexpr crc_pclmulqdq_parameters crc24B_ctx = {0x80140500, 0x42000100, 0x90042100, 0xffff83ff, 0x80006300};

static inline uint64x2_t neon_bswap(uint64x2_t input)
{
  uint8x16_t input_bytes    = vreinterpretq_u8_u64(input);
  uint8x16_t reversed_64    = vrev64q_u8(input_bytes);
  uint8x16_t reversed_bytes = vextq_u8(reversed_64, reversed_64, 8);
  return vreinterpretq_u64_u8(reversed_bytes);
}

static inline uint64x2_t neon_shift_right(uint64x2_t input, unsigned num)
{
  uint8x16_t              input_bytes = vreinterpretq_u8_u64(input);
  std::array<uint8_t, 32> temp        = {};
  vst1q_u8(temp.data(), input_bytes);
  uint8x16_t rotated_bytes = vld1q_u8(temp.data() + num);
  return vreinterpretq_u64_u8(rotated_bytes);
}

static inline uint64x2_t neon_shift_right_4(uint64x2_t input)
{
  uint8x16_t input_u8 = vreinterpretq_u8_u64(input);
  return vreinterpretq_u64_u8(vextq_u8(input_u8, vdupq_n_u8(0), 4));
}

static inline uint64x2_t neon_shift_left(uint64x2_t input, unsigned num)
{
  uint8x16_t              input_bytes = vreinterpretq_u8_u64(input);
  std::array<uint8_t, 32> temp        = {};
  vst1q_u8(temp.data() + 16, input_bytes);
  uint8x16_t rotated_bytes = vld1q_u8(temp.data() + 16 - num);
  return vreinterpretq_u64_u8(rotated_bytes);
}

static inline uint64x2_t neon_shift_left_4(uint64x2_t input)
{
  uint8x16_t input_u8 = vreinterpretq_u8_u64(input);
  return vreinterpretq_u64_u8(vextq_u8(vdupq_n_u8(0), input_u8, 12));
}

static inline uint64x2_t neon_clear_lower_4byte(uint64x2_t input)
{
  uint32x4_t input_u32  = vreinterpretq_u32_u64(input);
  uint32x4_t result_u32 = vsetq_lane_u32(0, input_u32, 0);
  return vreinterpretq_u64_u32(result_u32);
}

static inline uint64x2_t neon_vmull_p64(uint64x2_t a_u64x2, uint64x2_t b_u64x2, unsigned opcode)
{
  uint64x1_t a_u64 = vget_low_u64(a_u64x2);
  uint64x1_t b_u64 = vget_low_u64(b_u64x2);
  if ((opcode & 0x01) != 0) {
    a_u64 = vget_high_u64(a_u64x2);
  }
  if ((opcode & 0x10) != 0) {
    b_u64 = vget_high_u64(b_u64x2);
  }
  return vreinterpretq_u64_p128(
      vmull_p64((poly64_t)vreinterpret_p64_u64(a_u64), (poly64_t)vreinterpret_p64_u64(b_u64)));
}

static inline uint64x2_t crc32_folding_round(const uint64x2_t data_block, const uint64x2_t k1_k2, const uint64x2_t fold)
{
  uint64x2_t tmp = neon_vmull_p64(fold, k1_k2, 0x11);
  return veorq_u64(neon_vmull_p64(fold, k1_k2, 0x00), veorq_u64(data_block, tmp));
}

static inline uint64x2_t crc32_reduce_128_to_64(uint64x2_t data128, const uint64x2_t k3_q)
{
  uint64x2_t tmp = veorq_u64(neon_vmull_p64(data128, k3_q, 0x01), data128);

  data128 = veorq_u64(neon_vmull_p64(tmp, k3_q, 0x01), data128);

  return vsetq_lane_u64(0, data128, 1);
}

static inline uint64x2_t neon_safe_load(span<const uint8_t> data)
{
  alignas(64) std::array<uint8_t, 16> temp_data;
  span<uint8_t>                       simd_data = temp_data;

  srsvec::copy(simd_data.first(data.size()), data);
  srsvec::zero(simd_data.last(16 - data.size()));

  return vld1q_u64(reinterpret_cast<const uint64_t*>(simd_data.data()));
}

static inline uint32_t crc32_reduce_64_to_32(uint64x2_t fold, const uint64x2_t k3_q, const uint64x2_t p_res)
{
  uint64x2_t temp;

  temp = neon_vmull_p64(neon_shift_right_4(fold), k3_q, 0x10);
  temp = neon_shift_right_4(veorq_u64(temp, fold));
  temp = neon_vmull_p64(temp, p_res, 0x00);
  return vgetq_lane_u32(vreinterpretq_u32_u64(veorq_u64(temp, fold)), 0);
}

static inline uint64x2_t neon_set_u64(uint64_t high, uint64_t low)
{
  // Combine two 64-bit integers into a 128-bit vector
  return vcombine_u64(vdup_n_u64(low), vdup_n_u64(high));
}

static inline uint32_t crc32_calc_pclmulqdq(span<const uint8_t> data, const crc_pclmulqdq_parameters& params)
{
  // Add 4 bytes to data block size. This is to secure the following: CRC32 = M(X)^32 mod P(X) where M(X) is the message
  // to compute CRC on, and P(X) is the CRC polynomial.
  uint32_t data_len = data.size() + 4;

  // Load first 16 data bytes in fold and set swap BE<->LE 16 byte conversion variable.
  uint64x2_t fold = neon_safe_load(data.first(std::min(data.size(), 16UL)));

  // Folding all data into single 16 byte data block.
  if (data_len <= 16) {
    // Data block fits into 16 byte block:
    // - adjust data block, and
    // - 4 least significant bytes need to be zero.
    fold = neon_bswap(fold);
    fold = neon_shift_right(fold, 16 - data_len);
    fold = neon_clear_lower_4byte(fold);
  } else {
    // n = number of bytes required to align data_len to multiple of 16.
    uint32_t n = ((~data_len) + 1) & 15U;

    // Apply CRC initial value and get fold to BE format.
    fold = neon_bswap(fold);

    // Load next 16 bytes of data and adjust fold & next_data as follows: CONCAT(fold,next_data) >> (n*8)
    uint64x2_t next_data = neon_safe_load(data.subspan(16, std::min(data.size() - 16UL, 16UL)));
    next_data            = neon_bswap(next_data);
    next_data            = vorrq_u64(neon_shift_right(next_data, n), neon_shift_left(fold, 16 - n));
    fold                 = neon_shift_right(fold, n);

    if (data_len <= 32) {
      // In such unlikely case clear 4 least significant bytes.
      next_data = neon_clear_lower_4byte(next_data);
    }

    // Do the initial folding round on 2 first 16 byte chunks.
    uint64x2_t k = neon_set_u64(params.k2, params.k1);
    fold         = crc32_folding_round(next_data, k, fold);

    if (data_len > 32) {
      // data_block needs to be at least 48 bytes long in order to get here.
      uint64x2_t new_data;

      // Main folding loop:
      //  - n is adjusted to point to next 16 data block to read (16+16) = 2x16; represents 2 first data blocks
      //    processed above (- n) is the number of zero bytes padded to the message in order to align it to 16 bytes;
      //  - the last 16 bytes are processed separately.
      for (n = 16 + 16 - n; n < (data_len - 16U); n += 16) {
        new_data = vld1q_u64(reinterpret_cast<const uint64_t*>(&data[n]));
        new_data = neon_bswap(new_data);
        fold     = crc32_folding_round(new_data, k, fold);
      }

      // The last folding round works always on 12 bytes (12 bytes of data and 4 zero bytes). Read from offset -4 is to
      // avoid one shift right operation.
      new_data = vld1q_u64(reinterpret_cast<const uint64_t*>(&data[n - 4]));
      new_data = neon_bswap(new_data);
      new_data = neon_shift_left_4(new_data);
      fold     = crc32_folding_round(new_data, k, fold);
    }
  }

  // Reduction from 128 to 64 bits.
  uint64x2_t k = neon_set_u64(params.q, params.k3);
  fold         = crc32_reduce_128_to_64(fold, k);

  // Reduction from 128 to 32 bits.
  return crc32_reduce_64_to_32(fold, k, neon_set_u64(0UL, params.p));
}

crc_calculator_checksum_t crc_calculator_neon_impl::calculate_byte(span<const uint8_t> data) const
{
  if (poly == crc_generator_poly::CRC24A) {
    return crc32_calc_pclmulqdq(data, crc24A_ctx) >> 8U;
  }

  if (poly == crc_generator_poly::CRC24B) {
    return crc32_calc_pclmulqdq(data, crc24B_ctx) >> 8U;
  }

  return crc_calc_lut.calculate_byte(data);
}

crc_calculator_checksum_t crc_calculator_neon_impl::calculate_bit(span<const uint8_t> data) const
{
  return crc_calc_lut.calculate_bit(data);
}

crc_calculator_checksum_t crc_calculator_neon_impl::calculate(const bit_buffer& data) const
{
  unsigned nbytes          = data.size() / 8;
  unsigned remainder_nbits = data.size() % 8;

  uint32_t crc;
  if (poly == crc_generator_poly::CRC24A) {
    crc = crc32_calc_pclmulqdq(data.get_buffer().first(nbytes), crc24A_ctx) >> 8U;
  } else if (poly == crc_generator_poly::CRC24B) {
    crc = crc32_calc_pclmulqdq(data.get_buffer().first(nbytes), crc24B_ctx) >> 8U;
  } else {
    crc = crc_calc_lut.calculate_byte(data.get_buffer().first(nbytes));
  }

  // Return the current CRC if no remainder bits.
  if (remainder_nbits == 0) {
    return crc;
  }

  // Extract from the data the latest byte with zero padding.
  uint8_t last_byte = data.extract(nbytes * 8, remainder_nbits) << (8U - remainder_nbits);

  // Insert the latest byte into the LUT algorithm.
  crc = crc_calc_lut.put_byte(crc, last_byte);

  // Update checksum.
  crc = crc_calc_lut.get_checksum(crc);

  // Reverse the padded.
  crc = crc_calc_lut.reversecrcbit(crc, 8 - remainder_nbits);

  return crc_calc_lut.get_checksum(crc);
}
