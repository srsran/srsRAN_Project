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

#include "prach_generator_test_data.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

static std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os,
             "Format={}; RootSequenceIndex={}; PreambleIndex={}; RestrictedSet={}; ZeroCorrelationZone={};",
             to_string(test_case.config.format),
             test_case.config.root_sequence_index,
             test_case.config.preamble_index,
             static_cast<unsigned>(test_case.config.restricted_set),
             test_case.config.zero_correlation_zone);
  return os;
}

static std::ostream& operator<<(std::ostream& os, span<const cf_t> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const cf_t> in0, span<const cf_t> in1)
{
  return std::equal(in0.begin(), in0.end(), in1.begin(), in1.end(), [](cf_t value0, cf_t value1) {
    return (std::abs(value0 - value1) <= 0.0001F);
  });
}

} // namespace srsran

using namespace srsran;

class prach_generator_test : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<prach_generator> generator;

  void SetUp() override
  {
    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw();
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

  // For each element...
  ASSERT_EQ(span<const cf_t>(expected_output), span<const cf_t>(output));
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(ofdm_prach_demodulator_vectortest,
                         prach_generator_test,
                         ::testing::ValuesIn(prach_generator_test_data));
