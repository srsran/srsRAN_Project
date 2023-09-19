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

#include "../../support/resource_grid_test_doubles.h"
#include "../signal_processors/dmrs_pucch_processor_test_doubles.h"
#include "pucch_detector_test_doubles.h"
#include "uci_decoder_test_doubles.h"
#include <gtest/gtest.h>

using namespace srsran;

static constexpr unsigned nof_repetitions = 100;

namespace {

class PucchProcessorFormat1Fixture : public ::testing::TestWithParam<unsigned>
{
protected:
  /// PUCCH Processor factory.
  static std::shared_ptr<pucch_processor_factory> processor_factory;
  /// Channel estimator spy factory.
  static std::shared_ptr<dmrs_pucch_estimator_factory_spy> dmrs_factory_spy;
  /// PUCCH detector spy factory.
  static std::shared_ptr<pucch_detector_factory_spy> detector_factory_spy;

  static void SetUpTestSuite()
  {
    if (!processor_factory) {
      // Create spy factories.
      dmrs_factory_spy     = std::make_shared<dmrs_pucch_estimator_factory_spy>();
      detector_factory_spy = std::make_shared<pucch_detector_factory_spy>();

      // Create factories required by the PUCCH demodulator factory.
      std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_factory_zf();
      ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory.";

      std::shared_ptr<channel_modulation_factory> demod_factory = create_channel_modulation_sw_factory();
      ASSERT_NE(demod_factory, nullptr) << "Cannot create channel modulation factory.";

      std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
      ASSERT_NE(prg_factory, nullptr) << "Cannot create pseudo random generator factory.";

      // Create PUCCH demodulator factory.
      std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
          create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory);
      ASSERT_NE(pucch_demod_factory, nullptr) << "Cannot create PUCCH demodulator factory.";

      std::shared_ptr<uci_decoder_factory_spy> decoder_factory = std::make_shared<uci_decoder_factory_spy>();
      ASSERT_NE(decoder_factory, nullptr) << "Cannot create UCI decoder factory.";

      // Prepare Channel dimensions.
      channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
      channel_estimate_dimensions.nof_tx_layers = 1;
      channel_estimate_dimensions.nof_rx_ports  = 1;
      channel_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
      channel_estimate_dimensions.nof_prb       = MAX_RB;

      // Create PUCCH processor factory.
      processor_factory = create_pucch_processor_factory_sw(
          dmrs_factory_spy, detector_factory_spy, pucch_demod_factory, decoder_factory, channel_estimate_dimensions);
      ASSERT_NE(processor_factory, nullptr) << "Cannot create PUCCH processor factory.";
    }
  }

  void SetUp() override
  {
    // Assert PUCCH Processor factory.
    ASSERT_NE(processor_factory, nullptr) << "Cannot create PUCCH processor factory.";

    // Create PUCCH processor.
    processor = processor_factory->create();
    ASSERT_NE(processor, nullptr) << "Cannot create PUCCH processor.";

    // Create PUCCH validator.
    validator = processor_factory->create_validator();
    ASSERT_NE(validator, nullptr) << "Cannot create PUCCH processor validator.";

    // Select spies.
    dmrs_spy = dmrs_factory_spy->get_format1_entries().back();
    ASSERT_NE(dmrs_spy, nullptr);

    detector_spy = detector_factory_spy->get_entries().back();
    ASSERT_NE(detector_spy, nullptr);
  }

  pucch_processor::format1_configuration GetConfig()
  {
    std::mt19937                           rgen = std::mt19937(GetParam());
    pucch_processor::format1_configuration config;

    std::uniform_int_distribution<unsigned> num_dist(0, static_cast<unsigned>(subcarrier_spacing::kHz240));
    std::uniform_int_distribution<unsigned> slot_dist(0, 160 * 1024 - 1);
    std::uniform_int_distribution<unsigned> bwp_size_dist(1, MAX_RB);
    std::uniform_int_distribution<unsigned> bool_dist(0, 1);
    std::uniform_int_distribution<unsigned> starting_prb_dist(0, 274);
    std::uniform_int_distribution<unsigned> n_id_dist(0, 1023);
    std::uniform_int_distribution<unsigned> nof_harq_ack_dist(0, 2);
    std::uniform_int_distribution<uint8_t>  ports_dist(0, 255);
    std::uniform_int_distribution<unsigned> initial_cyclic_shift_dist(0, 11);
    std::uniform_int_distribution<unsigned> time_domain_occ_dist(0, 6);

    config.cp           = (bool_dist(rgen) == 0) ? cyclic_prefix::NORMAL : cyclic_prefix::EXTENDED;
    unsigned numerology = (config.cp == cyclic_prefix::EXTENDED) ? 2 : num_dist(rgen);
    unsigned slot       = slot_dist(rgen) % slot_point(numerology, 0).nof_slots_per_system_frame();
    config.slot         = slot_point(numerology, slot);
    config.bwp_size_rb  = bwp_size_dist(rgen);
    config.starting_prb = std::min(starting_prb_dist(rgen), config.bwp_size_rb - 1);
    if (bool_dist(rgen)) {
      config.second_hop_prb.emplace((config.starting_prb + config.bwp_size_rb / 2) % config.bwp_size_rb);
    } else {
      config.second_hop_prb = {};
    }
    config.n_id                 = n_id_dist(rgen);
    config.nof_harq_ack         = nof_harq_ack_dist(rgen);
    config.ports                = {ports_dist(rgen)};
    config.initial_cyclic_shift = initial_cyclic_shift_dist(rgen);
    config.time_domain_occ      = time_domain_occ_dist(rgen);

    std::uniform_int_distribution<unsigned> nof_symbols_dist(4, get_nsymb_per_slot(config.cp));
    config.nof_symbols = nof_symbols_dist(rgen);

    std::uniform_int_distribution<unsigned> bwp_start_dist(0, MAX_RB - config.bwp_size_rb);
    config.bwp_start_rb = bwp_start_dist(rgen);

    std::uniform_int_distribution<unsigned> start_symbol_index_dist(0,
                                                                    get_nsymb_per_slot(config.cp) - config.nof_symbols);
    config.start_symbol_index = start_symbol_index_dist(rgen);

    return config;
  }

  std::unique_ptr<pucch_processor>     processor;
  std::unique_ptr<pucch_pdu_validator> validator;
  dmrs_pucch_processor_spy*            dmrs_spy;
  pucch_detector_spy*                  detector_spy;
};

std::shared_ptr<pucch_processor_factory>          PucchProcessorFormat1Fixture::processor_factory    = nullptr;
std::shared_ptr<dmrs_pucch_estimator_factory_spy> PucchProcessorFormat1Fixture::dmrs_factory_spy     = nullptr;
std::shared_ptr<pucch_detector_factory_spy>       PucchProcessorFormat1Fixture::detector_factory_spy = nullptr;

TEST_P(PucchProcessorFormat1Fixture, UnitTest)
{
  // Generate random configuration with valid values.
  pucch_processor::format1_configuration config = GetConfig();

  // Prepare resource grid.
  resource_grid_reader_spy grid;

  // Make sure configuration is valid.
  ASSERT_TRUE(validator->is_valid(config));

  // Process.
  const pucch_processor_result result = processor->process(grid, config);

  // Verify channel estimator configuration.
  ASSERT_EQ(dmrs_spy->get_entries().size(), 1);
  const dmrs_pucch_processor_spy::entry_t& dmrs_entry = dmrs_spy->get_entries().front();
  ASSERT_EQ(dmrs_entry.config.format, pucch_format::FORMAT_1);
  ASSERT_EQ(dmrs_entry.config.slot, config.slot);
  ASSERT_EQ(dmrs_entry.config.cp, config.cp);
  ASSERT_EQ(dmrs_entry.config.group_hopping, pucch_group_hopping::NEITHER);
  ASSERT_EQ(dmrs_entry.config.start_symbol_index, config.start_symbol_index);
  ASSERT_EQ(dmrs_entry.config.nof_symbols, config.nof_symbols);
  ASSERT_EQ(dmrs_entry.config.starting_prb, config.starting_prb + config.bwp_start_rb);
  ASSERT_EQ(dmrs_entry.config.intra_slot_hopping, config.second_hop_prb.has_value());
  if (config.second_hop_prb.has_value()) {
    ASSERT_EQ(dmrs_entry.config.second_hop_prb, config.second_hop_prb.value() + config.bwp_start_rb);
  }
  ASSERT_EQ(dmrs_entry.config.nof_prb, 0);
  ASSERT_EQ(dmrs_entry.config.initial_cyclic_shift, config.initial_cyclic_shift);
  ASSERT_EQ(dmrs_entry.config.time_domain_occ, config.time_domain_occ);
  ASSERT_EQ(dmrs_entry.config.additional_dmrs, false);
  ASSERT_EQ(dmrs_entry.config.n_id, config.n_id);
  ASSERT_EQ(dmrs_entry.config.n_id_0, 0);
  ASSERT_EQ(span<const uint8_t>(dmrs_entry.config.ports), span<const uint8_t>(config.ports));

  // Verify PUCCH detector.
  ASSERT_EQ(detector_spy->get_entries_format1().size(), 1);
  const pucch_detector_spy::entry_format1& detector_entry = detector_spy->get_entries_format1().front();
  ASSERT_EQ(detector_entry.config.slot, config.slot);
  ASSERT_EQ(detector_entry.config.cp, config.cp);
  ASSERT_EQ(detector_entry.config.starting_prb, config.starting_prb + config.bwp_start_rb);
  if (config.second_hop_prb.has_value()) {
    ASSERT_TRUE(detector_entry.config.second_hop_prb.has_value());
    ASSERT_EQ(detector_entry.config.second_hop_prb.value(), config.second_hop_prb.value() + config.bwp_start_rb);
  } else {
    ASSERT_FALSE(detector_entry.config.second_hop_prb.has_value());
  }
  ASSERT_EQ(detector_entry.config.start_symbol_index, config.start_symbol_index);
  ASSERT_EQ(detector_entry.config.nof_symbols, config.nof_symbols);
  ASSERT_EQ(detector_entry.config.group_hopping, pucch_group_hopping::NEITHER);
  ASSERT_EQ(detector_entry.config.port, config.ports.front());
  ASSERT_EQ(detector_entry.config.beta_pucch, 1.0F);
  ASSERT_EQ(detector_entry.config.time_domain_occ, config.time_domain_occ);
  ASSERT_EQ(detector_entry.config.initial_cyclic_shift, config.initial_cyclic_shift);
  ASSERT_EQ(detector_entry.config.n_id, config.n_id);
  ASSERT_EQ(detector_entry.config.nof_harq_ack, config.nof_harq_ack);

  // Validate UCI message.
  ASSERT_EQ(result.message.get_status(), detector_entry.msg.get_status());
  ASSERT_EQ(result.message.get_full_payload().size(), config.nof_harq_ack);
  ASSERT_EQ(result.message.get_harq_ack_bits().size(), config.nof_harq_ack);
  ASSERT_EQ(result.message.get_harq_ack_bits(), detector_entry.msg.get_harq_ack_bits());
  ASSERT_EQ(result.message.get_csi_part1_bits().size(), 0);
  ASSERT_EQ(result.message.get_csi_part2_bits().size(), 0);
}

INSTANTIATE_TEST_SUITE_P(PucchProcessorFormat1, PucchProcessorFormat1Fixture, ::testing::Range(0U, nof_repetitions));

} // namespace
