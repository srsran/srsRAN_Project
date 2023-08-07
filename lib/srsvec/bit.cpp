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

#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"

#ifdef HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

using namespace srsran;
using namespace srsvec;

namespace {

template <typename InType = uint8_t>
void unpack_8bit(span<uint8_t> unpacked, InType value)
{
  srsran_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());
#if HAVE_SSE
  // Broadcast 8 bit value in all 8-bit registers.
  __m64 mask = _mm_set1_pi8(static_cast<uint8_t>(value));

  // Mask bits of interest for each 8-bit register.
  mask = _mm_and_si64(mask, _mm_set_pi8(1, 2, 4, 8, 16, 32, 64, -128));

  // Convert to a mask.
  mask = ~_mm_cmpeq_pi8(mask, _mm_setzero_si64());

  // Select least significant bits.
  mask = _mm_and_si64(mask, _mm_set1_pi8(1));

  // Get mask and write
  *reinterpret_cast<__m64*>(unpacked.data()) = mask;
#else
  for (unsigned i_bit = 0, i_bit_end = unpacked.size(); i_bit != i_bit_end; ++i_bit) {
    unpacked[i_bit] = static_cast<uint8_t>(value >> ((i_bit_end - 1) - i_bit)) & 1U;
  }
#endif
}

template <typename RetType = uint8_t>
RetType pack_8bit(span<const uint8_t> unpacked)
{
  srsran_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());
#if HAVE_SSE
  __m64 mask = _mm_cmpgt_pi8(*(reinterpret_cast<const __m64*>(unpacked.data())), _mm_set1_pi8(0));

  // Reverse
  mask = _mm_shuffle_pi8(mask, _mm_set_pi8(0, 1, 2, 3, 4, 5, 6, 7));

  // Get mask and write
  return static_cast<RetType>(_mm_movemask_pi8(mask));
#else
  RetType packed = 0;

  for (unsigned i = 0; i < unpacked.size(); i++) {
    packed |= static_cast<RetType>(unpacked[i] << (8 - i - 1U));
  }

  return packed;
#endif
}

} // namespace

span<uint8_t> srsran::srsvec::bit_unpack(span<uint8_t> bits, unsigned value, unsigned nof_bits)
{
  assert(bits.size() >= nof_bits);

  for (unsigned i = 0; i < nof_bits; i++) {
    bits[i] = (value >> (nof_bits - i - 1)) & 0x1;
  }

  return bits.last(bits.size() - nof_bits);
}

void srsran::srsvec::bit_unpack(span<uint8_t> unpacked, span<const uint8_t> packed)
{
  unsigned nbits  = unpacked.size();
  unsigned nbytes = packed.size();
  unsigned i;

  srsran_assert(divide_ceil(nbits, 8) == nbytes, "Inconsistent input sizes");

  for (i = 0; i < nbytes; i++) {
    unpacked = bit_unpack(unpacked, packed[i], 8);
  }
  if (nbits % 8) {
    bit_unpack(unpacked, packed[i] >> (8 - nbits % 8), nbits % 8);
  }
}

void srsran::srsvec::bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed)
{
  srsran_assert(packed.size() == unpacked.size(),
                "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
                packed.size(),
                unpacked.size());
  // Unpack each byte.
  unsigned bit_offset = 0;
  unsigned i_byte     = 0;

#ifdef HAVE_AVX2
  span<const uint8_t> packed_buffer = packed.get_buffer();

  for (unsigned i_byte_end = (packed.size() / 32) * 4; i_byte != i_byte_end; i_byte += 4, bit_offset += 32) {
    __m256i in = _mm256_setzero_si256();

    in = _mm256_insert_epi8(in, packed_buffer[i_byte + 0], 0);
    in = _mm256_insert_epi8(in, packed_buffer[i_byte + 1], 8);
    in = _mm256_insert_epi8(in, packed_buffer[i_byte + 2], 16);
    in = _mm256_insert_epi8(in, packed_buffer[i_byte + 3], 24);

    in = _mm256_shuffle_epi8(
        in,
        _mm256_setr_epi8(
            0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8));

    __m256i mask = _mm256_set_epi8(1,
                                   2,
                                   4,
                                   8,
                                   16,
                                   32,
                                   64,
                                   -128,
                                   1,
                                   2,
                                   4,
                                   8,
                                   16,
                                   32,
                                   64,
                                   -128,
                                   1,
                                   2,
                                   4,
                                   8,
                                   16,
                                   32,
                                   64,
                                   -128,
                                   1,
                                   2,
                                   4,
                                   8,
                                   16,
                                   32,
                                   64,
                                   -128);

    mask = _mm256_and_si256(mask, in);
    mask = ~_mm256_cmpeq_epi8(mask, _mm256_setzero_si256());
    mask = _mm256_and_si256(mask, _mm256_set1_epi8(1));

    _mm256_storeu_si256(reinterpret_cast<__m256i*>(unpacked.data()), mask);

    // Advance unpacked buffer.
    unpacked = unpacked.last(unpacked.size() - 32);
  }
#endif // HAVE_AVX2

  for (unsigned i_byte_end = packed.size() / 8; i_byte != i_byte_end; ++i_byte, bit_offset += 8) {
    // Extract byte.
    uint8_t byte = packed.get_byte(i_byte);
    // Unpack byte.
    unpack_8bit(unpacked.first(8), byte);
    // Advance unpacked buffer.
    unpacked = unpacked.last(unpacked.size() - 8);
  }

  // Unpack remainder bits.
  if (!unpacked.empty()) {
    std::array<uint8_t, 8> temp_unpacked;
    span<uint8_t>          unpacked2 = temp_unpacked;
    unpack_8bit(unpacked2, packed.extract(bit_offset, unpacked.size()));
    srsvec::copy(unpacked, unpacked2.last(unpacked.size()));
  }
}

unsigned srsran::srsvec::bit_pack(span<const uint8_t>& bits, unsigned nof_bits)
{
  srsran_assert(nof_bits <= 32U, "Number of bits ({}) exceeds maximum (32).", nof_bits);

  unsigned value = 0;

  for (unsigned i = 0; i < nof_bits; i++) {
    value |= (unsigned)bits[i] << (nof_bits - i - 1U);
  }

  // Advance pointer
  bits = bits.last(bits.size() - nof_bits);

  return value;
}

unsigned srsran::srsvec::bit_pack(span<const uint8_t> bits)
{
  srsran_assert(bits.size() <= 32U, "Number of bits ({}) exceeds maximum (32).", bits.size());

  unsigned value = 0;

  for (unsigned i = 0, nof_bits = bits.size(); i != nof_bits; i++) {
    value |= (unsigned)bits[i] << (nof_bits - i - 1U);
  }

  return value;
}

void srsran::srsvec::bit_pack(span<uint8_t> packed, span<const uint8_t> unpacked)
{
  srsran_assert(divide_ceil(unpacked.size(), 8) == packed.size(), "Inconsistent input sizes.");

  for (uint8_t& byte : packed) {
    byte     = pack_8bit(unpacked.first(8));
    unpacked = unpacked.last(unpacked.size() - 8);
  }
}

void srsran::srsvec::bit_pack(bit_buffer& packed, span<const uint8_t> unpacked)
{
  srsran_assert(packed.size() == unpacked.size(),
                "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
                packed.size(),
                unpacked.size());
  for (unsigned i_byte = 0, i_byte_end = unpacked.size() / 8; i_byte != i_byte_end; ++i_byte) {
    unsigned byte = pack_8bit<unsigned>(unpacked.first(8));
    unpacked      = unpacked.last(unpacked.size() - 8);
    packed.set_byte(byte, i_byte);
  }

  if (!unpacked.empty()) {
    unsigned               bit_offset = 8 * (packed.size() / 8);
    std::array<uint8_t, 8> temp_unpacked;
    span<uint8_t>          unpacked2 = temp_unpacked;
    srsvec::copy(unpacked2.first(unpacked.size()), unpacked);
    srsvec::zero(unpacked2.last(8 - unpacked.size()));

    unsigned byte = pack_8bit<unsigned>(unpacked2) >> (8 - unpacked.size());
    packed.insert(byte, bit_offset, unpacked.size());
  }
}

void srsran::srsvec::copy_offset(srsran::bit_buffer& output, span<const uint8_t> input, unsigned startpos)
{
  static constexpr unsigned bits_per_word    = 8;
  unsigned                  input_start_word = startpos / bits_per_word;
  unsigned                  input_start_mod  = startpos % bits_per_word;

  using extended_word_t = uint32_t;

  unsigned nof_full_words     = output.size() / bits_per_word;
  unsigned nof_remainder_bits = output.size() % bits_per_word;

  span<uint8_t> buffer = output.get_buffer();

  if (input_start_mod == 0) {
    std::copy_n(input.begin() + input_start_word, nof_full_words, buffer.begin());
  } else {
    unsigned i_word = 0;
#ifdef HAVE_AVX2
    for (unsigned i_word_end = (nof_full_words / 32) * 32; i_word != i_word_end; i_word += 32) {
      __m256i word0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[input_start_word + i_word]));
      __m256i word1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&input[input_start_word + i_word + 1]));
      word0         = _mm256_and_si256(_mm256_slli_epi32(word0, input_start_mod),
                               _mm256_set1_epi8(mask_msb_ones<uint8_t>(bits_per_word - input_start_mod)));
      word1         = _mm256_and_si256(_mm256_srli_epi32(word1, bits_per_word - input_start_mod),
                               _mm256_set1_epi8(mask_lsb_ones<uint8_t>(input_start_mod)));
      __m256i word  = _mm256_or_si256(word0, word1);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(&buffer[i_word]), word);
    }
#endif // HAVE_AVX2
    for (; i_word != nof_full_words; ++i_word) {
      extended_word_t input_word = static_cast<extended_word_t>(input[input_start_word + i_word]) << bits_per_word;
      input_word |= static_cast<extended_word_t>(input[input_start_word + i_word + 1]);
      input_word = input_word >> (bits_per_word - input_start_mod);
      input_word &= mask_lsb_ones<extended_word_t>(bits_per_word);
      buffer[i_word] = static_cast<uint8_t>(input_word);
    }
  }

  if (nof_remainder_bits != 0) {
    extended_word_t remainder_bits = static_cast<extended_word_t>(input[input_start_word + nof_full_words])
                                     << bits_per_word;
    if (input_start_mod + nof_remainder_bits > bits_per_word) {
      remainder_bits |= static_cast<extended_word_t>(input[input_start_word + nof_full_words + 1]);
    }
    remainder_bits = remainder_bits >> (bits_per_word - input_start_mod);
    remainder_bits &= remainder_bits & mask_lsb_ones<extended_word_t>(bits_per_word);
    buffer[nof_full_words] = static_cast<uint8_t>(remainder_bits) & mask_msb_ones<uint8_t>(nof_remainder_bits);
  }
}

void srsran::srsvec::copy_offset(srsran::bit_buffer&       output,
                                 unsigned                  out_offset,
                                 const srsran::bit_buffer& input,
                                 unsigned                  in_offset,
                                 unsigned                  nof_bits)
{
  srsran_assert(input.size() - in_offset >= nof_bits,
                "Input buffer size (i.e., {}) and reading offset (i.e., {}) cannot accomodate copy size of {} bits.",
                input.size(),
                in_offset,
                nof_bits);

  srsran_assert(input.size() - in_offset >= nof_bits,
                "Output buffer size (i.e., {}) and writing offset (i.e., {}) cannot accomodate copy size of {} bits.",
                output.size(),
                out_offset,
                nof_bits);

  // 8 bit per word.
  static constexpr unsigned bits_per_word = 8U;

  unsigned in_start_word = in_offset / bits_per_word;
  unsigned in_start_mod  = in_offset % bits_per_word;

  unsigned out_start_word = out_offset / bits_per_word;
  unsigned out_start_mod  = out_offset % bits_per_word;

  unsigned remaining_bits = nof_bits;

  if ((in_start_mod == 0) && (out_start_mod == 0)) {
    // Determine the number of bit words to copy.
    unsigned nof_words = nof_bits / bits_per_word;

    // Get the buffer views.
    span<uint8_t>       out_buffer = output.get_buffer().subspan(out_start_word, nof_words);
    span<const uint8_t> in_buffer  = input.get_buffer().subspan(in_start_word, nof_words);

    // Copy data.
    srsvec::copy(out_buffer, in_buffer);

    // Compute the remaining bits to copy.
    remaining_bits -= nof_words * bits_per_word;
  }

  for (unsigned i_bit = 0; remaining_bits != 0;) {
    // Extract at most one byte from the input.
    unsigned bits_to_extract = std::min(remaining_bits, 8U);
    uint8_t  byte            = input.extract(i_bit + in_offset, bits_to_extract);

    // Insert the extracted bits into the output buffer.
    output.insert(byte, i_bit + out_offset, bits_to_extract);

    remaining_bits -= bits_to_extract;
    i_bit += bits_to_extract;
  }
}
