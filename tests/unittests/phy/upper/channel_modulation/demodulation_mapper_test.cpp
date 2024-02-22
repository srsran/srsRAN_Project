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

/// \file
/// \brief Demodulation mapper unit test.
///
/// The test takes as input vectors containing noisy modulated symbols and the corresponding noise variances. The
/// symbols are demodulated and the resulting bits (both soft and hard versions) are compared with the expected values,
/// also provided by test vectors.

#include "demodulation_mapper_test_data.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  return os << fmt::format("{} modulation, {} symbols", to_string(tc.scheme), tc.nsymbols);
}

std::ostream& operator<<(std::ostream& os, span<const log_likelihood_ratio> data)
{
  return os << fmt::format("[{}]", data);
}

bool operator==(span<const log_likelihood_ratio> left, span<const log_likelihood_ratio> right)
{
  return std::equal(left.begin(), left.end(), right.begin(), right.end());
}

} // namespace srsran

namespace {

class DemodulatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }
    factory = create_channel_modulation_sw_factory();
    ASSERT_NE(factory, nullptr);
  }

  void SetUp() override
  {
    // Assert factory again for compatibility with GTest < 1.11.
    ASSERT_NE(factory, nullptr);

    // Create a demodulator.
    test_case_t test_case = GetParam();
    demodulator           = factory->create_demodulation_mapper();
    ASSERT_NE(demodulator, nullptr);

    // Read test input and output.
    mod = test_case.scheme;

    unsigned nof_symbols = test_case.nsymbols;
    symbols              = test_case.symbols.read();
    ASSERT_EQ(symbols.size(), nof_symbols) << "Error reading modulated symbols.";

    noise_var = test_case.noise_var.read();
    ASSERT_EQ(noise_var.size(), nof_symbols) << "Error reading noise variances.";

    ASSERT_TRUE(std::all_of(noise_var.cbegin(), noise_var.cend(), [](float f) { return f > 0; }))
        << "Noise variances should take positive values.";

    unsigned nof_bits  = nof_symbols * get_bits_per_symbol(mod);
    soft_bits_expected = test_case.soft_bits.read();
    ASSERT_EQ(soft_bits_expected.size(), nof_bits) << "Error reading soft bits.";
  }

  static std::shared_ptr<channel_modulation_factory> factory;
  std::unique_ptr<demodulation_mapper>               demodulator        = nullptr;
  std::vector<cf_t>                                  symbols            = {};
  std::vector<float>                                 noise_var          = {};
  std::vector<log_likelihood_ratio>                  soft_bits_expected = {};
  modulation_scheme                                  mod                = {};
};

std::shared_ptr<channel_modulation_factory> DemodulatorFixture::factory = nullptr;

TEST_P(DemodulatorFixture, DemodulatorTest)
{
  test_case_t test_case = GetParam();

  // Load expected results.
  unsigned nof_symbols = test_case.nsymbols;
  unsigned nof_bits    = nof_symbols * get_bits_per_symbol(mod);

  const std::vector<uint8_t> hard_bits_true = test_case.hard_bits.read();
  ASSERT_EQ(hard_bits_true.size(), nof_bits) << "Error reading hard bits.";

  // Run test.
  std::vector<log_likelihood_ratio> soft_bits(nof_bits);
  demodulator->demodulate_soft(soft_bits, symbols, noise_var, mod);

  ASSERT_EQ(span<const log_likelihood_ratio>(soft_bits_expected), span<const log_likelihood_ratio>(soft_bits))
      << fmt::format("Soft bits are not sufficiently precise (mod={}).", to_string(test_case.scheme));

  std::vector<uint8_t> hard_bits(nof_bits);
  std::transform(
      soft_bits.cbegin(), soft_bits.cend(), hard_bits.begin(), [](log_likelihood_ratio a) { return a.to_hard_bit(); });
  ASSERT_EQ(span<const uint8_t>(hard_bits), span<const uint8_t>(hard_bits_true)) << "Hard bits do not match.";
}

// Check that noise_var equal to zero implies LLR = 0.
TEST_P(DemodulatorFixture, DemodulatorNoiseZero)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned    nof_symbols = 18;
  span<float> noise_bad   = span<float>(noise_var).first(nof_symbols);
  // Set even-indexed noise variances to zero.
  for (unsigned i_symbol = 0; i_symbol < nof_symbols; i_symbol += 2) {
    noise_bad[i_symbol] = 0;
  }

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);
  demodulator->demodulate_soft(soft_bits, span<const cf_t>(symbols).first(nof_symbols), noise_bad, mod);

  bool are_zeros_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_zeros_ok = are_zeros_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                     return (a.to_int() == 0);
                   });
    local_soft   = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_zeros_ok) << "Division by zero went wrong.";
  ASSERT_TRUE(are_others_ok) << "Division by zero should not affect other soft bits.";
}

// Check that noise_var equal to infinity implies LLR = 0.
TEST_P(DemodulatorFixture, DemodulatorNoiseInfinity)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned    nof_symbols = 18;
  span<float> noise_bad   = span<float>(noise_var).first(nof_symbols);
  // Set even-indexed noise variances to infinity.
  for (unsigned i_symbol = 0; i_symbol < nof_symbols; i_symbol += 2) {
    noise_bad[i_symbol] = std::numeric_limits<float>::infinity();
  }

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);
  demodulator->demodulate_soft(soft_bits, span<const cf_t>(symbols).first(nof_symbols), noise_bad, mod);

  bool are_zeros_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_zeros_ok = are_zeros_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                     return (a.to_int() == 0);
                   });
    local_soft   = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_zeros_ok) << "Division by infinity went wrong.";
  ASSERT_TRUE(are_others_ok) << "Division by infinity should not affect other soft bits.";
}

// Check that a negative noise_var implies LLR = 0.
TEST_P(DemodulatorFixture, DemodulatorNoiseNegative)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned    nof_symbols = 18;
  span<float> noise_bad   = span<float>(noise_var).first(nof_symbols);
  // Set even-indexed noise variances to a negative value.
  for (unsigned i_symbol = 0; i_symbol < nof_symbols; i_symbol += 2) {
    noise_bad[i_symbol] = -2;
  }

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);
  demodulator->demodulate_soft(soft_bits, span<const cf_t>(symbols).first(nof_symbols), noise_bad, mod);

  bool are_zeros_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_zeros_ok = are_zeros_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                     return (a.to_int() == 0);
                   });
    local_soft   = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_zeros_ok) << "Dealing with negative variance went wrong.";
  ASSERT_TRUE(are_others_ok) << "Dealing with negative variance should not affect other soft bits.";
}

// Check that a NaN noise_var implies LLR = 0.
TEST_P(DemodulatorFixture, DemodulatorNoiseNaN)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned    nof_symbols = 18;
  span<float> noise_bad   = span<float>(noise_var).first(nof_symbols);
  // Set even-indexed noise variances to NaN.
  for (unsigned i_symbol = 0; i_symbol < nof_symbols; i_symbol += 2) {
    noise_bad[i_symbol] = std::numeric_limits<float>::quiet_NaN();
  }

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);
  demodulator->demodulate_soft(soft_bits, span<const cf_t>(symbols).first(nof_symbols), noise_bad, mod);

  bool are_zeros_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_zeros_ok = are_zeros_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                     return (a.to_int() == 0);
                   });
    local_soft   = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_zeros_ok) << "Dealing with NaN variance went wrong.";
  ASSERT_TRUE(are_others_ok) << "Dealing with NaN variance should not affect other soft bits.";
}

// Check that an infinite symbol doesn't crash the demodulator.
TEST_P(DemodulatorFixture, DemodulatorSymbolInfinity)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned   nof_symbols = 18;
  span<cf_t> symbol_bad  = span<cf_t>(symbols).first(nof_symbols);
  // Set even-indexed symbols to infinity.
  symbol_bad[0]  = cf_t(std::numeric_limits<float>::infinity(), 0);
  symbol_bad[2]  = cf_t(-std::numeric_limits<float>::infinity(), 0);
  symbol_bad[4]  = cf_t(0, std::numeric_limits<float>::infinity());
  symbol_bad[6]  = cf_t(0, -std::numeric_limits<float>::infinity());
  symbol_bad[8]  = cf_t(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
  symbol_bad[10] = cf_t(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
  symbol_bad[12] = cf_t(-std::numeric_limits<float>::infinity(), 0);
  symbol_bad[14] = cf_t(0, std::numeric_limits<float>::infinity());
  symbol_bad[16] = cf_t(0, -std::numeric_limits<float>::infinity());

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);

  demodulator->demodulate_soft(soft_bits, symbol_bad, span<const float>(noise_var).first(nof_symbols), mod);

  bool are_infinity_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_infinity_ok = are_infinity_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                        return ((a <= log_likelihood_ratio::max()) && (a >= log_likelihood_ratio::min()));
                      });
    local_soft      = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_infinity_ok) << "Dealing with NaN symbols went wrong.";
  ASSERT_TRUE(are_others_ok) << "Infinite symbols should not affect other soft bits.";
}

// Check that zero symbols are translated to zero bits.
TEST_P(DemodulatorFixture, DemodulatorSymbolZero)
{
  // Random generator to select the symbols that will become zero.
  std::mt19937                            rgen(symbols.size());
  std::uniform_int_distribution<unsigned> i_symbol_dist(0, symbols.size() - 1);

  unsigned bits_per_symbol = get_bits_per_symbol(mod);

  // Set one of every 12 symbols to zero.
  unsigned nof_zero_symbols = divide_ceil(symbols.size(), 12);
  for (unsigned i = 0; i != nof_zero_symbols; ++i) {
    // Select a random symbol.
    unsigned i_symbol = i_symbol_dist(rgen);

    // Set symbol to zero.
    symbols[i_symbol] = cf_t();

    // The expected bits must be zero.
    srsvec::zero(span<log_likelihood_ratio>(soft_bits_expected).subspan(bits_per_symbol * i_symbol, bits_per_symbol));
  }

  std::vector<log_likelihood_ratio> soft_bits(symbols.size() * bits_per_symbol);

  demodulator->demodulate_soft(soft_bits, symbols, noise_var, mod);

  ASSERT_EQ(span<const log_likelihood_ratio>(soft_bits), span<const log_likelihood_ratio>(soft_bits_expected));
}

// Check that a NaN symbol doesn't crash the demodulator.
TEST_P(DemodulatorFixture, DemodulatorSymbolNaN)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 4 symbols).
  unsigned   nof_symbols = 18;
  span<cf_t> symbol_bad  = span<cf_t>(symbols).first(nof_symbols);
  // Set even-indexed symbols to NaN.
  symbol_bad[0]  = cf_t(std::numeric_limits<float>::quiet_NaN(), 0);
  symbol_bad[2]  = cf_t(-std::numeric_limits<float>::quiet_NaN(), 0);
  symbol_bad[4]  = cf_t(0, std::numeric_limits<float>::quiet_NaN());
  symbol_bad[6]  = cf_t(0, -std::numeric_limits<float>::quiet_NaN());
  symbol_bad[8]  = cf_t(std::numeric_limits<float>::quiet_NaN(), -std::numeric_limits<float>::quiet_NaN());
  symbol_bad[10] = cf_t(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
  symbol_bad[12] = cf_t(-std::numeric_limits<float>::quiet_NaN(), 0);
  symbol_bad[14] = cf_t(0, std::numeric_limits<float>::quiet_NaN());
  symbol_bad[16] = cf_t(0, -std::numeric_limits<float>::quiet_NaN());

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);

  demodulator->demodulate_soft(soft_bits, symbol_bad, span<const float>(noise_var).first(nof_symbols), mod);

  bool are_zeros_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    for (unsigned i_bit = 0; i_bit != bits_per_symbol; ++i_bit) {
      // A bit is affected by the test if it has an even index and the real part of the channel symbol is NaN...
      bool is_affected = (i_bit % 2 == 0) && std::isnan(std::real(symbol_bad[i_symbol]));
      // ... or if it has an odd index and the imaginary part of the channel symbol is NaN.
      is_affected = is_affected || ((i_bit % 2 == 1) && std::isnan(std::imag(symbol_bad[i_symbol])));
      if (is_affected) {
        are_zeros_ok = are_zeros_ok && (local_soft[i_bit].to_int() == 0);
      } else {
        are_zeros_ok = are_zeros_ok && (local_soft[i_bit] <= log_likelihood_ratio::max()) &&
                       (local_soft[i_bit] >= log_likelihood_ratio::min());
      }
    }
    local_soft = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_zeros_ok) << "Dealing with NaN symbols went wrong.";
  ASSERT_TRUE(are_others_ok) << "Dealing with NaN symbols should not affect other soft bits.";
}

// Check that a pair of infinite variance and symbol do not cause a crash.
TEST_P(DemodulatorFixture, DemodulatorInfinityOverInfinity)
{
  // By taking 18 symbols, we test both the AVX2 implementation and the classic one (last 2 symbols).
  unsigned   nof_symbols = 18;
  span<cf_t> symbol_bad  = span<cf_t>(symbols).first(nof_symbols);
  // Set even-indexed symbols to infinity.
  symbol_bad[0]  = cf_t(std::numeric_limits<float>::infinity(), 0);
  symbol_bad[2]  = cf_t(-std::numeric_limits<float>::infinity(), 0);
  symbol_bad[4]  = cf_t(0, std::numeric_limits<float>::infinity());
  symbol_bad[6]  = cf_t(0, -std::numeric_limits<float>::infinity());
  symbol_bad[8]  = cf_t(std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
  symbol_bad[10] = cf_t(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
  symbol_bad[12] = cf_t(-std::numeric_limits<float>::infinity(), 0);
  symbol_bad[14] = cf_t(0, std::numeric_limits<float>::infinity());
  symbol_bad[16] = cf_t(0, -std::numeric_limits<float>::infinity());

  // Set even-indexed variances to infinity.
  span<float> noise_bad = span<float>(noise_var).first(nof_symbols);
  for (unsigned i_var = 0; i_var < nof_symbols; i_var += 2) {
    noise_bad[i_var] = std::numeric_limits<float>::infinity();
  }

  unsigned                          bits_per_symbol = get_bits_per_symbol(mod);
  std::vector<log_likelihood_ratio> soft_bits(nof_symbols * bits_per_symbol);

  demodulator->demodulate_soft(soft_bits, symbol_bad, span<const float>(noise_var).first(nof_symbols), mod);

  bool are_infinity_ok = true, are_others_ok = true;
  for (unsigned i_symbol = 0, offset = 0; i_symbol < nof_symbols; i_symbol += 2) {
    span<const log_likelihood_ratio> local_soft =
        span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_infinity_ok = are_infinity_ok && std::all_of(local_soft.begin(), local_soft.end(), [](log_likelihood_ratio a) {
                        return (a.to_int() == 0);
                      });
    local_soft      = span<const log_likelihood_ratio>(soft_bits).subspan(offset, bits_per_symbol);
    span<const log_likelihood_ratio> true_soft =
        span<const log_likelihood_ratio>(soft_bits_expected).subspan(offset, bits_per_symbol);
    offset += bits_per_symbol;
    are_others_ok = are_others_ok && std::equal(local_soft.begin(), local_soft.end(), true_soft.begin());
  }
  ASSERT_TRUE(are_infinity_ok) << "Dealing with infinity-over-infinity cases went wrong.";
  ASSERT_TRUE(are_others_ok) << "Infinity-over-infinity cases should not affect other soft bits.";
}

INSTANTIATE_TEST_SUITE_P(DemodulatorSuite, DemodulatorFixture, ::testing::ValuesIn(demodulation_mapper_test_data));

} // namespace