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

#ifdef __x86_64__
#include <immintrin.h>
#endif // __x86_64__

using namespace srsran;
using namespace srsvec;

namespace {

template <typename InType = uint8_t>
void unpack_8bit(span<uint8_t> unpacked, InType value)
{
  srsran_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());

  // Unpack a byte by copying each bit into a byte MSB within a 64-bit register.
  uint64_t unpacked_ = ((static_cast<uint64_t>(value) * 0x8040201008040201) & 0x8080808080808080) >> 7UL;

  // Store the unpacked data.
  std::memcpy(unpacked.data(), &unpacked_, sizeof(uint64_t));
}

template <typename RetType = uint8_t>
RetType pack_8bit(span<const uint8_t> unpacked)
{
  srsran_assert(unpacked.size() == 8, "The amount of data to pack (i.e., {}) must be eight.", unpacked.size());

  // Load unpacked data.
  uint64_t unpacked_ = 0;
  std::memcpy(&unpacked_, unpacked.data(), sizeof(uint64_t));

  // Mask the unpacked bits.
  unpacked_ &= 0x101010101010101UL;

  // Pack data and select the first eight MSB.
  uint64_t packed = (unpacked_ * 0x8040201008040201UL) >> 56UL;

  return static_cast<RetType>(packed);
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

void srsran::srsvec::bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed)
{
  srsran_assert(packed.size() == unpacked.size(),
                "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
                packed.size(),
                unpacked.size());
  // Read/write byte index.
  unsigned i_byte = 0;

#if defined(__AVX512F__) && defined(__AVX512VBMI__) && defined(__AVX512BW__)
  const __mmask64* packed_ptr = reinterpret_cast<const __mmask64*>(packed.get_buffer().data());
  for (unsigned i_byte_end = (packed.size() / 64) * 8; i_byte != i_byte_end; i_byte += 8) {
    // Load 64 bits in a go as a mask.
    __mmask64 blend_mask = *(packed_ptr++);

    // Load 64 bits in a go.
    __m512i reg = _mm512_mask_blend_epi8(blend_mask, _mm512_set1_epi8(0), _mm512_set1_epi8(1));

    // Reverses bits within bytes.
    __m512i permute_mask = _mm512_set_epi8(
        // clang-format off
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        // clang-format on
    );
    reg = _mm512_permutexvar_epi8(permute_mask, reg);

    // Store register.
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(unpacked.data()), reg);

    // Advance unpacked buffer.
    unpacked = unpacked.last(unpacked.size() - 64);
  }
#endif // defined(__AVX512F__) && defined(__AVX512VBMI__) && defined(__AVX512BW__)

#if defined(__AVX__) && defined(__AVX2__)
  span<const uint8_t> packed_buffer = packed.get_buffer();

  for (unsigned i_byte_end = (packed.size() / 32) * 4; i_byte != i_byte_end; i_byte += 4) {
    // Load input in different registers.
    __m256i in = _mm256_setzero_si256();
    in         = _mm256_insert_epi8(in, packed_buffer[i_byte + 0], 0);
    in         = _mm256_insert_epi8(in, packed_buffer[i_byte + 1], 8);
    in         = _mm256_insert_epi8(in, packed_buffer[i_byte + 2], 16);
    in         = _mm256_insert_epi8(in, packed_buffer[i_byte + 3], 24);

    // Repeats each byte 8 times.
    __m256i shuffle_mask = _mm256_setr_epi8(
        // clang-format off
        0, 0, 0, 0, 0, 0, 0, 0,
        8, 8, 8, 8, 8, 8, 8, 8,
        0, 0, 0, 0, 0, 0, 0, 0,
        8, 8, 8, 8, 8, 8, 8, 8
        // clang-format off
    );
    in = _mm256_shuffle_epi8(in, shuffle_mask);

    // Selects each of the bits.
    __m256i mask = _mm256_set_epi8(
        // clang-format off
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128,
        1, 2, 4, 8, 16, 32, 64, -128
        // clang-format on
    );
    mask = _mm256_and_si256(mask, in);

    //  Moves the selected bit to the LSB of each output byte.
    mask = ~_mm256_cmpeq_epi8(mask, _mm256_setzero_si256());
    mask = _mm256_and_si256(mask, _mm256_set1_epi8(1));

    // Store register.
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(unpacked.data()), mask);

    // Advance unpacked buffer.
    unpacked = unpacked.last(unpacked.size() - 32);
  }
#endif // defined(__AVX__) && defined(__AVX2__)

  for (unsigned i_byte_end = packed.size() / 8; i_byte != i_byte_end; ++i_byte) {
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
    unpack_8bit(unpacked2, packed.extract(8 * i_byte, unpacked.size()));
    srsvec::copy(unpacked, unpacked2.last(unpacked.size()));
  }
}

void srsran::srsvec::bit_unpack(span<uint8_t> unpacked, const bit_buffer& packed, unsigned offset)
{
  // Calculate the number of bits to align the packed data to byte boundary.
  unsigned nof_head_bits = std::min((8 - (offset % 8)) % 8, static_cast<unsigned>(unpacked.size()));
  if (nof_head_bits != 0) {
    // Extract the alignment bits.
    uint8_t head_bits = packed.extract(offset, nof_head_bits);

    // Unpack the head.
    unpacked = bit_unpack(unpacked, head_bits, nof_head_bits);
  }

  unsigned         aligned_offset = divide_ceil(offset, 8) * 8;
  const bit_buffer aligned_packed = packed.last(packed.size() - aligned_offset).first(unpacked.size());
  bit_unpack(unpacked, aligned_packed);
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

void srsran::srsvec::bit_pack(bit_buffer& packed, span<const uint8_t> unpacked)
{
  srsran_assert(packed.size() == unpacked.size(),
                "The packed number of bits (i.e.{}) must be equal to the number of unpacked bits (i.e., {}).",
                packed.size(),
                unpacked.size());
  unsigned i_byte = 0;

#if defined(__AVX512F__) && defined(__AVX512VBMI__) && defined(__AVX512BW__)
  __mmask64* packed_ptr = reinterpret_cast<__mmask64*>(packed.get_buffer().data());
  for (unsigned i_byte_end = (packed.size() / 64) * 8; i_byte != i_byte_end; i_byte += 8) {
    // Load the 64 input values into an AVX-512 register.
    __m512i reg = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(unpacked.data()));

    // Reverses bits within bytes.
    __m512i permute_mask = _mm512_set_epi8(
        // clang-format off
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        // clang-format on
    );
    reg = _mm512_permutexvar_epi8(permute_mask, reg);

    // Extract the LSBs using a bitwise AND operation.
    *(packed_ptr++) = _mm512_test_epi8_mask(reg, _mm512_set1_epi8(1));

    unpacked = unpacked.last(unpacked.size() - 64);
  }
#endif // defined(__AVX512F__) && defined(__AVX512VBMI__) && defined(__AVX512BW__)

  for (unsigned i_byte_end = packed.size() / 8; i_byte != i_byte_end; ++i_byte) {
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

void srsran::srsvec::bit_pack(srsran::bit_buffer& packed, unsigned offset, span<const uint8_t> unpacked)
{
  // Calculate the number of bits to align the packed data to byte boundary.
  unsigned nof_head_bits = std::min((8 - (offset % 8)) % 8, static_cast<unsigned>(unpacked.size()));
  if (nof_head_bits != 0) {
    // Pack the bits of the head.
    unsigned head_bits = bit_pack(unpacked, nof_head_bits);

    // Insert the packed head bits.
    packed.insert(head_bits, offset, nof_head_bits);
  }

  unsigned   aligned_offset = divide_ceil(offset, 8) * 8;
  bit_buffer aligned_packed = packed.last(packed.size() - aligned_offset).first(unpacked.size());
  bit_pack(aligned_packed, unpacked);
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
#ifdef __AVX2__
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
#endif // __AVX2__
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

  unsigned i_bit = 0;
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

    // Increment input offset.
    i_bit += nof_words * bits_per_word;
  }

  for (; remaining_bits != 0;) {
    // Extract at most one byte from the input.
    unsigned bits_to_extract = std::min(remaining_bits, 8U);
    uint8_t  byte            = input.extract(i_bit + in_offset, bits_to_extract);

    // Insert the extracted bits into the output buffer.
    output.insert(byte, i_bit + out_offset, bits_to_extract);

    remaining_bits -= bits_to_extract;
    i_bit += bits_to_extract;
  }
}
