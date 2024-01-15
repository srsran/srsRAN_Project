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

#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/srsvec/aligned_vec.h"
#include "srsran/srsvec/bit.h"
#include <gtest/gtest.h>
#include <random>

static std::mt19937 rgen(0);

using namespace srsran;

#define Nc 1600

static constexpr float ASSERT_MAX_FLOAT_ERROR = 1e-6;

namespace {

using PseudoRandomGeneratorParams = std::tuple<unsigned, unsigned>;
std::uniform_int_distribution<unsigned> dist(0, INT32_MAX);

class PseudoRandomGeneratorFixture : public ::testing::TestWithParam<PseudoRandomGeneratorParams>
{
protected:
  static std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  unsigned                                                offset;
  unsigned                                                size;
  unsigned                                                c_init;
  static std::unique_ptr<pseudo_random_generator>         generator;

  static void SetUpTestSuite()
  {
    // Create pseudo random generator factory.
    if (!prg_factory) {
      prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr) << "Cannot create factory";
    }
    if (!generator) {
      // Create sequence generator.
      generator = prg_factory->create();
      ASSERT_NE(generator, nullptr) << "Cannot create sequence generator";
    }
  }

  void SetUp() override
  {
    ASSERT_NE(prg_factory, nullptr) << "Cannot create pseudo random generator factory";
    ASSERT_NE(generator, nullptr) << "Cannot create pseudo random generator";

    // Get test parameters.
    auto params = GetParam();
    offset      = std::get<0>(params);
    size        = std::get<1>(params);

    c_init = dist(rgen);

    c.resize(size);
    c_float.resize(size);
    c_char.resize(size);
    c_packed.resize(size);
    x1.resize(Nc + 31 + size);
    x2.resize(Nc + 31 + size);

    // Generate gold sequence.
    generate_gold();
  }

  span<const uint8_t> get_gold() const { return c; }
  span<const float>   get_gold_float() const { return c_float; }
  span<const int8_t>  get_gold_char() const { return c_char; }
  const bit_buffer&   get_gold_packed() const { return c_packed; }

private:
  dynamic_bit_buffer   c_packed;
  std::vector<uint8_t> x1;
  std::vector<uint8_t> x2;
  std::vector<uint8_t> c;
  std::vector<float>   c_float;
  std::vector<int8_t>  c_char;

  void generate_gold()
  {
    // Initialize states
    for (uint32_t n = 0; n < 31; n++) {
      x2[n] = (c_init >> n) & 0x1;
    }
    x1[0] = 1;

    // reach initial states
    for (uint32_t n = 0; n < Nc + size; n++) {
      x1[n + 31] = (x1[n + 3] + x1[n]) & 0x1;
      x2[n + 31] = (x2[n + 3] + x2[n + 2] + x2[n + 1] + x2[n]) & 0x1;
    }

    // Create each sequence
    for (uint32_t n = 0; n < size; n++) {
      c[n]       = (x1[n + Nc + offset] + x2[n + Nc + offset]) & 0x1;
      c_float[n] = c[n] ? -1.0f : +1.0f;
      c_char[n]  = c[n] ? -1 : +1;
    }

    srsvec::bit_pack(c_packed, c);
  }
};

std::shared_ptr<pseudo_random_generator_factory> PseudoRandomGeneratorFixture::prg_factory = nullptr;
std::unique_ptr<pseudo_random_generator>         PseudoRandomGeneratorFixture::generator   = nullptr;

TEST_P(PseudoRandomGeneratorFixture, PseudoRandomGeneratorXorPacked)
{
  // Create data buffer.
  dynamic_bit_buffer data(size);

  // Fill buffer with random data.
  for (unsigned i_byte = 0, i_byte_end = size / 8; i_byte != i_byte_end; ++i_byte) {
    data.set_byte(rgen() & mask_lsb_ones<unsigned>(8), i_byte);
  }
  if (size % 8 != 0) {
    data.insert((rgen() & mask_lsb_ones<unsigned>(size % 8)), 8 * (size / 8), size % 8);
  }

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  dynamic_bit_buffer data_xor(size);
  generator->apply_xor(data_xor, data);

  // Assert.
  const bit_buffer& gold_packed = get_gold_packed();
  for (unsigned i = 0; i != size / 8; ++i) {
    uint8_t gold = gold_packed.get_byte(i) ^ data.get_byte(i);
    ASSERT_EQ(gold, data_xor.get_byte(i));
  }
}

TEST_P(PseudoRandomGeneratorFixture, PseudoRandomGeneratorXorUnpacked)
{
  std::uniform_int_distribution<unsigned char> distXorUnpacked(0, 1);

  // Create data buffer.
  srsvec::aligned_vec<uint8_t> data(size);

  // Fill buffer with random data.
  for (unsigned char& v : data) {
    v = distXorUnpacked(rgen);
  }

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  srsvec::aligned_vec<uint8_t> data_xor(size);
  generator->apply_xor(data_xor, data);

  // Assert.
  span<const uint8_t> gold = get_gold();
  for (unsigned i = 0; i != size; ++i) {
    ASSERT_EQ(gold[i] ^ data[i], data_xor[i]);
  }
}

TEST_P(PseudoRandomGeneratorFixture, PseudoRandomGeneratorXori8)
{
  std::uniform_int_distribution<int8_t> distXori8(log_likelihood_ratio::min().to_value_type(),
                                                  log_likelihood_ratio::max().to_value_type());

  // Create data buffer.
  srsvec::aligned_vec<log_likelihood_ratio> data(size);

  // Fill buffer with random data.
  for (log_likelihood_ratio& v : data) {
    v = distXori8(rgen);
  }

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  srsvec::aligned_vec<log_likelihood_ratio> data_xor(size);
  generator->apply_xor(data_xor, data);

  // Assert.
  span<const int8_t> gold_char = get_gold_char();
  for (unsigned i = 0; i != size; ++i) {
    log_likelihood_ratio gold = gold_char[i] * data[i].to_value_type();
    ASSERT_EQ(gold, data_xor[i]);
  }
}

TEST_P(PseudoRandomGeneratorFixture, PseudoRandomGeneratorBit)
{
  // Create data buffer.
  dynamic_bit_buffer sequence(size);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Generate sequence.
  generator->generate(sequence);

  // Assert.
  const bit_buffer& gold_packed = get_gold_packed();
  for (unsigned i_byte = 0, i_byte_end = size / 8; i_byte != i_byte_end; ++i_byte) {
    ASSERT_EQ(gold_packed.get_byte(i_byte), sequence.get_byte(i_byte));
  }

  span<const uint8_t> gold = get_gold();
  for (unsigned i_bit = (size / 8) * 8; i_bit != size; ++i_bit) {
    ASSERT_EQ(gold[i_bit], sequence.extract(i_bit, 1));
  }
}

TEST_P(PseudoRandomGeneratorFixture, PseudoRandomGeneratorFloat)
{
  // Create data buffer.
  srsvec::aligned_vec<float> sequence(size);

  // Initialize sequence generator.
  generator->init(c_init);

  // Advance.
  generator->advance(offset);

  // Apply sequence.
  generator->generate(sequence, M_SQRT1_2);

  // Assert.
  span<const float> gold_float = get_gold_float();
  for (unsigned i = 0; i != size; ++i) {
    float gold = gold_float[i] * M_SQRT1_2;
    float err  = std::abs(gold - sequence[i]);
    ASSERT_TRUE(err < ASSERT_MAX_FLOAT_ERROR) << ::fmt::format("Failed err={}.", err);
  }
}

INSTANTIATE_TEST_SUITE_P(PseudoRandomGeneratorTest,
                         PseudoRandomGeneratorFixture,
                         ::testing::Combine(::testing::Values(0, 2, 3, 5, 7), ::testing::Values(255, 512, 768)));

} // end namespace