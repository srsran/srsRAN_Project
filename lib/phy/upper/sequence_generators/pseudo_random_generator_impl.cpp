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
#include "srsgnb/support/math_utils.h"
#include <cassert>
#include <cstring>

#if HAVE_SSE
#include <immintrin.h>
#endif // HAVE_SSE

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

        _mm_storeu_ps(&out[i + j], v);
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
      out[i] = in[i] ^ reverse_byte(c & 255U);
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
    out[i] = in[i] ^ reverse_byte(c & 255U);
    c      = c >> 8U;
    i++;
  }

  // Process spare bits
  uint32_t rem8 = length % 8;
  if (rem8 != 0) {
    out[i] = in[i] ^ reverse_byte(c & ((1U << rem8) - 1U) & 255U);
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
        __m128i v = _mm_loadu_si128((__m128i*)(&in[i + j]));

        // Apply XOR
        v = _mm_xor_si128(mask, v);

        _mm_storeu_si128((__m128i*)(&out[i + j]), v);

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
        __m128i v = _mm_loadu_si128((__m128i*)(&in[i + j]));

        // Negate
        v = _mm_xor_si128(mask, v);

        // Add one
        mask = _mm_and_si128(mask, _mm_set1_epi8(1));
        v    = _mm_add_epi8(v, mask);

        _mm_storeu_si128((__m128i*)(&out[i + j]), v);

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
