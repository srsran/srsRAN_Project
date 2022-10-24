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
#include <random>

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
struct pucch_harq_params {
  // Input parameters.
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Parameters for PUCCH scheduler initialization.
struct pucch_params {
  // Input parameters.
  sr_periodicity sr_period;
  unsigned       sr_offset;
};

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_test_parameters {
  expected_output_params output_params;
  pucch_params           pucch_input_params;
  pucch_harq_params      harq_param;
};

class test_pucch_output_sr_only : public ::testing::TestWithParam<pucch_test_parameters>
{
public:
  test_pucch_output_sr_only() :
    params{GetParam()},
    t_bench(),
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

class test_pucch_output_sr_harq : public ::testing::TestWithParam<pucch_test_parameters>
{
public:
  test_pucch_output_sr_harq() :
    params{GetParam()},
    t_bench(),
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
TEST_P(test_pucch_output_sr_only, test_pucch_sr_only)
{
  for (; t_bench.sl_tx.to_uint() < 1000; t_bench.slot_indication(++t_bench.sl_tx)) {
    t_bench.pucch_alloc.slot_indication(t_bench.sl_tx);
    t_bench.pucch_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

    if (t_bench.sl_tx.to_uint() % 40 == 0) {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
      ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
      ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, 0));
    } else {
      ASSERT_TRUE(t_bench.res_grid[0].result.ul.pucchs.empty());
    }
  }
}

// Tests the output of the PUCCH allocator (or PUCCH PDU).
TEST_P(test_pucch_output_sr_harq, test_pucch_sr_harq)
{
  test_bench     t_bench{13, 0};
  const unsigned sl_point_harq_delay = 0;

  for (; t_bench.sl_tx.to_uint() < 1000; t_bench.slot_indication(++t_bench.sl_tx)) {
    t_bench.pucch_alloc.slot_indication(t_bench.sl_tx);
    t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.user.crnti, t_bench.k0, sl_point_harq_delay, t_bench.dci_info);
    t_bench.pucch_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

    if (t_bench.sl_tx.to_uint() % sr_periodicity_to_slot(srsgnb::sr_periodicity::sl_40) == 0) {
      ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
      ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
      ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, sl_point_harq_delay));
    } else {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_only,
                         testing::Values(pucch_test_parameters{
                             .output_params      = expected_output_params{pucch_format::FORMAT_1,
                                                                     prb_interval{0, 1},
                                                                     prb_interval{51, 52},
                                                                     ofdm_symbol_range{0, 14},
                                                                     true,
                                                                     0,
                                                                     sr_nof_bits::one,
                                                                     0,
                                                                     0},
                             .pucch_input_params = pucch_params{.sr_period = sr_periodicity::sl_40, .sr_offset = 0}}));

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_harq,
                         testing::Values(pucch_test_parameters{
                             .output_params      = expected_output_params{pucch_format::FORMAT_1,
                                                                     prb_interval{0, 1},
                                                                     prb_interval{51, 52},
                                                                     ofdm_symbol_range{0, 14},
                                                                     true,
                                                                     0,
                                                                     sr_nof_bits::one,
                                                                     1,
                                                                     0},
                             .pucch_input_params = pucch_params{.sr_period = sr_periodicity::sl_40, .sr_offset = 0}}));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
