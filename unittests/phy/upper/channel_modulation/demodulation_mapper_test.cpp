/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Demodulation mapper unit test.
///
/// The test takes as input vectors containing noisy modulated symbols and the corresponding noise variances. The
/// symbols are demodulated and the resulting bits (both soft and hard versions) are compared with the expected values,
/// also provided by test vectors.

#include "demodulation_mapper_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/support/srsgnb_test.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

namespace srsgnb {
std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  return os << fmt::format("{} modulation, {} symbols", to_string(tc.scheme), tc.nsymbols);
}

} // namespace srsgnb
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

  static std::shared_ptr<channel_modulation_factory> factory;
};

std::shared_ptr<channel_modulation_factory> DemodulatorFixture::factory = nullptr;

TEST_P(DemodulatorFixture, DemodulatorTest)
{
  // Assert factory again for compatibility with GTest < 1.11.
  ASSERT_NE(factory, nullptr);

  // Create a demodulator.
  test_case_t                          test_case   = GetParam();
  std::unique_ptr<demodulation_mapper> demodulator = factory->create_demodulation_mapper();
  ASSERT_NE(demodulator, nullptr);

  // Read test input and output.
  const modulation_scheme mod = test_case.scheme;

  const unsigned          nof_symbols = test_case.nsymbols;
  const std::vector<cf_t> symbols     = test_case.symbols.read();
  ASSERT_EQ(symbols.size(), nof_symbols) << "Error reading modulated symbols.";

  const std::vector<float> noise_var = test_case.noise_var.read();
  ASSERT_EQ(noise_var.size(), nof_symbols) << "Error reading noise variances.";

  ASSERT_TRUE(std::all_of(noise_var.cbegin(), noise_var.cend(), [](float f) { return f > 0; }))
      << "Noise variances should take positive values.";

  const unsigned nof_bits = nof_symbols * get_bits_per_symbol(mod);

  const std::vector<log_likelihood_ratio> soft_bits_expected = test_case.soft_bits.read();
  ASSERT_EQ(soft_bits_expected.size(), nof_bits) << "Error reading soft bits.";

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

INSTANTIATE_TEST_SUITE_P(DemodulatorSuite, DemodulatorFixture, ::testing::ValuesIn(demodulation_mapper_test_data));

} // namespace