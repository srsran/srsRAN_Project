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

#include "pucch_detector_test_doubles.h"
#include "pucch_processor_format0_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/srsvec/compare.h"
#include "srsran/support/complex_normal_random.h"
#include <gtest/gtest.h>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, const pucch_processor::format0_configuration& config)
{
  return os << fmt::format("{}", config);
}

std::ostream& operator<<(std::ostream& os, const test_case_t& tc)
{
  os << tc.entry.config;
  return os;
}

std::ostream& operator<<(std::ostream& os, const uci_status& status)
{
  switch (status) {
    case uci_status::unknown:
      return os << "unknown";
    case uci_status::valid:
      return os << "valid";
    case uci_status::invalid:
    default:
      return os << "invalid";
  }
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

} // namespace srsran

namespace {

using PucchProcessorFormat0Param = test_case_t;

class PucchProcessorFormat0Fixture : public ::testing::TestWithParam<PucchProcessorFormat0Param>
{
protected:
  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }

    std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory = create_low_papr_sequence_generator_sw_factory();
    ASSERT_NE(lpg_factory, nullptr);

    std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
        create_low_papr_sequence_collection_sw_factory(lpg_factory);
    ASSERT_NE(lpc_factory, nullptr) << "Cannot create low PAPR sequence collection factory.";

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr) << "Cannot create PRG factory.";

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

    std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
        create_time_alignment_estimator_dft_factory(dft_factory);
    ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

    // Create channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw(ta_estimator_factory);
    ASSERT_NE(port_chan_estimator_factory, nullptr) << "Cannot create port channel estimator factory.";

    std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
        create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, port_chan_estimator_factory);
    ASSERT_NE(estimator_factory, nullptr) << "Cannot create DM-RS PUCCH estimator factory.";

    // Create factories required by the PUCCH demodulator factory.
    std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_generic_factory();
    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory.";

    std::shared_ptr<pucch_detector_factory> detector_factory =
        create_pucch_detector_factory_sw(lpc_factory, prg_factory, equalizer_factory);
    ASSERT_NE(detector_factory, nullptr);

    std::shared_ptr<channel_modulation_factory> demod_factory = create_channel_modulation_sw_factory();
    ASSERT_NE(demod_factory, nullptr) << "Cannot create channel modulation factory.";

    // Create PUCCH demodulator factory.
    std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
        create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory);
    ASSERT_NE(pucch_demod_factory, nullptr) << "Cannot create PUCCH demodulator factory.";

    // Create short block detector factory.
    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    ASSERT_NE(short_block_det_factory, nullptr) << "Cannot create short block detector factory.";

    // Create polar decoder factory.
    std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
    ASSERT_NE(polar_dec_factory, nullptr) << "Invalid polar decoder factory.";

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calc_factory, nullptr) << "Invalid CRC calculator factory.";

    // Create UCI decoder factory.
    std::shared_ptr<uci_decoder_factory> uci_dec_factory =
        create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
    ASSERT_NE(uci_dec_factory, nullptr) << "Cannot create UCI decoder factory.";

    channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
    channel_estimate_dimensions.nof_tx_layers = 1;
    channel_estimate_dimensions.nof_rx_ports  = 4;
    channel_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    channel_estimate_dimensions.nof_prb       = MAX_RB;

    factory = create_pucch_processor_factory_sw(
        estimator_factory, detector_factory, pucch_demod_factory, uci_dec_factory, channel_estimate_dimensions);
    ASSERT_NE(factory, nullptr);
  }

  void SetUp() override
  {
    ASSERT_NE(factory, nullptr);
    processor = factory->create();
    ASSERT_NE(processor, nullptr) << "Could not create PUCCH processor.";
    validator = factory->create_validator();
    ASSERT_NE(validator, nullptr) << "Could not create PUCCH validator.";
  }

  static std::shared_ptr<pucch_processor_factory> factory;

  // PUCCH processor.
  std::unique_ptr<pucch_processor> processor;
  // PUCCH processor validator.
  std::unique_ptr<pucch_pdu_validator> validator;
};

std::shared_ptr<pucch_processor_factory> PucchProcessorFormat0Fixture::factory;

TEST_P(PucchProcessorFormat0Fixture, FromVector)
{
  // Prepare resource grid.
  resource_grid_reader_spy grid;
  grid.write(GetParam().grid.read());

  const PucchProcessorFormat0Param& param = GetParam();

  const pucch_entry& entry = param.entry;
  // Make sure configuration is valid.
  ASSERT_TRUE(validator->is_valid(entry.config));

  pucch_processor_result result = processor->process(grid, entry.config);

  // Check channel state information.
  // Time alignment shouldn't exceed plus minus 3 us.
  std::optional<phy_time_unit> time_aligment = result.csi.get_time_alignment();
  ASSERT_FALSE(time_aligment.has_value());
  // EPRE depends on the number of entries.
  std::optional<float> epre_dB = result.csi.get_epre_dB();
  ASSERT_TRUE(epre_dB.has_value());
  ASSERT_NEAR(epre_dB.value(), 0.0, 0.2);
  // SINR should be larger than -5 dB.
  std::optional<float> sinr_dB = result.csi.get_sinr_dB();
  ASSERT_TRUE(sinr_dB.has_value());
  ASSERT_GT(sinr_dB.value(), -5.0) << "Entry configuration: " << entry.config;

  // The message shall be valid.
  ASSERT_EQ(result.message.get_status(), uci_status::valid);
  ASSERT_EQ(result.message.get_harq_ack_bits().size(), entry.ack_bits.size());
  if (!entry.ack_bits.empty()) {
    ASSERT_EQ(span<const uint8_t>(result.message.get_harq_ack_bits()), span<const uint8_t>(entry.ack_bits));
  }
  if (entry.sr.has_value()) {
    ASSERT_EQ(result.message.get_sr_bits().size(), 1);
    ASSERT_EQ(result.message.get_sr_bits().front(), entry.sr.value());
  } else {
    ASSERT_TRUE(result.message.get_sr_bits().empty());
  }
  ASSERT_TRUE(result.message.get_csi_part1_bits().empty());
  ASSERT_TRUE(result.message.get_csi_part2_bits().empty());
}

TEST_P(PucchProcessorFormat0Fixture, FalseAlarm)
{
  std::vector<resource_grid_reader_spy::expected_entry_t> res = GetParam().grid.read();

  complex_normal_distribution<cf_t> noise = {};
  std::mt19937                      rgen(12345);

  unsigned nof_trials = 200;
  // Acceptable probability of false alarm. The value is higher than the 1% given by the PUCCH requirements in TS38.104
  // Section 8.3.
  // Important: This is just a quick test, longer simulations are needed to properly estimate the PFA.
  float acceptable_pfa = 0.1;

  // Prepare resource grid.
  resource_grid_reader_spy grid;
  unsigned                 counter = 0;
  for (unsigned i = 0; i != nof_trials; ++i) {
    grid.reset();
    for (auto& entry : res) {
      entry.value = noise(rgen);
    }
    grid.write(res);

    const PucchProcessorFormat0Param& param = GetParam();
    const pucch_entry&                entry = param.entry;

    // Make sure configuration is valid.
    ASSERT_TRUE(validator->is_valid(entry.config));

    pucch_processor_result result = processor->process(grid, entry.config);

    counter += static_cast<unsigned>(result.message.get_status() == uci_status::valid);
  }

  // Assert that the probability of false alarm doesn't exceed the acceptable value.
  float pfa = static_cast<float>(counter) / nof_trials;
  ASSERT_TRUE(pfa <= acceptable_pfa) << fmt::format(
      "Probability of false alarms too high: {} while max is {} ({} hits out of {} trials).",
      pfa,
      acceptable_pfa,
      counter,
      nof_trials);
}

INSTANTIATE_TEST_SUITE_P(PucchProcessorFormat0,
                         PucchProcessorFormat0Fixture,
                         ::testing::ValuesIn(pucch_processor_format0_test_data));

} // namespace
