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

#include "crc_calculator_clmul_impl.h"
#include "immintrin.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

// Register for converting data representation between big endian (BE) to little endian (LE).
static const __m128i crc_xmm_be_le_swap128 = _mm_setr_epi32(0x0c0d0e0f, 0x08090a0b, 0x04050607, 0x00010203);

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

static constexpr std::array<uint8_t, 48> crc_xmm_shift_tab = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static inline __m128i xmm_shift_right(__m128i reg, const unsigned num)
{
  const __m128i* p = reinterpret_cast<const __m128i*>(&crc_xmm_shift_tab[16 + num]);

  return _mm_shuffle_epi8(reg, _mm_loadu_si128(p));
}

static inline __m128i xmm_shift_left(__m128i reg, const unsigned num)
{
  const __m128i* p = reinterpret_cast<const __m128i*>(&crc_xmm_shift_tab[16 - num]);

  return _mm_shuffle_epi8(reg, _mm_loadu_si128(p));
}

static inline __m128i crc32_folding_round(const __m128i data_block, const __m128i k1_k2, const __m128i fold)
{
  __m128i tmp = _mm_clmulepi64_si128(fold, k1_k2, 0x11);

  return _mm_xor_si128(_mm_clmulepi64_si128(fold, k1_k2, 0x00), _mm_xor_si128(data_block, tmp));
}

static inline __m128i crc32_reduce_128_to_64(__m128i data128, const __m128i k3_q)
{
  __m128i tmp = _mm_xor_si128(_mm_clmulepi64_si128(data128, k3_q, 0x01 /* k3 */), data128);

  data128 = _mm_xor_si128(_mm_clmulepi64_si128(tmp, k3_q, 0x01 /* k3 */), data128);

  return _mm_srli_si128(_mm_slli_si128(data128, 8), 8);
}

static inline __m128i mm_safe_load_si128(span<const uint8_t> data)
{
  alignas(64) std::array<uint8_t, 16> temp_data;
  span<uint8_t>                       simd_data = temp_data;

  srsvec::copy(simd_data.first(data.size()), data);
  srsvec::zero(simd_data.last(16 - data.size()));

  return _mm_load_si128(reinterpret_cast<const __m128i*>(simd_data.data()));
}

static inline uint32_t crc32_reduce_64_to_32(__m128i fold, const __m128i k3_q, const __m128i p_res)
{
  __m128i temp;

  temp = _mm_clmulepi64_si128(_mm_srli_si128(fold, 4), k3_q, 0x10 /* Q */);
  temp = _mm_srli_si128(_mm_xor_si128(temp, fold), 4);
  temp = _mm_clmulepi64_si128(temp, p_res, 0 /* P */);
  return _mm_extract_epi32(_mm_xor_si128(temp, fold), 0);
}

static inline uint32_t crc32_calc_pclmulqdq(span<const uint8_t> data, const crc_pclmulqdq_parameters& params)
{
  // Add 4 bytes to data block size. This is to secure the following: CRC32 = M(X)^32 mod P(X) where M(X) is the message
  // to compute CRC on, and P(X) is the CRC polynomial.
  uint32_t data_len = data.size() + 4;

  // Load first 16 data bytes in fold and set swap BE<->LE 16 byte conversion variable.
  __m128i fold = mm_safe_load_si128(data.first(std::min(data.size(), 16UL)));
  __m128i swap = crc_xmm_be_le_swap128;

  // Folding all data into single 16 byte data block.
  if (data_len <= 16) {
    // Data block fits into 16 byte block:
    // - adjust data block, and
    // - 4 least significant bytes need to be zero.
    fold = _mm_shuffle_epi8(fold, swap);
    fold = _mm_slli_si128(xmm_shift_right(fold, 20 - data_len), 4);
  } else {
    // n = number of bytes required to align data_len to multiple of 16.
    uint32_t n = ((~data_len) + 1) & 15U;

    // Apply CRC initial value and get fold to BE format.
    fold = _mm_shuffle_epi8(fold, swap);

    // Load next 16 bytes of data and adjust fold & next_data as follows: CONCAT(fold,next_data) >> (n*8)
    __m128i next_data = mm_safe_load_si128(data.subspan(16, std::min(data.size() - 16UL, 16UL)));
    next_data         = _mm_shuffle_epi8(next_data, swap);
    next_data         = _mm_or_si128(xmm_shift_right(next_data, n), xmm_shift_left(fold, 16 - n));
    fold              = xmm_shift_right(fold, n);

    if (data_len <= 32) {
      // In such unlikely case clear 4 least significant bytes.
      next_data = _mm_slli_si128(_mm_srli_si128(next_data, 4), 4);
    }

    // Do the initial folding round on 2 first 16 byte chunks.
    __m128i k = _mm_set_epi64x(params.k2, params.k1);
    fold      = crc32_folding_round(next_data, k, fold);

    if (data_len > 32) {
      // data_block needs to be at least 48 bytes long in order to get here.
      __m128i new_data;

      // Main folding loop:
      //  - n is adjusted to point to next 16 data block to read (16+16) = 2x16; represents 2 first data blocks
      //    processed above (- n) is the number of zero bytes padded to the message in order to align it to 16 bytes;
      //  - the last 16 bytes are processed separately.
      for (n = 16 + 16 - n; n < (data_len - 16U); n += 16) {
        new_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[n]));
        new_data = _mm_shuffle_epi8(new_data, swap);
        fold     = crc32_folding_round(new_data, k, fold);
      }

      // The last folding round works always on 12 bytes (12 bytes of data and 4 zero bytes). Read from offset -4 is to
      // avoid one shift right operation.
      new_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&data[n - 4]));
      new_data = _mm_shuffle_epi8(new_data, swap);
      new_data = _mm_slli_si128(new_data, 4);
      fold     = crc32_folding_round(new_data, k, fold);
    }
  }

  // Reduction from 128 to 64 bits.
  __m128i k = _mm_set_epi64x(params.q, params.k3);
  fold      = crc32_reduce_128_to_64(fold, k);

  // Reduction from 128 to 32 bits.
  return crc32_reduce_64_to_32(fold, k, _mm_set_epi64x(0UL, params.p));
}

crc_calculator_checksum_t crc_calculator_clmul_impl::calculate_byte(span<const uint8_t> data)
{
  if (poly == crc_generator_poly::CRC24A) {
    return crc32_calc_pclmulqdq(data, crc24A_ctx) >> 8U;
  }

  if (poly == crc_generator_poly::CRC24B) {
    return crc32_calc_pclmulqdq(data, crc24B_ctx) >> 8U;
  }

  return crc_calc_lut.calculate_byte(data);
}

crc_calculator_checksum_t crc_calculator_clmul_impl::calculate_bit(span<const uint8_t> data)
{
  return crc_calc_lut.calculate_bit(data);
}

crc_calculator_checksum_t crc_calculator_clmul_impl::calculate(const bit_buffer& data)
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

  // Reset the LUT algorithm with the current CRC.
  crc_calc_lut.reset(crc);

  // Extract from the data the latest byte with zero padding.
  uint8_t last_byte = data.extract(nbytes * 8, remainder_nbits) << (8U - remainder_nbits);

  // Insert the latest byte into the LUT algorithm.
  crc_calc_lut.put_byte(last_byte);

  // Update checksum.
  crc = crc_calc_lut.get_checksum();

  // Reset CRC prior to reverse.
  crc_calc_lut.reset(crc);

  // Reverse the padded.
  crc_calc_lut.reversecrcbit(8 - remainder_nbits);

  return crc_calc_lut.get_checksum();
}
