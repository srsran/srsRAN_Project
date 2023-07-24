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

#include "prach_detector_test_data.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

static bool is_recognized_conf(const srsran::prach_detector::configuration& conf)
{
  if (conf.nof_rx_ports == 1) {
    if ((conf.format == prach_format_type::zero) && (conf.zero_correlation_zone == 1)) {
      return true;
    }
    if ((conf.format == prach_format_type::B4) && (conf.zero_correlation_zone == 11)) {
      return true;
    }
    return false;
  }
  if (conf.nof_rx_ports == 2) {
    if ((conf.format == prach_format_type::zero) && (conf.zero_correlation_zone == 1)) {
      return true;
    }
    if ((conf.format == prach_format_type::A1) && (conf.zero_correlation_zone == 11)) {
      return true;
    }
    return false;
  }
  return false;
}

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t test_case)
{
  fmt::print(os, "{} {}", test_case.context.config, test_case.context.result);
  return os;
}

std::ostream& operator<<(std::ostream& os, phy_time_unit value)
{
  fmt::print(os, "{}usec", value.to_seconds() * 1e6);
  return os;
}

} // namespace srsran

using namespace srsran;

using PrachDetectorParams = test_case_t;

class PrachDetectorFixture : public ::testing::TestWithParam<PrachDetectorParams>
{
protected:
  std::unique_ptr<prach_detector>           detector;
  std::unique_ptr<prach_detector_validator> validator;

  void SetUp() override
  {
    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_TRUE(dft_factory);

    std::shared_ptr<prach_generator_factory> generator_factory = create_prach_generator_factory_sw();
    ASSERT_TRUE(generator_factory);

    std::shared_ptr<prach_detector_factory> detector_factory =
        create_prach_detector_factory_sw(dft_factory, generator_factory);
    ASSERT_TRUE(detector_factory);

#if 0
    srslog::init();
    detector = detector_factory->create(srslog::fetch_basic_logger("PRACH"), true);
#else
    detector = detector_factory->create();
#endif
    ASSERT_TRUE(detector);

    validator = detector_factory->create_validator();
    ASSERT_TRUE(validator);
  }
};

TEST_P(PrachDetectorFixture, FromVector)
{
  const PrachDetectorParams&           params          = GetParam();
  const prach_detector::configuration& config          = params.context.config;
  const prach_detection_result&        expected_result = params.context.result;
  const phy_time_unit&                 true_delay      = params.context.true_delay;
  auto                                 sequence_data   = params.symbols.read();

  // Make sure configuration is valid.
  // todo(david): this should be an assertion!
  // ASSERT_TRUE(validator->is_valid(config));
  if (!validator->is_valid(config)) {
    GTEST_SKIP() << "Unsupported PRACH configuration.";
  }

  // Get preamble information.
  prach_preamble_information preamble_info;
  if (is_long_preamble(config.format)) {
    preamble_info = get_prach_preamble_long_info(config.format);
  } else {
    preamble_info = get_prach_preamble_short_info(config.format, config.ra_scs, false);
  }

  // Calculate the number of symbols.
  unsigned nof_symbols =
      static_cast<unsigned>(preamble_info.symbol_length.to_seconds() * ra_scs_to_Hz(preamble_info.scs));

  // Get frequency domain data.
  prach_buffer_tensor sequence(sequence_data);
  ASSERT_EQ(sequence.get_sequence_length(), preamble_info.sequence_length);
  ASSERT_EQ(sequence.get_max_nof_symbols(), nof_symbols);
  ASSERT_EQ(sequence.get_max_nof_td_occasions(), 1);
  ASSERT_EQ(sequence.get_max_nof_fd_occasions(), 1);
  ASSERT_EQ(sequence.get_max_nof_ports(), config.nof_rx_ports);

  // Run generator.
  prach_detection_result result = detector->detect(sequence, config);

  // Calculate expected delay.
  phy_time_unit time_error_tolerance = phy_time_unit::from_seconds(1.04e-6F);
  if (config.ra_scs == prach_subcarrier_spacing::kHz15) {
    time_error_tolerance = phy_time_unit::from_seconds(0.52e-6F);
  } else if (config.ra_scs == prach_subcarrier_spacing::kHz30) {
    time_error_tolerance = phy_time_unit::from_seconds(0.26e-6F);
  }

  if (is_recognized_conf(config)) {
    // Assert the required preamble was found. The detector thresholds are tweaked to have a small FA probability when
    // there is no signal. However, when there is signal, spurious false detections may appear, especially when the SNR
    // is high.
    auto* it = std::find_if(result.preambles.begin(),
                            result.preambles.end(),
                            [&expected_result](const prach_detection_result::preamble_indication& a) {
                              return (a.preamble_index == expected_result.preambles.front().preamble_index);
                            });

    ASSERT_NE(it, result.preambles.end());

    // Verify the preamble index.
    const prach_detection_result::preamble_indication& preamble_indication = *it;
    ASSERT_EQ(expected_result.preambles.front().preamble_index, preamble_indication.preamble_index);
    // Assert the estimated time advance with respect to true one - should be less than time_error_tolerance.
    ASSERT_NEAR(
        preamble_indication.time_advance.to_seconds(), true_delay.to_seconds(), time_error_tolerance.to_seconds());

    // Assert the estimated time advance with respect to the expected one - we allow at most a difference equal to the
    // time resolution of the detection algorithm.
    ASSERT_NEAR(expected_result.preambles.front().time_advance.to_seconds(),
                preamble_indication.time_advance.to_seconds(),
                result.time_resolution.to_seconds());

    // Allow a 1% difference between expected and measured SNR.
    ASSERT_NEAR(expected_result.preambles.front().snr_dB,
                preamble_indication.snr_dB,
                std::abs(preamble_indication.snr_dB) / 100);

    // Allow a 1% difference between expected and measured RSSI.
    ASSERT_NEAR(expected_result.rssi_dB, result.rssi_dB, std::abs(result.rssi_dB) / 100);
  }
}

// Creates test suite that combines all possible parameters. Denote zero_correlation_zone exceeds the maximum by one.
INSTANTIATE_TEST_SUITE_P(PrachDetectorSimple, PrachDetectorFixture, ::testing::ValuesIn(prach_detector_test_data));
