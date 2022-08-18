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
#include "srsgnb/srslog/bundled/fmt/ostream.h"
#include "srsgnb/srsvec/copy.h"
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

class PrachDetectorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  std::unique_ptr<prach_detector> detector;

  PrachDetectorFixture() {}

  void SetUp() override
  {
    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_generic();
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw(dft_factory);
    ASSERT_TRUE(generator_factory);

    std::shared_ptr<prach_detector_factory> detector_factory =
        create_prach_detector_factory_simple(dft_factory, generator_factory);
    ASSERT_TRUE(detector_factory);

    detector = detector_factory->create();
    ASSERT_TRUE(detector);
  }
};

TEST_P(PrachDetectorFixture, FromVector)
{
  const test_case_t& test_case = GetParam();

  // Restricted sets are not implemented. Skip.
  if (test_case.config.restricted_set != restricted_set_config::UNRESTRICTED) {
    GTEST_SKIP();
  }

  std::vector<cf_t> expected_output = test_case.sequence.read();

  prach_detector::configuration detector_config;
  detector_config.root_sequence_index   = test_case.config.root_sequence_index;
  detector_config.format                = test_case.config.format;
  detector_config.restricted_set        = test_case.config.restricted_set;
  detector_config.zero_correlation_zone = test_case.config.zero_correlation_zone;
  detector_config.start_preamble_index  = 0;
  detector_config.nof_preamble_indices  = 64;

  // Create buffer.
  std::unique_ptr<prach_buffer> buffer = create_prach_buffer_long();
  ASSERT_TRUE(buffer);

  std::vector<cf_t> frequency_data = test_case.sequence.read();

  // Fill buffer with time frequency-domain data.
  srsvec::copy(buffer->get_symbol(0), span<cf_t>(frequency_data).first(839));

  // Run generator.
  prach_detection_result result = detector->detect(*buffer, detector_config);

  // Assert a one preamble is found.
  ASSERT_EQ(1, result.preambles.size());

  // Verify the preamble index.
  prach_detection_result::preamble_indication& preamble_info = result.preambles.back();
  ASSERT_EQ(test_case.config.preamble_index, preamble_info.preamble_index);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(OfdmPrachDetector, PrachDetectorFixture, ::testing::ValuesIn(prach_generator_test_data));