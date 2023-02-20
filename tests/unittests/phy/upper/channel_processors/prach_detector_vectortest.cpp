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
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/copy.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

namespace srsran {

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

} // namespace srsran

using namespace srsran;

using PrachDetectorParams = std::tuple<unsigned, int, test_case_t>;

class PrachDetectorFixture : public ::testing::TestWithParam<PrachDetectorParams>
{
protected:
  std::unique_ptr<prach_detector> detector;

  void SetUp() override
  {
    unsigned dft_size_detector = std::get<0>(GetParam());

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw();
    ASSERT_TRUE(generator_factory);

    std::shared_ptr<prach_detector_factory> detector_factory =
        create_prach_detector_factory_simple(dft_factory, generator_factory, dft_size_detector);
    ASSERT_TRUE(detector_factory);

    detector = detector_factory->create();
    ASSERT_TRUE(detector);
  }
};

TEST_P(PrachDetectorFixture, FromVector)
{
  const PrachDetectorParams&            params            = GetParam();
  const prach_generator::configuration& config            = std::get<2>(params).config;
  const file_vector<cf_t>&              sequence          = std::get<2>(params).sequence;
  int                                   delay_samples     = std::get<1>(params);
  unsigned                              dft_size_detector = std::get<0>(params);

  // Restricted sets are not implemented. Skip.
  if (config.restricted_set != restricted_set_config::UNRESTRICTED) {
    GTEST_SKIP();
  }

  prach_detector::configuration detector_config;
  detector_config.root_sequence_index   = config.root_sequence_index;
  detector_config.format                = config.format;
  detector_config.restricted_set        = config.restricted_set;
  detector_config.zero_correlation_zone = config.zero_correlation_zone;
  detector_config.start_preamble_index  = 0;
  detector_config.nof_preamble_indices  = 64;

  // Create buffer.
  std::unique_ptr<prach_buffer> buffer = create_prach_buffer_long();
  ASSERT_TRUE(buffer);

  prach_preamble_information preamble_info = get_prach_preamble_long_info(config.format);

  // Get frequency domain data.
  std::vector<cf_t> frequency_data = sequence.read();
  std::transform(frequency_data.begin(), frequency_data.end(), frequency_data.begin(), [&, n = 0](cf_t sample) mutable {
    return sample * std::exp(-COMPLEX_J * TWOPI * static_cast<float>(n++) * static_cast<float>(delay_samples) /
                             static_cast<float>(dft_size_detector));
  });

  // Fill buffer with time frequency-domain data.
  srsvec::copy(buffer->get_symbol(0), span<cf_t>(frequency_data).first(839));

  // Run generator.
  prach_detection_result result = detector->detect(*buffer, detector_config);

  // Calculate expected delay.
  phy_time_unit expected_delay = phy_time_unit::from_seconds(
      static_cast<double>(delay_samples) / static_cast<double>(dft_size_detector * preamble_info.scs.to_Hz()));

  // Assert a one preamble is found.
  ASSERT_EQ(1, result.preambles.size());

  // Verify the preamble index.
  prach_detection_result::preamble_indication& preamble_indication = result.preambles.back();
  ASSERT_EQ(config.preamble_index, preamble_indication.preamble_index);
  ASSERT_EQ(expected_delay, preamble_indication.time_advance);
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(PrachDetectorSimple,
                         PrachDetectorFixture,
                         ::testing::Combine(::testing::Values(1536),
                                            ::testing::Values(-8, 0, 1, 3),
                                            ::testing::ValuesIn(prach_generator_test_data)));
