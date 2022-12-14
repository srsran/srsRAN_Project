/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/dmrs_helpers.h"
#include "lib/scheduler/support/pusch/mcs_tbs_calculator.h"
#include "lib/scheduler/support/tbs_calculator.h"
#include "lib/scheduler/ue_scheduling/ue_sch_pdu_builder.h"
#include "unittests/scheduler/test_utils/config_generators.h"
#include "srsgnb/ran/pusch/ulsch_info.h"
#include "srsgnb/ran/uci/uci_mapping.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

const unsigned NOF_BITS_PER_BYTE = 8U;

// Helper used by the linear_search_prbs_tbs() function.
// NOTE: This function is the same as defined in mcs_tbs_calculator.cpp, but duplicated here as it is a static function.
static ulsch_configuration build_ulsch_info(const pusch_config_params&   pusch_cfg,
                                            const ue_cell_configuration& ue_cell_cfg,
                                            unsigned                     tbs_bytes,
                                            sch_mcs_description          mcs_info,
                                            unsigned                     nof_prbs)
{
  ulsch_configuration ulsch_info{.tbs                = static_cast<units::bits>(tbs_bytes * 8U),
                                 .mcs_descr          = mcs_info,
                                 .nof_harq_ack_bits  = static_cast<units::bits>(pusch_cfg.nof_harq_ack_bits),
                                 .nof_csi_part1_bits = static_cast<units::bits>(pusch_cfg.nof_csi_part1_bits),
                                 .nof_csi_part2_bits = static_cast<units::bits>(pusch_cfg.nof_csi_part2_bits),
                                 .nof_rb             = nof_prbs,
                                 .start_symbol_index = pusch_cfg.symbols.start(),
                                 .nof_symbols        = static_cast<unsigned>(pusch_cfg.symbols.length()),
                                 .dmrs_type          = pusch_cfg.dmrs.config_type,
                                 .dmrs_symbol_mask   = pusch_cfg.dmrs.dmrs_symb_pos,
                                 .nof_cdm_groups_without_data =
                                     static_cast<unsigned>(pusch_cfg.dmrs.num_dmrs_cdm_grps_no_data),
                                 .nof_layers = pusch_cfg.nof_layers};

  ulsch_info.alpha_scaling = alpha_scaling_to_float(
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value().scaling);

  // HARQ-ACK beta offset.
  if (pusch_cfg.nof_harq_ack_bits == 0) {
    ulsch_info.beta_offset_harq_ack = 0;
  } else if (pusch_cfg.nof_harq_ack_bits < 3) {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_1.value());
  } else if (pusch_cfg.nof_harq_ack_bits < 12) {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_2.value());
  } else {
    ulsch_info.beta_offset_harq_ack =
        beta_harq_ack_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                       .ul_config.value()
                                                                                       .init_ul_bwp.pusch_cfg.value()
                                                                                       .uci_cfg.value()
                                                                                       .beta_offsets_cfg.value())
                                   .beta_offset_ack_idx_3.value());
  }

  // CSI part 1 beta offset.
  if (pusch_cfg.nof_csi_part1_bits == 0) {
    ulsch_info.beta_offset_csi_part1 = 0;
  } else if (pusch_cfg.nof_csi_part1_bits < 12) {
    ulsch_info.beta_offset_harq_ack =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_1.value());
  } else {
    ulsch_info.beta_offset_harq_ack =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_2.value());
  }

  // CSI part 2 beta offset.
  if (pusch_cfg.nof_csi_part2_bits == 0) {
    ulsch_info.beta_offset_csi_part1 = 0;
  } else if (pusch_cfg.nof_csi_part1_bits < 12) {
    ulsch_info.beta_offset_harq_ack =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_1.value());
  } else {
    ulsch_info.beta_offset_harq_ack =
        beta_csi_to_float(variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_cell_cfg.cfg_dedicated()
                                                                                  .ul_config.value()
                                                                                  .init_ul_bwp.pusch_cfg.value()
                                                                                  .uci_cfg.value()
                                                                                  .beta_offsets_cfg.value())
                              .beta_offset_csi_p1_idx_2.value());
  }

  return ulsch_info;
}

// Computes the MCS, TBS and required number of PRBs by a brute-force-like approach; this function will be used as a
// benchmark for the results of the compute_ul_mcs_tbs() function under test.
static optional<pusch_mcs_tbs_params> linear_search_prbs_tbs(const pusch_config_params&   pusch_cfg,
                                                             const ue_cell_configuration& ue_cell_cfg,
                                                             unsigned                     payload_size_bytes,
                                                             sch_mcs_index                starting_mcs,
                                                             unsigned                     max_nof_prbs)
{
  const double                   max_supported_code_rate = 0.95;
  const unsigned                 dmrs_prbs               = calculate_nof_dmrs_per_rb(pusch_cfg.dmrs);
  unsigned                       nof_symbols             = pusch_cfg.symbols.length();
  optional<pusch_mcs_tbs_params> output;

  // > Find number of PRBs, first.
  unsigned tbs{0}, n_prbs{0};
  do {
    ++n_prbs;
    sch_mcs_description mcs_info =
        pusch_mcs_get_config(pusch_cfg.mcs_table, starting_mcs, pusch_cfg.tp_pi2bpsk_present);

    // Compute the TBS.
    tbs = tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                                .nof_dmrs_prb     = dmrs_prbs,
                                                                .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                                .mcs_descr        = mcs_info,
                                                                .nof_layers       = pusch_cfg.nof_layers,
                                                                .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                                .n_prb            = n_prbs}) /
          NOF_BITS_PER_BYTE;
  } while (tbs < payload_size_bytes and n_prbs < max_nof_prbs);

  // > Compute the effective code rate.
  sch_mcs_description mcs_info  = pusch_mcs_get_config(pusch_cfg.mcs_table, starting_mcs, pusch_cfg.tp_pi2bpsk_present);
  ulsch_configuration ulsch_cfg = build_ulsch_info(pusch_cfg, ue_cell_cfg, tbs, mcs_info, n_prbs);
  float               effective_code_rate = get_ulsch_information(ulsch_cfg).get_effective_code_rate();

  // > Return TBS, MCS and PRBs if the effective code rate is not above the threshold.
  if (effective_code_rate <= max_supported_code_rate) {
    return output.emplace(pusch_mcs_tbs_params{.mcs = starting_mcs, .tbs = tbs, .n_prbs = n_prbs});
  }

  // > Try with MCS = starting_MCS, starting_MCS - 1, starting_MCS -2, ..., 0 until the effective code rate is above the
  // threshold.
  for (unsigned mcs = starting_mcs.to_uint(); mcs > 0;) {
    mcs_info = pusch_mcs_get_config(pusch_cfg.mcs_table, --mcs, pusch_cfg.tp_pi2bpsk_present);

    // Compute the TBS.
    tbs = tbs_calculator_calculate(tbs_calculator_configuration{.nof_symb_sh      = nof_symbols,
                                                                .nof_dmrs_prb     = dmrs_prbs,
                                                                .nof_oh_prb       = pusch_cfg.nof_oh_prb,
                                                                .mcs_descr        = mcs_info,
                                                                .nof_layers       = pusch_cfg.nof_layers,
                                                                .tb_scaling_field = pusch_cfg.tb_scaling_field,
                                                                .n_prb            = n_prbs}) /
          NOF_BITS_PER_BYTE;

    // Return TBS, MCS and PRBs if the effective code rate is not above the threshold.
    ulsch_cfg = build_ulsch_info(pusch_cfg, ue_cell_cfg, tbs, mcs_info, n_prbs);
    if (get_ulsch_information(ulsch_cfg).get_effective_code_rate() <= max_supported_code_rate) {
      return output.emplace(pusch_mcs_tbs_params{.mcs = mcs, .tbs = tbs, .n_prbs = n_prbs});
    }
  }

  return output;
}

// Contains the input test parameters.
struct mcs_test_entry {
  unsigned payload_size_bytes;
  unsigned starting_mcs;
  unsigned max_nof_prbs;
};

class mcs_tbs_calculator_rnd_test_bench : public ::testing::TestWithParam<mcs_test_entry>
{
public:
  mcs_tbs_calculator_rnd_test_bench() :
    cell_cfg(test_helpers::make_default_sched_cell_configuration_request()),
    ue_cell_cfg(cell_cfg, config_helpers::create_default_initial_ue_serving_cell_config()),
    time_resource{0},
    pusch_cfg(get_pusch_config_f0_0_tc_rnti(cell_cfg, time_resource)){};

  mcs_test_entry make_random_test_entry()
  {
    return mcs_test_entry{.payload_size_bytes = test_rgen::uniform_int<unsigned>(min_payload_size, max_payload_size),
                          .starting_mcs       = test_rgen::uniform_int<unsigned>(0, max_mcs),
                          .max_nof_prbs       = test_rgen::uniform_int<unsigned>(1, max_prbs)};
  }

protected:
  const cell_configuration    cell_cfg;
  const ue_cell_configuration ue_cell_cfg;
  unsigned                    time_resource;
  pusch_config_params         pusch_cfg;

  // Parameters used to generate the test entry with random parameters.
  const unsigned max_payload_size{554400};
  const unsigned min_payload_size{1};
  const unsigned max_prbs{275};
  const unsigned max_mcs{28};
};

TEST_F(mcs_tbs_calculator_rnd_test_bench, compare_with_lin_search)
{
  const unsigned num_of_tests = 10000;
  for (unsigned n = 0; n != num_of_tests; ++n) {
    mcs_test_entry test_entry = make_random_test_entry();

    // Run test function.
    optional<pusch_mcs_tbs_params> test = compute_ul_mcs_tbs(pusch_cfg,
                                                             ue_cell_cfg,
                                                             test_entry.payload_size_bytes,
                                                             sch_mcs_index(test_entry.starting_mcs),
                                                             test_entry.max_nof_prbs);

    // Run benchmark function.
    optional<pusch_mcs_tbs_params> benchmark = linear_search_prbs_tbs(pusch_cfg,
                                                                      ue_cell_cfg,
                                                                      test_entry.payload_size_bytes,
                                                                      sch_mcs_index(test_entry.starting_mcs),
                                                                      test_entry.max_nof_prbs);

    // Compare expected output with function under test.
    ASSERT_EQ(benchmark.has_value(), test.has_value());
    if (test.has_value()) {
      ASSERT_EQ(benchmark.value().mcs.to_uint(), test.value().mcs.to_uint());
      ASSERT_EQ(benchmark.value().tbs, test.value().tbs);
      ASSERT_EQ(benchmark.value().n_prbs, test.value().n_prbs);
    }
  }
}
