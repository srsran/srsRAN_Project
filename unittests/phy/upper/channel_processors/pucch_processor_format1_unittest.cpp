/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../signal_processors/dmrs_pucch_processor_test_doubles.h"
#include "pucch_detector_test_doubles.h"
#include "pucch_processor_format1_test_data.h"
#include "srsgnb/srsvec/compare.h"
#include <gtest/gtest.h>

using namespace srsgnb;

static constexpr unsigned nof_repetitions = 100;

namespace {

class PucchProcessorFormat1Fixture : public ::testing::TestWithParam<unsigned>
{
protected:
  void SetUp() override
  {
    // Create spy factories.
    std::shared_ptr<dmrs_pucch_estimator_factory_spy> dmrs_factory_spy =
        std::make_shared<dmrs_pucch_estimator_factory_spy>();
    std::shared_ptr<pucch_detector_factory_spy> detector_factory_spy = std::make_shared<pucch_detector_factory_spy>();

    // Prepare Channel dimensions.
    channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
    channel_estimate_dimensions.nof_tx_layers = 1;
    channel_estimate_dimensions.nof_rx_ports  = 1;
    channel_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    channel_estimate_dimensions.nof_prb       = MAX_RB;

    // Create PUCCH processor factory.
    std::shared_ptr<pucch_processor_factory> factory =
        create_pucch_processor_factory_sw(dmrs_factory_spy, detector_factory_spy, channel_estimate_dimensions);
    ASSERT_NE(factory, nullptr);

    // Create PUCCH processor.
    processor = factory->create();
    ASSERT_NE(processor, nullptr);

    // Select spies.
    dmrs_spy     = dmrs_factory_spy->get_entries().front();
    detector_spy = detector_factory_spy->get_entries().front();
  }

  pucch_processor::format1_configuration getParam()
  {
    std::mt19937                           rgen = std::mt19937(GetParam());
    pucch_processor::format1_configuration config;

    std::uniform_int_distribution<unsigned> num_dist(0, static_cast<unsigned>(subcarrier_spacing::kHz240));
    std::uniform_int_distribution<unsigned> slot_dist(0, 160 * 1024 - 1);
    std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
    std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
    std::uniform_int_distribution<unsigned> bool_dist(0, 1);
    std::uniform_int_distribution<unsigned> starting_prb_dist(0, 274);
    std::uniform_int_distribution<unsigned> n_id_dist(0, 1023);
    std::uniform_int_distribution<unsigned> nof_harq_ack_dist(0, 2);
    std::uniform_int_distribution<uint8_t>  ports_dist(0, 255);
    std::uniform_int_distribution<unsigned> initial_cyclic_shift_dist(0, 11);
    std::uniform_int_distribution<unsigned> nof_symbols_dist(4, 14);
    std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 10);
    std::uniform_int_distribution<unsigned> time_domain_occ_dist(0, 6);

    unsigned numerology = num_dist(rgen);
    unsigned slot       = slot_dist(rgen) % slot_point(numerology, 0).nof_slots_per_system_frame();
    config.slot         = slot_point(numerology, slot);
    config.bwp_size_rb  = bwp_size_dist(rgen);
    config.bwp_start_rb = bwp_start_dist(rgen);
    config.cp           = bool_dist(rgen) == 0 ? cyclic_prefix::NORMAL : cyclic_prefix::EXTENDED;
    config.starting_prb = std::min(starting_prb_dist(rgen), config.bwp_size_rb - 1);
    if (bool_dist(rgen)) {
      config.second_hop_prb.emplace((config.starting_prb + config.bwp_size_rb / 2) % config.bwp_size_rb);
    } else {
      config.second_hop_prb = {};
    }
    config.n_id                 = n_id_dist(rgen);
    config.nof_harq_ack         = n_id_dist(rgen);
    config.ports                = {ports_dist(rgen)};
    config.initial_cyclic_shift = initial_cyclic_shift_dist(rgen);
    config.nof_symbols          = nof_symbols_dist(rgen);
    config.start_symbol_index   = start_symbol_index_dist(rgen);
    config.time_domain_occ      = time_domain_occ_dist(rgen);

    return config;
  }

  std::unique_ptr<pucch_processor> processor;
  dmrs_pucch_processor_spy*        dmrs_spy;
  pucch_detector_spy*              detector_spy;
};

TEST_P(PucchProcessorFormat1Fixture, UnitTest)
{
  // Generate random configuration with valid values.
  pucch_processor::format1_configuration config = getParam();

  // Prepare resource grid.
  resource_grid_reader_spy grid;

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
