/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../support/prach_buffer_test_doubles.h"
#include "prach_generator_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/ran/prach/prach_cyclic_shifts.h"
#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/prod.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

static std::mt19937                            rgen(1234);
static std::uniform_int_distribution<unsigned> root_sequence_index_dist(0, 837);
static std::uniform_int_distribution<unsigned> zero_correlation_zone_dist(0, 15);
static std::uniform_int_distribution<unsigned> preamble_index_dist(0, 63);
static std::uniform_int_distribution<unsigned> start_preamble_index_dist(0, 63);
static std::uniform_int_distribution<unsigned> nof_preamble_indices_dist(1, 64);

namespace srsgnb {
std::ostream& operator<<(std::ostream& os, preamble_format format)
{
  fmt::print(os, "{}", static_cast<unsigned>(format));
  return os;
}
std::ostream& operator<<(std::ostream& os, restricted_set_config config)
{
  fmt::print(os, "{}", static_cast<unsigned>(config));
  return os;
}
} // namespace srsgnb

// Parameters in order dft_size, format, restricted set and extra iterations.
using prach_detector_params = std::tuple<unsigned, preamble_format, restricted_set_config, unsigned>;

class PrachDetectorFixture : public ::testing::TestWithParam<prach_detector_params>
{
protected:
  std::shared_ptr<dft_processor_factory_spy>   dft_factory_spy       = std::make_shared<dft_processor_factory_spy>();
  std::shared_ptr<prach_generator_factory_spy> generator_factory_spy = std::make_shared<prach_generator_factory_spy>();
  std::unique_ptr<prach_detector>              detector              = nullptr;
  dft_processor_spy*                           dft_spy               = nullptr;
  prach_generator_spy*                         generator_spy         = nullptr;

  void SetUp() override
  {
    unsigned dft_size = std::get<0>(GetParam());

    std::shared_ptr<prach_detector_factory> prach_detector_factory =
        create_prach_detector_factory_simple(dft_factory_spy, generator_factory_spy, dft_size);
    ASSERT_TRUE(prach_detector_factory);

    detector = prach_detector_factory->create();
    ASSERT_TRUE(detector);

    // The factory should have created one entry of each factory.
    ASSERT_EQ(1, dft_factory_spy->get_entries().size());
    ASSERT_EQ(1, generator_factory_spy->get_entries().size());

    // Select spies.
    dft_spy       = (dft_factory_spy->get_entries().back().dft);
    generator_spy = (generator_factory_spy->get_entries().back());
  }

  phy_time_unit GetExpectedTimeAdvancedResolution() const
  {
    unsigned prach_scs_Hz = get_prach_preamble_long_info(std::get<1>(GetParam())).scs.to_Hz();
    unsigned dft_size     = std::get<0>(GetParam());

    return phy_time_unit::from_seconds(1 / static_cast<double>(dft_size * prach_scs_Hz));
  }

  phy_time_unit GetExpectedTimeAdvancedMaximum(unsigned zero_correlation_zone) const
  {
    unsigned                   prach_scs_Hz     = get_prach_preamble_long_info(std::get<1>(GetParam())).scs.to_Hz();
    unsigned                   dft_size         = std::get<0>(GetParam());
    unsigned                   sampling_rate_Hz = prach_scs_Hz * dft_size;
    prach_preamble_information preamble_info    = get_prach_preamble_long_info(std::get<1>(GetParam()));
    restricted_set_config      restricted_set   = std::get<2>(GetParam());
    unsigned                   N_cs = prach_cyclic_shifts_get(preamble_info.scs, restricted_set, zero_correlation_zone);

    // Calculate maximum delay due to the cyclic prefix.
    phy_time_unit time_advance_max = preamble_info.cp_length;

    // If the cyclic shift is not zero...
    if (N_cs != 0) {
      // Calculate the maximum time in advance limited by the number of cyclic shifts.
      phy_time_unit N_cs_time =
          phy_time_unit::from_seconds(static_cast<double>((N_cs * preamble_info.sequence_length) / dft_size) /
                                      static_cast<double>(sampling_rate_Hz));
      // Select the most limiting value.
      time_advance_max = std::min(time_advance_max, N_cs_time);
    }

    return time_advance_max;
  }
};

TEST_P(PrachDetectorFixture, Flow)
{
  // Prepare derived parameters.
  unsigned sequence_length = get_prach_preamble_long_info(std::get<1>(GetParam())).sequence_length;
  unsigned dft_size        = std::get<0>(GetParam());

  // Generate frequency domain buffer data. A complex exponential with an average power of 1 (0.0 dB).
  std::vector<cf_t> buffer_data(sequence_length);
  std::generate(buffer_data.begin(), buffer_data.end(), [&, n = 0]() mutable {
    return std::exp(COMPLEX_J * static_cast<float>(n++) / static_cast<float>(sequence_length));
  });

  // Prepare PRACH buffer.
  prach_buffer_spy buffer;
  buffer.set_symbol_data(buffer_data, sequence_length, 1);

  // Generate PRACH configuration
  prach_detector::configuration detector_config;
  detector_config.root_sequence_index   = root_sequence_index_dist(rgen);
  detector_config.format                = std::get<1>(GetParam());
  detector_config.restricted_set        = std::get<2>(GetParam());
  detector_config.zero_correlation_zone = zero_correlation_zone_dist(rgen);
  detector_config.start_preamble_index  = start_preamble_index_dist(rgen);
  detector_config.nof_preamble_indices  = nof_preamble_indices_dist(rgen) % (64 - detector_config.start_preamble_index);

  // Run the detector.
  prach_detection_result detection_result = detector->detect(buffer, detector_config);

  // Verify the buffer calls.
  ASSERT_EQ(detector_config.nof_preamble_indices + 1, buffer.get_get_symbol_const_entries().size());

  // Verify that the generator and the DFT are called for each preamble.
  ASSERT_EQ(detector_config.nof_preamble_indices, generator_spy->get_entries().size());
  ASSERT_EQ(detector_config.nof_preamble_indices, dft_spy->get_entries().size());

  // Verify each preamble processing.
  for (unsigned index = 0; index < detector_config.nof_preamble_indices; ++index) {
    unsigned preamble_index = detector_config.start_preamble_index + index;

    // Verify PRACH generation configuration.
    const prach_generator_spy::generate_entry& generator_entry = generator_spy->get_entries()[index];
    ASSERT_EQ(detector_config.format, generator_entry.config.format);
    ASSERT_EQ(detector_config.root_sequence_index, generator_entry.config.root_sequence_index);
    ASSERT_EQ(preamble_index, generator_entry.config.preamble_index);
    ASSERT_EQ(detector_config.restricted_set, generator_entry.config.restricted_set);
    ASSERT_EQ(detector_config.zero_correlation_zone, generator_entry.config.zero_correlation_zone);

    // Generate expected frequency domain data before DFT.
    std::vector<cf_t> freq_domain_data = generator_entry.result;
    srsvec::prod_conj(buffer_data, freq_domain_data, freq_domain_data);

    // Extract DFT entry.
    const dft_processor_spy::entry& dft_entry = dft_spy->get_entries()[index];

    // Verify DFT input, upper grid.
    for (unsigned dft_index = 0, freq_index = sequence_length / 2, dft_index_end = (sequence_length + 1) / 2;
         dft_index != dft_index_end;
         ++dft_index, ++freq_index) {
      ASSERT_NEAR(freq_domain_data[freq_index].real(), dft_entry.input[dft_index].real(), 0.0001);
      ASSERT_NEAR(freq_domain_data[freq_index].imag(), dft_entry.input[dft_index].imag(), 0.0001);
    }

    // Verify DFT input, guards.
    for (unsigned k = (sequence_length + 1) / 2, k_end = dft_size - sequence_length / 2; k != k_end; ++k) {
      ASSERT_EQ(0.0, dft_entry.input[k].real());
      ASSERT_EQ(0.0, dft_entry.input[k].imag());
    }

    // Verify DFT input, lower grid.
    for (unsigned dft_index = dft_size - sequence_length / 2, freq_index = 0, dft_index_end = dft_size;
         dft_index != dft_index_end;
         ++dft_index, ++freq_index) {
      ASSERT_NEAR(freq_domain_data[freq_index].real(), dft_entry.input[dft_index].real(), 0.0001);
      ASSERT_NEAR(freq_domain_data[freq_index].imag(), dft_entry.input[dft_index].imag(), 0.0001);
    }
  }

  // Verify the detection results. As the IDFT output is random, no preamble is expected to be detected.
  ASSERT_EQ(detection_result.time_resolution, GetExpectedTimeAdvancedResolution());
  ASSERT_EQ(detection_result.time_advance_max, GetExpectedTimeAdvancedMaximum(detector_config.zero_correlation_zone));
  ASSERT_NEAR(0.0F, detection_result.rssi_dB, 1e-3F);
  ASSERT_EQ(0, detection_result.preambles.size());
}

TEST_P(PrachDetectorFixture, RssiZero)
{
  // Prepare derived parameters.
  unsigned sequence_length = get_prach_preamble_long_info(std::get<1>(GetParam())).sequence_length;

  // Generate frequency domain buffer data. All zeros to trigger a zero RSSI.
  std::vector<cf_t> buffer_data(sequence_length);
  std::fill(buffer_data.begin(), buffer_data.end(), 0.0);

  // Prepare PRACH buffer.
  prach_buffer_spy buffer;
  buffer.set_symbol_data(buffer_data, sequence_length, 1);

  // Generate PRACH configuration. It is irrelevant.
  prach_detector::configuration detector_config;
  detector_config.root_sequence_index   = root_sequence_index_dist(rgen);
  detector_config.format                = std::get<1>(GetParam());
  detector_config.restricted_set        = std::get<2>(GetParam());
  detector_config.zero_correlation_zone = zero_correlation_zone_dist(rgen);
  detector_config.start_preamble_index  = start_preamble_index_dist(rgen);
  detector_config.nof_preamble_indices  = nof_preamble_indices_dist(rgen) % (64 - detector_config.start_preamble_index);

  // Run the detector.
  prach_detection_result detection_result = detector->detect(buffer, detector_config);

  // Verify the buffer calls.
  ASSERT_EQ(1, buffer.get_get_symbol_const_entries().size());

  // Verify that the generator and the DFT are called for each preamble.
  ASSERT_EQ(0, generator_spy->get_entries().size());
  ASSERT_EQ(0, dft_spy->get_entries().size());

  // Verify the detection results. As the IDFT output is random, no preamble is expected to be detected.
  ASSERT_EQ(detection_result.time_resolution, GetExpectedTimeAdvancedResolution());
  ASSERT_EQ(detection_result.time_advance_max, GetExpectedTimeAdvancedMaximum(detector_config.zero_correlation_zone));
  ASSERT_EQ(-INFINITY, detection_result.rssi_dB);
  ASSERT_EQ(0, detection_result.preambles.size());
}

TEST_P(PrachDetectorFixture, ThresholdTest)
{
  // Prepare derived parameters.
  unsigned sequence_length       = get_prach_preamble_long_info(std::get<1>(GetParam())).sequence_length;
  unsigned prach_scs_Hz          = get_prach_preamble_long_info(std::get<1>(GetParam())).scs.to_Hz();
  unsigned dft_size              = dft_spy->get_size();
  unsigned sampling_rate_Hz      = prach_scs_Hz * dft_size;
  unsigned zero_correlation_zone = zero_correlation_zone_dist(rgen);

  // Generate frequency domain buffer data. All ones with a RSSI equal to one.
  std::vector<cf_t> buffer_data(sequence_length);
  std::fill(buffer_data.begin(), buffer_data.end(), 1.0);

  // Correlation data.
  std::vector<cf_t> correlation(dft_spy->get_size());
  std::fill(correlation.begin(), correlation.end(), 0.0);

  // Select peak position and set a large enough peak.
  unsigned peak_pos     = rgen() % GetExpectedTimeAdvancedMaximum(zero_correlation_zone).to_samples(sampling_rate_Hz);
  float    peak_value   = static_cast<float>(sequence_length);
  correlation[peak_pos] = peak_value;

  // Expected delay.
  double expected_delay_s = static_cast<double>(peak_pos) / static_cast<double>(dft_size * prach_scs_Hz);

  // Force DFT ouput.
  dft_spy->set_output_buffer(correlation);

  // Prepare PRACH buffer.
  prach_buffer_spy buffer;
  buffer.set_symbol_data(buffer_data, sequence_length, 1);

  // Generate PRACH configuration. Only one preamble index.
  prach_detector::configuration detector_config;
  detector_config.root_sequence_index   = root_sequence_index_dist(rgen);
  detector_config.format                = std::get<1>(GetParam());
  detector_config.restricted_set        = std::get<2>(GetParam());
  detector_config.zero_correlation_zone = zero_correlation_zone;
  detector_config.start_preamble_index  = start_preamble_index_dist(rgen);
  detector_config.nof_preamble_indices  = 1;

  // Run the detector.
  prach_detection_result detection_result = detector->detect(buffer, detector_config);

  // Verify the buffer calls.
  ASSERT_EQ(detector_config.nof_preamble_indices + 1, buffer.get_get_symbol_const_entries().size());

  // Verify that the generator and the DFT are called for each preamble.
  ASSERT_EQ(1, generator_spy->get_entries().size());
  ASSERT_EQ(1, dft_spy->get_entries().size());

  // Verify the detection results. As the IDFT output is random, no preamble is expected to be detected.
  ASSERT_EQ(detection_result.time_resolution, GetExpectedTimeAdvancedResolution());
  ASSERT_EQ(detection_result.time_advance_max, GetExpectedTimeAdvancedMaximum(detector_config.zero_correlation_zone));
  ASSERT_NEAR(0.0, detection_result.rssi_dB, 1e-6);
  ASSERT_EQ(1, detection_result.preambles.size());
  ASSERT_EQ(detector_config.start_preamble_index, detection_result.preambles[0].preamble_index);
  ASSERT_NEAR(expected_delay_s,
              detection_result.preambles[0].time_advance.to_seconds(),
              detection_result.time_resolution.to_seconds());
  ASSERT_NEAR(convert_amplitude_to_dB(peak_value), detection_result.preambles[0].power_dB, 1e-5);
  ASSERT_NEAR(0.0, detection_result.preambles[0].snr_dB, 1e-5);
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(PrachDetectorSimple,
                         PrachDetectorFixture,
                         ::testing::Combine(::testing::Values(1536U),
                                            ::testing::Values(preamble_format::FORMAT0,
                                                              preamble_format::FORMAT1,
                                                              preamble_format::FORMAT2,
                                                              preamble_format::FORMAT3),
                                            ::testing::Values(restricted_set_config::UNRESTRICTED),
                                            ::testing::Range(0U, 10U)));
