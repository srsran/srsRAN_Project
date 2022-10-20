/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/pucch_scheduling/pucch_scheduler_impl.h"
#include "pucch_test_utils.h"
#include "scheduler_test_suite.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_params {
  pucch_format      format;
  prb_interval      prbs;
  prb_interval      second_hop_prbs;
  ofdm_symbol_range symbols;
  bool              intra_slot_freq_hop;
  uint8_t           initial_cyclic_shift;
  sr_nof_bits       sr_bits;
  unsigned          harq_ack_nof_bits;
  uint8_t           time_domain_occ;
};

// Parameters for PUCCH scheduler initialization.
struct pucch_params {
  // Input parameters.
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_test_parameters {
  unsigned               dci_pucch_res_indicator;
  expected_output_params output_params;
  pucch_params           pucch_input_params;
};

class test_pucch_output : public ::testing::TestWithParam<pucch_test_parameters>
{
public:
  test_pucch_output() :
    params{GetParam()},
    t_bench(params.pucch_input_params.pucch_res_common, params.pucch_input_params.n_cces),
    pucch_output_builder{&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                         static_cast<unsigned>(t_bench.cell_cfg.pci),
                         params.output_params.format,
                         params.output_params.prbs,
                         params.output_params.second_hop_prbs,
                         params.output_params.symbols,
                         params.output_params.intra_slot_freq_hop,
                         params.output_params.initial_cyclic_shift,
                         params.output_params.sr_bits,
                         params.output_params.harq_ack_nof_bits,
                         params.output_params.time_domain_occ}
  {
    pucch_expected = pucch_output_builder.get_expected_pucch_output();
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  pucch_test_parameters     params;
  test_bench                t_bench;
  pucch_output_test_builder pucch_output_builder;
  pucch_info                pucch_expected;
  // Expected \c pucch_res_indicator value returned by the scheduler.
  unsigned pucch_res_indicator;
  // Expected \c harq_timing_indicator value returned by the scheduler.
  unsigned harq_timing_indicator;
};

// Tests the output of the PUCCH allocator (or PUCCH PDU).
TEST_P(test_pucch_output, test_pucch_output_info)
{
  pucch_harq_ack_grant pucch_test = t_bench.pucch_sched.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.user.crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_TRUE(pucch_test.pucch_pdu != nullptr);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, *pucch_test.pucch_pdu));
}

// Tests whether PUCCH allocator fills the scheduler grid correctly.
TEST_P(test_pucch_output, test_pucch_grid_filling)
{
  t_bench.pucch_sched.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.user.crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, t_bench.k1));
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_P(test_pucch_output, test_pucch_output_for_dci)
{
  pucch_harq_ack_grant pucch_test = t_bench.pucch_sched.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.user.crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_EQ(GetParam().dci_pucch_res_indicator, pucch_test.pucch_res_indicator);
}

INSTANTIATE_TEST_SUITE_P(
    test_pucch_output_for_dci,
    test_pucch_output,
    testing::Values(pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{0, 1},
                                                                                  prb_interval{51, 52},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{0, 1}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{3, 4},
                                                                                  prb_interval{48, 49},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{2, 1}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{3, 4},
                                                                                  prb_interval{48, 49},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  8,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{2, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{4, 5},
                                                                                  prb_interval{47, 48},
                                                                                  ofdm_symbol_range{4, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{10, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{4, 5},
                                                                                  prb_interval{47, 48},
                                                                                  ofdm_symbol_range{4, 14},
                                                                                  true,
                                                                                  6,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{10, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{0, 1},
                                                                                  prb_interval{51, 52},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{11, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{1, 2},
                                                                                  prb_interval{50, 51},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{11, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{13, 14},
                                                                                  prb_interval{38, 39},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{15, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator = 0,
                                          .output_params           = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{13, 14},
                                                                                  prb_interval{38, 39},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  6,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params      = pucch_params{15, 6}})

);

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
