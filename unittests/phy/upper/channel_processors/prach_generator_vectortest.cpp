/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_generator_test_data.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsgnb {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "Format={}; RootSequenceIndex={}; PreambleIndex={}; RestrictedSet={}; ZeroCorrelationZone={};",
             static_cast<unsigned>(test_case.config.format),
             test_case.config.root_sequence_index,
             test_case.config.preamble_index,
             static_cast<unsigned>(test_case.config.restricted_set),
             test_case.config.zero_correlation_zone);
  return os;
}

} // namespace srsgnb

using namespace srsgnb;

class prach_generator_test : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<prach_generator> generator;

  void SetUp() override
  {
    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw(dft_factory);
    ASSERT_TRUE(generator_factory);

    generator = generator_factory->create();
    ASSERT_TRUE(generator);
  }
};

TEST_P(prach_generator_test, vector)
{
  const test_case_t& test_case = GetParam();

  // Restricted sets are not implemented. Skip.
  if (test_case.config.restricted_set != restricted_set_config::UNRESTRICTED) {
    GTEST_SKIP();
  }

  std::vector<cf_t> expected_output = test_case.sequence.read();

  // Run generator.
  span<const cf_t> output = generator->generate(test_case.config);

  // Make sure sizes match.
  ASSERT_EQ(expected_output.size(), output.size());

  // For each element...
  for (unsigned index = 0; index != output.size(); ++index) {
    EXPECT_NEAR(expected_output[index].real(), output[index].real(), 0.0001);
    EXPECT_NEAR(expected_output[index].imag(), output[index].imag(), 0.0001);
  }
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(ofdm_prach_demodulator_vectortest,
                         prach_generator_test,
                         ::testing::ValuesIn(prach_generator_test_data));
