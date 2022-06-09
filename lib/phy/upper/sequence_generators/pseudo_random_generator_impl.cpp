/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pseudo_random_generator_impl.h"
#include <cassert>
#include <cstring>

using namespace srsgnb;

const pseudo_random_generator_impl::x1_init_s pseudo_random_generator_impl::x1_init =
    pseudo_random_generator_impl::x1_init_s();

const pseudo_random_generator_impl::x2_init_s pseudo_random_generator_impl::x2_init =
    pseudo_random_generator_impl::x2_init_s();

pseudo_random_generator_impl::x1_init_s::x1_init_s() : x1(1)
{
  // Compute transition step
  for (uint32_t n = 0; n < SEQUENCE_NC; n++) {
    x1 = step_x1(x1);
  }
}

unsigned pseudo_random_generator_impl::x1_init_s::get() const
{
  return x1;
}

pseudo_random_generator_impl::x2_init_s::x2_init_s() : x2()
{
  // For each bit of the seed
  for (uint32_t i = 0; i < SEQUENCE_SEED_LEN; ++i) {
    // Compute transition step
    x2[i] = 1U << i;
    for (uint32_t n = 0; n < SEQUENCE_NC; ++n) {
      x2[i] = step_x2(x2[i]);
    }
  }
}

unsigned pseudo_random_generator_impl::x2_init_s::get(unsigned c_init) const
{
  unsigned ret = 0;

  for (unsigned i = 0; i < SEQUENCE_SEED_LEN; ++i) {
    if ((c_init >> i) & 1UL) {
      ret ^= x2[i];
    }
  }

  return ret;
}

void pseudo_random_generator_impl::init(unsigned c_init)
{
  x1 = x1_init.get();
  x2 = x2_init.get(c_init);
}

void pseudo_random_generator_impl::init(const state_s& state)
{
  x1 = state.x1;
  x2 = state.x2;
}

void pseudo_random_generator_impl::advance(unsigned count)
{
  unsigned i = 0;

  // Advance in parallel steps
  if (count >= SEQUENCE_PAR_BITS) {
    for (; i < count - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);
    }
  }

  // Advance in single steps
  for (; i < count; i++) {
    // Step sequences
    x1 = step_x1(x1);
    x2 = step_x2(x2);
  }
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

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef HAVE_SSE
      for (; j < SEQUENCE_PAR_BITS - 3; j += 4) {
        // Preloads bits of interest in the 4 LSB
        __m128i mask = _mm_set1_epi32(c >> j);

        // Masks each bit
        mask = _mm_and_si128(mask, _mm_setr_epi32(1, 2, 4, 8));

        // Get non zero mask
        mask = _mm_cmpgt_epi32(mask, _mm_set1_epi32(0));

        // And with MSB
        mask = _mm_and_si128(mask, (__m128i)_mm_set1_ps(-0.0F));

        // Load input
        __m128 v = _mm_set1_ps(value);

        // Loads input and perform sign XOR
        v = _mm_xor_ps((__m128)mask, v);

        _mm_storeu_ps(out + i + j, v);
      }
#endif
      // Finish the parallel bits with generic code
      for (; j < SEQUENCE_PAR_BITS; j++) {
        FLOAT_U32_XOR(out[i + j], value, (c << (31U - j)) & 0x80000000);
      }

      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    FLOAT_U32_XOR(out[i], value, (x1 ^ x2) << 31U);

    // Step sequences
    x1 = step_x1(x1);
    x2 = step_x2(x2);
  }
}
void pseudo_random_generator_impl::apply_xor_byte(span<const uint8_t> in, span<uint8_t> out)
{
  assert(in.size() == out.size());

  unsigned i      = 0;
  unsigned length = in.size() * 8;

  static const std::array<uint8_t, 256> reverse_lut = {
      0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000, 0b00010000,
      0b10010000, 0b01010000, 0b11010000, 0b00110000, 0b10110000, 0b01110000, 0b11110000, 0b00001000, 0b10001000,
      0b01001000, 0b11001000, 0b00101000, 0b10101000, 0b01101000, 0b11101000, 0b00011000, 0b10011000, 0b01011000,
      0b11011000, 0b00111000, 0b10111000, 0b01111000, 0b11111000, 0b00000100, 0b10000100, 0b01000100, 0b11000100,
      0b00100100, 0b10100100, 0b01100100, 0b11100100, 0b00010100, 0b10010100, 0b01010100, 0b11010100, 0b00110100,
      0b10110100, 0b01110100, 0b11110100, 0b00001100, 0b10001100, 0b01001100, 0b11001100, 0b00101100, 0b10101100,
      0b01101100, 0b11101100, 0b00011100, 0b10011100, 0b01011100, 0b11011100, 0b00111100, 0b10111100, 0b01111100,
      0b11111100, 0b00000010, 0b10000010, 0b01000010, 0b11000010, 0b00100010, 0b10100010, 0b01100010, 0b11100010,
      0b00010010, 0b10010010, 0b01010010, 0b11010010, 0b00110010, 0b10110010, 0b01110010, 0b11110010, 0b00001010,
      0b10001010, 0b01001010, 0b11001010, 0b00101010, 0b10101010, 0b01101010, 0b11101010, 0b00011010, 0b10011010,
      0b01011010, 0b11011010, 0b00111010, 0b10111010, 0b01111010, 0b11111010, 0b00000110, 0b10000110, 0b01000110,
      0b11000110, 0b00100110, 0b10100110, 0b01100110, 0b11100110, 0b00010110, 0b10010110, 0b01010110, 0b11010110,
      0b00110110, 0b10110110, 0b01110110, 0b11110110, 0b00001110, 0b10001110, 0b01001110, 0b11001110, 0b00101110,
      0b10101110, 0b01101110, 0b11101110, 0b00011110, 0b10011110, 0b01011110, 0b11011110, 0b00111110, 0b10111110,
      0b01111110, 0b11111110, 0b00000001, 0b10000001, 0b01000001, 0b11000001, 0b00100001, 0b10100001, 0b01100001,
      0b11100001, 0b00010001, 0b10010001, 0b01010001, 0b11010001, 0b00110001, 0b10110001, 0b01110001, 0b11110001,
      0b00001001, 0b10001001, 0b01001001, 0b11001001, 0b00101001, 0b10101001, 0b01101001, 0b11101001, 0b00011001,
      0b10011001, 0b01011001, 0b11011001, 0b00111001, 0b10111001, 0b01111001, 0b11111001, 0b00000101, 0b10000101,
      0b01000101, 0b11000101, 0b00100101, 0b10100101, 0b01100101, 0b11100101, 0b00010101, 0b10010101, 0b01010101,
      0b11010101, 0b00110101, 0b10110101, 0b01110101, 0b11110101, 0b00001101, 0b10001101, 0b01001101, 0b11001101,
      0b00101101, 0b10101101, 0b01101101, 0b11101101, 0b00011101, 0b10011101, 0b01011101, 0b11011101, 0b00111101,
      0b10111101, 0b01111101, 0b11111101, 0b00000011, 0b10000011, 0b01000011, 0b11000011, 0b00100011, 0b10100011,
      0b01100011, 0b11100011, 0b00010011, 0b10010011, 0b01010011, 0b11010011, 0b00110011, 0b10110011, 0b01110011,
      0b11110011, 0b00001011, 0b10001011, 0b01001011, 0b11001011, 0b00101011, 0b10101011, 0b01101011, 0b11101011,
      0b00011011, 0b10011011, 0b01011011, 0b11011011, 0b00111011, 0b10111011, 0b01111011, 0b11111011, 0b00000111,
      0b10000111, 0b01000111, 0b11000111, 0b00100111, 0b10100111, 0b01100111, 0b11100111, 0b00010111, 0b10010111,
      0b01010111, 0b11010111, 0b00110111, 0b10110111, 0b01110111, 0b11110111, 0b00001111, 0b10001111, 0b01001111,
      0b11001111, 0b00101111, 0b10101111, 0b01101111, 0b11101111, 0b00011111, 0b10011111, 0b01011111, 0b11011111,
      0b00111111, 0b10111111, 0b01111111, 0b11111111,
  };

#if SEQUENCE_PAR_BITS % 8 != 0
  uint64_t buffer = 0;
  uint32_t count  = 0;

  for (; i < length / 8; i++) {
    // Generate sequence bits
    while (count < 8) {
      uint32_t c = (uint32_t)(x1 ^ x2);
      buffer     = buffer | ((SEQUENCE_MASK & c) << count);

      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);

      // Increase count
      count += SEQUENCE_PAR_BITS;
    }

    // Apply XOR
    out[i] = in[i] ^ reverse_lut[buffer & 255UL];
    buffer = buffer >> 8UL;
    count -= 8;
  }

  // Process spare bits
  uint32_t rem8 = length % 8;
  if (rem8 != 0) {
    // Generate sequence bits
    while (count < rem8) {
      uint32_t c = (uint32_t)(x1 ^ x2);
      buffer     = buffer | ((SEQUENCE_MASK & c) << count);

      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);

      // Increase count
      count += SEQUENCE_PAR_BITS;
    }

    out[i] = in[i] ^ reverse_lut[buffer & ((1U << rem8) - 1U) & 255U];
  }
#else  // SEQUENCE_PAR_BITS % 8 == 0
  while (i < (length / 8 - (SEQUENCE_PAR_BITS - 1) / 8)) {
    uint32_t c = (uint32_t)(x1 ^ x2);

    for (uint32_t j = 0; j < SEQUENCE_PAR_BITS / 8; j++) {
      out[i] = in[i] ^ reverse_lut[c & 255U];
      c      = c >> 8U;
      i++;
    }

    // Step sequences
    x1 = step_par_x1(x1);
    x2 = step_par_x2(x2);
  }

  // Process spare bytes
  uint32_t c = (uint32_t)(x1 ^ x2);
  while (i < length / 8) {
    out[i] = in[i] ^ reverse_lut[c & 255U];
    c      = c >> 8U;
    i++;
  }

  // Process spare bits
  uint32_t rem8 = length % 8;
  if (rem8 != 0) {
    out[i] = in[i] ^ reverse_lut[c & ((1U << rem8) - 1U) & 255U];
  }
#endif // SEQUENCE_PAR_BITS % 8 == 0
}

void pseudo_random_generator_impl::apply_xor_bit(span<const uint8_t> in, span<uint8_t> out)
{
  assert(in.size() == out.size());

  unsigned i      = 0;
  unsigned length = in.size();

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef HAVE_SSE
      if (SEQUENCE_PAR_BITS >= 16) {
        // Preloads bits of interest in the 16 LSB
        __m128i mask = _mm_set1_epi32(c);
        mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1));

        // Masks each bit
        // mask = _mm_and_si128( mask, _mm_set_epi64x(0x0102040810204080, 0x0102040810204080));
        mask = _mm_and_si128(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Get non zero mask
        mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Reduce to 1s and 0s
        mask = _mm_and_si128(mask, _mm_set1_epi8(1));

        // Load input
        __m128i v = _mm_loadu_si128((__m128i*)(in + i + j));

        // Apply XOR
        v = _mm_xor_si128(mask, v);

        _mm_storeu_si128((__m128i*)(out + i + j), v);

        // Increment bit counter `j`
        j += 16;
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] ^ ((c >> j) & 1U);
      }

      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] ^ ((x1 ^ x2) & 1U);

    // Step sequences
    x1 = step_x1(x1);
    x2 = step_x2(x2);
  }
}
void pseudo_random_generator_impl::apply_xor(span<const int8_t> in, span<int8_t> out)
{
  assert(in.size() == out.size());

  unsigned i      = 0;
  unsigned length = in.size();

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef HAVE_SSE
      if (SEQUENCE_PAR_BITS >= 16) {
        // Preloads bits of interest in the 16 LSB
        __m128i mask = _mm_set1_epi32(c);
        mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1));

        // Masks each bit
        // mask = _mm_and_si128( mask, _mm_set_epi64x(0x0102040810204080, 0x0102040810204080));
        mask = _mm_and_si128(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Get non zero mask
        mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Load input
        __m128i v = _mm_loadu_si128((__m128i*)(in + i + j));

        // Negate
        v = _mm_xor_si128(mask, v);

        // Add one
        mask = _mm_and_si128(mask, _mm_set1_epi8(1));
        v    = _mm_add_epi8(v, mask);

        _mm_storeu_si128((__m128i*)(out + i + j), v);

        // Increment bit counter `j`
        j += 16;
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] * (((c >> j) & 1U) ? -1 : +1);
      }

      // Step sequences
      x1 = step_par_x1(x1);
      x2 = step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] * (((x1 ^ x2) & 1U) ? -1 : +1);

    // Step sequences
    x1 = step_x1(x1);
    x2 = step_x2(x2);
  }
}
pseudo_random_generator::state_s pseudo_random_generator_impl::get_state() const
{
  return {x1, x2};
}
