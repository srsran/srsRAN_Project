/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/srsran_assert.h"
#include <cassert>
#include <random>

static std::mt19937 rgen(0);

using namespace srsgnb;

#define Nc 1600
#define MAX_SEQ_LEN (256 * 1024)

static std::array<uint8_t, Nc + MAX_SEQ_LEN + 31> x1;
static std::array<uint8_t, Nc + MAX_SEQ_LEN + 31> x2;
static std::array<uint8_t, Nc + MAX_SEQ_LEN + 31> c;
static std::array<float, Nc + MAX_SEQ_LEN + 31>   c_float;
static std::array<int16_t, Nc + MAX_SEQ_LEN + 31> c_short;
static std::array<int8_t, Nc + MAX_SEQ_LEN + 31>  c_char;
static std::array<uint8_t, MAX_SEQ_LEN / 8>       c_packed;
static std::array<uint8_t, MAX_SEQ_LEN>           c_unpacked;

static uint32_t bit_pack(uint8_t** bits, int nof_bits)
{
  int      i;
  uint32_t value = 0;

  for (i = 0; i < nof_bits; i++) {
    value |= (uint32_t)(*bits)[i] << (nof_bits - i - 1);
  }
  *bits += nof_bits;
  return value;
}

static void pack_vector(uint8_t* unpacked, uint8_t* packed, int nof_bits)
{
  uint32_t i, nbytes;
  nbytes = nof_bits / 8;

  for (i = 0; i < nbytes; i++) {
    packed[i] = bit_pack(&unpacked, 8);
  }

  if (nof_bits % 8) {
    packed[i] = bit_pack(&unpacked, nof_bits % 8);
    packed[i] <<= 8 - (nof_bits % 8);
  }
}

static void generate_gold(unsigned c_init, unsigned length, unsigned offset)
{
  // Initialise states
  for (uint32_t n = 0; n < 31; n++) {
    x2[n] = (c_init >> n) & 0x1;
  }
  x1[0] = 1;

  // reach initial states
  for (uint32_t n = 0; n < Nc + length; n++) {
    x1[n + 31] = (x1[n + 3] + x1[n]) & 0x1;
    x2[n + 31] = (x2[n + 3] + x2[n + 2] + x2[n + 1] + x2[n]) & 0x1;
  }

  // Create each sequence
  for (uint32_t n = 0; n < length; n++) {
    c[n]          = (x1[n + Nc + offset] + x2[n + Nc + offset]) & 0x1;
    c_float[n]    = c[n] ? -1.0f : +1.0f;
    c_short[n]    = c[n] ? -1 : +1;
    c_char[n]     = c[n] ? -1 : +1;
    c_unpacked[n] = c[n];
  }

  pack_vector(c_unpacked.data(), c_packed.data(), length);
}

void test_apply_xor_byte(unsigned c_init, unsigned N, unsigned offset)
{
  std::uniform_int_distribution<unsigned char> dist(0, UINT8_MAX);

  // Create data buffer
  std::vector<uint8_t> data(N / 8);

  // Fill buffer with random data
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Create sequence generator
  std::unique_ptr<pseudo_random_generator> generator = create_pseudo_random();

  // Initialise sequence generator
  generator->init(c_init);

  // Advance
  generator->advance(offset);

  // Apply sequence
  std::vector<uint8_t> data_xor(N / 8);
  generator->apply_xor_byte(data, data_xor);

  // Assert
  for (unsigned i = 0; i != N / 8; ++i) {
    uint8_t gold = c_packed[i] ^ data[i];
    srsran_assert(gold == data_xor[i], "Failed");
  }
}

void test_apply_xor_bit(unsigned c_init, unsigned N, unsigned offset)
{
  std::uniform_int_distribution<unsigned char> dist(0, 1);

  // Create data buffer
  srsvec::aligned_vec<uint8_t> data(N);

  // Fill buffer with random data
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Create sequence generator
  std::unique_ptr<pseudo_random_generator> generator = create_pseudo_random();

  // Initialise sequence generator
  generator->init(c_init);

  // Advance
  generator->advance(offset);

  // Apply sequence
  srsvec::aligned_vec<uint8_t> data_xor(N);
  generator->apply_xor_bit(data, data_xor);

  // Assert
  for (unsigned i = 0; i != N; ++i) {
    uint8_t gold = c_unpacked[i] ^ data[i];
    srsran_assert(gold == data_xor[i], "Failed");
  }
}

void test_apply_xor_i8(unsigned c_init, unsigned N, unsigned offset)
{
  std::uniform_int_distribution<int8_t> dist(INT8_MIN, INT8_MAX);

  // Create data buffer
  srsvec::aligned_vec<int8_t> data(N);

  // Fill buffer with random data
  for (int8_t& v : data) {
    v = dist(rgen);
  }

  // Create sequence generator
  std::unique_ptr<pseudo_random_generator> generator = create_pseudo_random();

  // Initialise sequence generator
  generator->init(c_init);

  // Advance
  generator->advance(offset);

  // Apply sequence
  srsvec::aligned_vec<int8_t> data_xor(N);
  generator->apply_xor(data, data_xor);

  // Assert
  for (unsigned i = 0; i != N; ++i) {
    int8_t gold = c_char[i] * data[i];
    srsran_assert(gold == data_xor[i], "Failed");
  }
}

void test_generate_float(unsigned c_init, unsigned N, unsigned offset)
{
  // Create data buffer
  srsvec::aligned_vec<float> sequence(N);

  // Create sequence generator
  std::unique_ptr<pseudo_random_generator> generator = create_pseudo_random();

  // Initialise sequence generator
  generator->init(c_init);

  // Advance
  generator->advance(offset);

  // Apply sequence
  generator->generate(sequence, M_SQRT1_2);

  // Assert
  for (unsigned i = 0; i != N; ++i) {
    float gold = c_float[i] * M_SQRT1_2;
    float err  = std::abs(gold - sequence[i]);
    srsran_assert(err < 1e-6, "Failed err=%f", err);
  }
}

int main()
{
  std::vector<unsigned>                   offsets = {0, 2, 3, 5, 7};
  std::vector<unsigned>                   sizes   = {256, 512, 768};
  std::uniform_int_distribution<unsigned> dist(0, INT32_MAX);

  for (unsigned N : sizes) {
    for (unsigned offset : offsets) {
      unsigned c_init = dist(rgen);

      // Generate gold sequence
      generate_gold(c_init, N, offset);

      // Test sequence XOR with byte buffer
      test_apply_xor_byte(c_init, N, offset);

      // Test sequence XOR with bit buffer
      test_apply_xor_bit(c_init, N, offset);

      // Test sequence XOR with 8-bit signed buffer
      test_apply_xor_i8(c_init, N, offset);

      // Test sequence generation in float
      test_generate_float(c_init, N, offset);
    }
  }
}