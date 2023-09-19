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

#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/srsran_test.h"
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

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

static constexpr float ASSERT_MAX_FLOAT_ERROR = 1e-6;

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
  // Initialize states
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

static void test_apply_xor_packed(std::shared_ptr<pseudo_random_generator_factory> factory,
                                  unsigned                                         c_init,
                                  unsigned                                         N,
                                  unsigned                                         offset)
{
  // Create data buffer.
  dynamic_bit_buffer data(N);

  // Fill buffer with random data.
  for (unsigned i_byte = 0, i_byte_end = N / 8; i_byte != i_byte_end; ++i_byte) {
    data.set_byte(rgen() & mask_lsb_ones<unsigned>(8), i_byte);
  }
  if (N % 8 != 0) {
    data.insert((rgen() & mask_lsb_ones<unsigned>(N % 8)), 8 * (N / 8), N % 8);
  }

  // Create sequence generator.
  std::unique_ptr<pseudo_random_generator> generator = factory->create();
  TESTASSERT(generator);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  dynamic_bit_buffer data_xor(N);
  generator->apply_xor(data_xor, data);

  // Assert.
  for (unsigned i = 0; i != N / 8; ++i) {
    uint8_t gold = c_packed[i] ^ data.extract(8 * i, 8);
    TESTASSERT_EQ(gold, data_xor.extract(8 * i, 8));
  }
}

static void test_apply_xor_unpacked(std::shared_ptr<pseudo_random_generator_factory> factory,
                                    unsigned                                         c_init,
                                    unsigned                                         N,
                                    unsigned                                         offset)
{
  std::uniform_int_distribution<unsigned char> dist(0, 1);

  // Create data buffer.
  srsvec::aligned_vec<uint8_t> data(N);

  // Fill buffer with random data.
  for (unsigned char& v : data) {
    v = dist(rgen);
  }

  // Create sequence generator.
  std::unique_ptr<pseudo_random_generator> generator = factory->create();
  TESTASSERT(generator);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  srsvec::aligned_vec<uint8_t> data_xor(N);
  generator->apply_xor(data_xor, data);

  // Assert.
  for (unsigned i = 0; i != N; ++i) {
    uint8_t gold = c_unpacked[i] ^ data[i];
    TESTASSERT_EQ(gold, data_xor[i]);
  }
}

static void test_apply_xor_i8(std::shared_ptr<pseudo_random_generator_factory> factory,
                              unsigned                                         c_init,
                              unsigned                                         N,
                              unsigned                                         offset)
{
  std::uniform_int_distribution<int8_t> dist(log_likelihood_ratio::min().to_value_type(),
                                             log_likelihood_ratio::max().to_value_type());

  // Create data buffer.
  srsvec::aligned_vec<log_likelihood_ratio> data(N);

  // Fill buffer with random data.
  for (log_likelihood_ratio& v : data) {
    v = dist(rgen);
  }

  // Create sequence generator.
  std::unique_ptr<pseudo_random_generator> generator = factory->create();
  TESTASSERT(generator);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  srsvec::aligned_vec<log_likelihood_ratio> data_xor(N);
  generator->apply_xor(data_xor, data);

  // Assert.
  for (unsigned i = 0; i != N; ++i) {
    log_likelihood_ratio gold = c_char[i] * data[i].to_value_type();
    TESTASSERT_EQ(gold, data_xor[i]);
  }
}

static void test_generate_bit(std::shared_ptr<pseudo_random_generator_factory> factory,
                              unsigned                                         c_init,
                              unsigned                                         N,
                              unsigned                                         offset)
{
  // Create data buffer.
  dynamic_bit_buffer sequence(N);

  // Create sequence generator.
  std::unique_ptr<pseudo_random_generator> generator = factory->create();
  TESTASSERT(generator);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Generate sequence.
  generator->generate(sequence);

  // Assert.
  for (unsigned i_byte = 0, i_byte_end = N / 8; i_byte != i_byte_end; ++i_byte) {
    uint8_t gold = c_packed[i_byte];
    TESTASSERT_EQ(gold, sequence.get_byte(i_byte));
  }

  for (unsigned i_bit = (N / 8) * 8; i_bit != N; ++i_bit) {
    uint8_t gold = c_unpacked[i_bit];
    TESTASSERT_EQ(gold, sequence.extract(i_bit, 1));
  }
}

static void test_generate_float(std::shared_ptr<pseudo_random_generator_factory> factory,
                                unsigned                                         c_init,
                                unsigned                                         N,
                                unsigned                                         offset)
{
  // Create data buffer.
  srsvec::aligned_vec<float> sequence(N);

  // Create sequence generator.
  std::unique_ptr<pseudo_random_generator> generator = factory->create();
  TESTASSERT(generator);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  generator->generate(sequence, M_SQRT1_2);

  // Assert.
  for (unsigned i = 0; i != N; ++i) {
    float gold = c_float[i] * M_SQRT1_2;
    float err  = std::abs(gold - sequence[i]);
    TESTASSERT(err < ASSERT_MAX_FLOAT_ERROR, "Failed err={}.", err);
  }
}

int main()
{
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::vector<unsigned>                   offsets = {0, 2, 3, 5, 7};
  std::vector<unsigned>                   sizes   = {256, 512, 768};
  std::uniform_int_distribution<unsigned> dist(0, INT32_MAX);

  for (unsigned N : sizes) {
    for (unsigned offset : offsets) {
      unsigned c_init = dist(rgen);

      // Generate gold sequence.
      generate_gold(c_init, N, offset);

      // Test sequence XOR with byte buffer.
      test_apply_xor_packed(prg_factory, c_init, N, offset);

      // Test sequence XOR with bit buffer.
      test_apply_xor_unpacked(prg_factory, c_init, N, offset);

      // Test sequence XOR with 8-bit signed buffer.
      test_apply_xor_i8(prg_factory, c_init, N, offset);

      // Test sequence generation in packed bits.
      test_generate_bit(prg_factory, c_init, N, offset);

      // Test sequence generation in float.
      test_generate_float(prg_factory, c_init, N, offset);
    }
  }
}