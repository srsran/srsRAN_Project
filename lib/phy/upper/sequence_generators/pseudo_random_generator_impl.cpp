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

#include "pseudo_random_generator_impl.h"
#include "pseudo_random_generator_fast_advance.h"
#include "pseudo_random_generator_initializers.h"
#include "pseudo_random_generator_sequence.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"

#if __SSE3__
#include <immintrin.h>
#endif // __SSE3__

#ifdef __aarch64__
#include <arm_neon.h>
#endif // __aarch64__

using namespace srsran;

/// \brief Parameter \f$N_{\textup{C}}\f$, as defined in TS38.211 Section 5.2.1.
///
/// Corresponds to the delay between the state sequences \f$x_1(n), x_2(n)\f$ and the output sequence \f$c(n) =
/// x_1(n + N_{\textup{C}}) \oplus x_2(n + N_{\textup{C}})\f$.
static constexpr unsigned pseudo_random_generator_Nc = 1600;

/// Maximum number of steps that can be the state advanced using the pseudo-random generator fast advance.
static constexpr unsigned pseudo_random_state_fast_advance_max_steps = 1U << 15U;

/// Sequence \f$x_1(n)\f$ initializer object.
static const pseudo_random_initializer_x1 x1_init(pseudo_random_generator_Nc);

/// Sequence \f$x_2(n)\f$ initializer object.
static const pseudo_random_initializer_x2 x2_init(pseudo_random_generator_Nc);

/// Sequence state fast advance instance.
static const pseudo_random_generator_fast_advance<pseudo_random_state_fast_advance_max_steps> fast_advance;

void pseudo_random_generator_impl::init(unsigned c_init)
{
  x1 = x1_init.get_reverse();
  x2 = x2_init.get_reverse(c_init);
}

void pseudo_random_generator_impl::init(const state_s& state)
{
  x1 = state.x1;
  x2 = state.x2;
}

void pseudo_random_generator_impl::advance(unsigned count)
{
  while (count != 0) {
    // Ceil the number of steps to advance to the maximum number of steps.
    unsigned n = std::min(pseudo_random_state_fast_advance_max_steps, count);

    // Advance sequence states.
    fast_advance.advance(x1, x2, n);

    // Decrement count.
    count -= n;
  }
}

void pseudo_random_generator_impl::generate(bit_buffer& data)
{
  // Number of bits per processing word.
  static constexpr unsigned nof_bits_per_byte = 8;

  // Calculate the maximum number of simultaneous words to process.
  static constexpr unsigned max_nof_bytes_step = 64 / nof_bits_per_byte;

  // Calculate the maximum number of simultaneous bits to process.
  static constexpr unsigned max_nof_bits_step = max_nof_bytes_step * nof_bits_per_byte;

  // Create sequence with the current x1 and x2 states.
  pseudo_random_generator_sequence sequence(x1, x2);

  // Processes batches of 64 bits in parallel.
  for (unsigned i_byte = 0, i_byte_end = (data.size() / max_nof_bits_step) * max_nof_bytes_step; i_byte != i_byte_end;
       i_byte += 8) {
    uint64_t c                 = sequence.step64();
    c                          = __builtin_bswap64(c);
    span<uint8_t> output_chunk = data.get_buffer().subspan(i_byte, max_nof_bytes_step);
    memcpy(output_chunk.data(), &c, max_nof_bytes_step);
  }

  // Process spare bits in a batch of the remainder bits.
  unsigned i_bit     = (data.size() / max_nof_bits_step) * max_nof_bits_step;
  unsigned remainder = data.size() - i_bit;
  unsigned count     = 0;
  while (remainder != 0) {
    unsigned batch_size    = std::min(remainder, pseudo_random_generator_sequence::get_max_step_size());
    unsigned reminder_size = batch_size;
    uint32_t c             = sequence.step(batch_size);

    while (batch_size != 0) {
      // Process per byte basis, ceiling at the remainder.
      unsigned word_size = std::min(batch_size, nof_bits_per_byte);

      // Mask the LSB of the sequence.
      uint32_t mask = mask_lsb_ones<uint32_t>(nof_bits_per_byte);

      // Shift to align the reversed sequence LSB with the remainder bits.
      unsigned right_shift = nof_bits_per_byte - word_size;

      // Calculate the output byte.
      uint8_t output_word = ((c >> (24u - (count * nof_bits_per_byte))) & mask) >> right_shift;

      // Insert the output byte.
      data.insert(output_word, i_bit, word_size);

      // Advance bit index.
      i_bit += word_size;

      // Decrement remainder.
      batch_size -= word_size;
      ++count;
    }
    remainder -= reminder_size;
  }

  x1 = sequence.get_x1();
  x2 = sequence.get_x2();
}

#define FLOAT_U32_XOR(DST, SRC, U32_MASK)                                                                              \
  do {                                                                                                                 \
    uint32_t temp_u32;                                                                                                 \
    memcpy(&temp_u32, &(SRC), 4);                                                                                      \
    temp_u32 ^= (U32_MASK);                                                                                            \
    memcpy(&(DST), &temp_u32, 4);                                                                                      \
  } while (false)

void pseudo_random_generator_impl::generate(span<float> out, float value)
{
  uint32_t i      = 0;
  unsigned length = out.size();

  // Number of bits that can be processed with a SIMD register.
  static constexpr unsigned nof_bits_per_simd = 4;

  // Calculate maximum number of SIMD words that can be generated in parallel.
  static constexpr unsigned max_nof_word_step =
      pseudo_random_generator_sequence::get_max_step_size() / nof_bits_per_simd;

  // Calculate the number of bits for sequence step.
  static constexpr unsigned max_nof_bits_step = max_nof_word_step * nof_bits_per_simd;

  // Create sequence with the current x1 and x2 states.
  pseudo_random_generator_sequence sequence(x1, x2);

  // Process in batches of max_nof_bits_step bits.
  for (unsigned i_end = (length / max_nof_bits_step) * max_nof_bits_step; i != i_end; i += max_nof_bits_step) {
    uint32_t c = sequence.step<max_nof_bits_step>();

    uint32_t j = 0;
#ifdef __SSE3__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 4 MSB.
      __m128i mask = _mm_set1_epi32(c << j);

      // Masks each bit.
      mask = _mm_and_si128(mask, _mm_setr_epi32(0x80000000, 0x40000000, 0x20000000, 0x10000000));

      // Get non zero mask.
      __m128i offset        = _mm_set1_epi32(0x80000000);
      mask                  = _mm_add_epi32(mask, offset);
      __m128i adjusted_zero = _mm_add_epi32(_mm_set1_epi32(0), offset);
      mask                  = _mm_cmpgt_epi32(mask, adjusted_zero);

      // And with MSB.
      mask = _mm_and_si128(mask, (__m128i)_mm_set1_ps(-0.0F));

      // Load input.
      __m128 v = _mm_set1_ps(value);

      // Loads input and perform sign XOR.
      v = _mm_xor_ps((__m128)mask, v);

      _mm_storeu_ps(&out[i + j], v);
    }
#endif // __SSE3__
#ifdef __aarch64__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 4 MSB.
      uint32x4_t mask_u32 = vdupq_n_u32(c << j);

      // Masks each bit.
      const uint32x4_t tmp_mask = vcombine_u32(vcreate_u32(0x4000000080000000UL), vcreate_u32(0x1000000020000000UL));

      mask_u32 = vandq_u32(mask_u32, tmp_mask);

      // Get non zero mask.
      mask_u32 = vcgtq_u32(mask_u32, vdupq_n_u32(0));

      // And with MSB.
      mask_u32 = vandq_u32(mask_u32, vreinterpretq_u32_f32(vdupq_n_f32(-0.0F)));

      // Load input.
      float32x4_t v = vdupq_n_f32(value);

      // Loads input and perform sign XOR.
      v = vreinterpretq_f32_u32(veorq_u32(mask_u32, vreinterpretq_u32_f32(v)));

      vst1q_f32(&out[i + j], v);
    }
#endif // __aarch64__

    // Finish the parallel bits with generic code.
    for (; j != max_nof_bits_step; ++j) {
      FLOAT_U32_XOR(out[i + j], value, (c << j) & 0x80000000);
    }
  }

  unsigned remainder = length - i;
  uint32_t c         = sequence.step(remainder);
  for (; i != length; ++i) {
    FLOAT_U32_XOR(out[i], value, c & 0x80000000);

    c = c << 1U;
  }

  // Update x1 and x2 states.
  x1 = sequence.get_x1();
  x2 = sequence.get_x2();
}

void pseudo_random_generator_impl::apply_xor(bit_buffer& out, const bit_buffer& in)
{
  srsran_assert(in.size() == out.size(),
                "Input size (i.e., {}) and output size (i.e., {}) must be equal.",
                in.size(),
                out.size());

  // Number of bits per processing word.
  static constexpr unsigned nof_bits_per_byte = 8;

  // Calculate the maximum number of simultaneous words to process.
  static constexpr unsigned max_nof_bytes_step = 64 / nof_bits_per_byte;

  // Calculate the maximum number of simultaneous bits to process.
  static constexpr unsigned max_nof_bits_step = max_nof_bytes_step * nof_bits_per_byte;

  // Create sequence with the current x1 and x2 states.
  pseudo_random_generator_sequence sequence(x1, x2);

  // Processes batches of words.
  for (unsigned i_byte = 0, i_byte_end = (in.size() / max_nof_bits_step) * max_nof_bytes_step; i_byte != i_byte_end;
       i_byte += 8) {
    uint64_t c = sequence.step64();
    c          = __builtin_bswap64(c);
    uint64_t temp;
    memcpy(&temp, &in.get_buffer()[i_byte], 8);
    temp ^= c;
    memcpy(&out.get_buffer()[i_byte], &temp, 8);
  }

  // Process spare bits in a batch of the remainder bits.
  unsigned i_bit     = (in.size() / max_nof_bits_step) * max_nof_bits_step;
  unsigned remainder = in.size() - i_bit;
  unsigned count     = 0;
  while (remainder != 0) {
    unsigned batch_size    = std::min(remainder, pseudo_random_generator_sequence::get_max_step_size());
    unsigned reminder_size = batch_size;
    uint32_t c             = sequence.step(batch_size);

    // Process bits.
    while (batch_size != 0) {
      // Process per byte basis, ceiling at the remainder.
      unsigned word_size = std::min(batch_size, nof_bits_per_byte);

      uint8_t input_word = in.extract(i_bit, word_size);

      // Shift to align the reversed sequence LSB with the remainder bits.
      unsigned right_shift = nof_bits_per_byte - word_size;

      // Calculate the output byte.
      uint8_t output_word =
          input_word ^ ((static_cast<uint8_t>((c >> (24u - (count * nof_bits_per_byte))) & 0xff)) >> right_shift);

      // Insert the output byte.
      out.insert(output_word, i_bit, word_size);

      // Advance bit index.
      i_bit += word_size;

      // Decrement remainder.
      batch_size -= word_size;
      ++count;
    }
    remainder -= reminder_size;
  }
  // Update x1 and x2 states.
  x1 = sequence.get_x1();
  x2 = sequence.get_x2();
}

void pseudo_random_generator_impl::apply_xor(span<uint8_t> out, span<const uint8_t> in)
{
  srsran_assert(in.size() == out.size(),
                "Input size (i.e., {}) and output size (i.e., {}) must be equal.",
                in.size(),
                out.size());

  // Number of bits that can be processed with a SIMD register.
  static constexpr unsigned nof_bits_per_simd = 16;

  // Calculate maximum number of SIMD words that can be generated in parallel.
  static constexpr unsigned max_nof_word_step =
      pseudo_random_generator_sequence::get_max_step_size() / nof_bits_per_simd;

  // Calculate the number of bits for sequence step.
  static constexpr unsigned max_nof_bits_step = max_nof_word_step * nof_bits_per_simd;

  // Create sequence with the current x1 and x2 states.
  pseudo_random_generator_sequence sequence(x1, x2);

  unsigned i      = 0;
  unsigned length = in.size();

  for (unsigned i_end = (length / max_nof_bits_step) * max_nof_bits_step; i != i_end; i += max_nof_bits_step) {
    uint32_t c = sequence.step<max_nof_bits_step>();

    unsigned j = 0;
#ifdef __SSE3__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 16 LSB
      __m128i mask = _mm_set1_epi32(c);
      mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2));

      // Masks each bit.
      mask = _mm_and_si128(mask, _mm_set_epi64x(0x102040810204080, 0x102040810204080));

      // Get non zero mask.
      mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x102040810204080, 0x102040810204080));

      // Reduce to 1s and 0s.
      mask = _mm_and_si128(mask, _mm_set1_epi8(1));

      // Load input.
      __m128i v = _mm_loadu_si128((__m128i*)(&in[i + j]));

      // Apply XOR.
      v = _mm_xor_si128(mask, v);

      // Store output.
      _mm_storeu_si128((__m128i*)(&out[i + j]), v);
    }
#endif // __SSE3__
#ifdef __aarch64__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 16 LSB.
      uint32x2_t c_dup_u32 = vdup_n_u32(c);
      uint8x16_t mask_u8 =
          vcombine_u8(vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 3), vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 2));

      // Create bit masks.
      const uint8_t    bit_masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
      const uint8x16_t bit_masks_u8 = vcombine_u8(vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))),
                                                  vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))));
      // Mask each bit.
      mask_u8 = vandq_u8(mask_u8, bit_masks_u8);

      // Get non zero mask.
      mask_u8 = vceqq_u8(mask_u8, bit_masks_u8);

      // Reduce to 1s and 0s.
      mask_u8 = vandq_u8(mask_u8, vdupq_n_u8(1));

      // Load input.
      uint8x16_t v = vld1q_u8(&in[i + j]);

      // Apply XOR.
      v = veorq_u8(mask_u8, v);

      // Store output.
      vst1q_u8(&out[i + j], v);
    }
#endif // __aarch64__

    // Apply mask to remainder bits.
    for (; j != max_nof_bits_step; ++j) {
      unsigned bit = (c & (1U << 31U)) != 0;
      out[i + j]   = in[i + j] ^ bit;
      c            = c << 1U;
    }
  }

  // Apply remainder bits.
  unsigned remainder = length - i;
  uint32_t c         = sequence.step(remainder);
  for (; i != length; ++i) {
    unsigned bit = (c & (1U << 31U)) != 0;
    out[i]       = in[i] ^ bit;
    c            = c << 1U;
  }

  // Update x1 and x2 states.
  x1 = sequence.get_x1();
  x2 = sequence.get_x2();
}

void pseudo_random_generator_impl::apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in)
{
  srsran_assert(in.size() == out.size(),
                "Input size (i.e., {}) and output size (i.e., {}) must be equal.",
                in.size(),
                out.size());

  unsigned i      = 0;
  unsigned length = in.size();

  // Number of bits that can be processed with a SIMD register.
  static constexpr unsigned nof_bits_per_simd = 16;

  // Calculate maximum number of SIMD words that can be generated in parallel.
  static constexpr unsigned max_nof_word_step =
      pseudo_random_generator_sequence::get_max_step_size() / nof_bits_per_simd;

  // Calculate the number of bits for sequence step.
  static constexpr unsigned max_nof_bits_step = max_nof_word_step * nof_bits_per_simd;

  // Create sequence with the current x1 and x2 states.
  pseudo_random_generator_sequence sequence(x1, x2);

  for (unsigned i_end = (length / max_nof_bits_step) * max_nof_bits_step; i != i_end; i += max_nof_bits_step) {
    uint32_t c = sequence.step<max_nof_bits_step>();

    unsigned j = 0;
#ifdef __SSE3__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 16 LSB.
      __m128i mask = _mm_set1_epi32(c);
      mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2));

      // Masks each bit.
      mask = _mm_and_si128(mask, _mm_set_epi64x(0x102040810204080, 0x102040810204080));

      // Get non zero mask.
      mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x102040810204080, 0x102040810204080));

      // Load input.
      __m128i v = _mm_loadu_si128((__m128i*)(&in[i + j]));

      // Negate.
      v = _mm_xor_si128(mask, v);

      // Add one.
      mask = _mm_and_si128(mask, _mm_set1_epi8(1));
      v    = _mm_add_epi8(v, mask);

      _mm_storeu_si128((__m128i*)(&out[i + j]), v);
    }
#endif // __SSE3__
#ifdef __aarch64__
    for (; j != max_nof_bits_step; j += nof_bits_per_simd) {
      // Preloads bits of interest in the 16 LSB.
      uint32x2_t c_dup_u32 = vdup_n_u32(c);
      uint8x16_t mask_u8 =
          vcombine_u8(vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 3), vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 2));

      // Create bit masks.
      const uint8_t    bit_masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
      const uint8x16_t bit_masks_u8 = vcombine_u8(vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))),
                                                  vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))));
      // Mask each bit.
      mask_u8 = vandq_u8(mask_u8, bit_masks_u8);

      // Get non zero mask.
      mask_u8 = vceqq_u8(mask_u8, bit_masks_u8);

      // Load input.
      int8x16_t v = vld1q_s8(reinterpret_cast<const int8_t*>(&in[i + j]));

      // Negate.
      v = veorq_s8(vreinterpretq_s8_u8(mask_u8), v);

      // Add one.
      int8x16_t one_s8 = vandq_s8(vreinterpretq_s8_u8(mask_u8), vdupq_n_s8(1));
      v                = vaddq_s8(v, one_s8);

      // Store the result.
      vst1q_s8(reinterpret_cast<int8_t*>(&out[i + j]), v);
    }
#endif // __aarch64__
    for (; j != max_nof_bits_step; ++j) {
      out[i + j] = in[i + j].to_value_type() * ((c << j & 0x80000000) ? -1 : +1);
    }
  }

  // Apply remainder bits.
  unsigned remainder = length - i;
  uint32_t c         = sequence.step(remainder);
  for (; i != length; ++i) {
    out[i] = in[i].to_value_type() * (c & 0x80000000 ? -1 : 1);

    c = c << 1;
  }

  // Update x1 and x2 states.
  x1 = sequence.get_x1();
  x2 = sequence.get_x2();
}

pseudo_random_generator::state_s pseudo_random_generator_impl::get_state() const
{
  return {x1, x2};
}
