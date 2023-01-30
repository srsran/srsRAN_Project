/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_test_utils.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

const unsigned NOF_RBS = 52;

// Class to test PUCCH schedule with SR occasions only.
class test_pucch_output_sr_only : public ::testing::TestWithParam<sr_periodicity>
{
public:
  test_pucch_output_sr_only() :
    sr_period(GetParam()),
    sr_offset(test_rgen::uniform_int<unsigned>(0, sr_periodicity_to_slot(GetParam()) - 1)),
    t_bench{test_bench_params{.pucch_res_common = pucch_res_common,
                              .n_cces           = n_cces,
                              .period           = sr_period,
                              .offset           = sr_offset}}
  {
    // Set expected grant for PUCCH Format 1.
    pucch_expected.format  = srsgnb::pucch_format::FORMAT_1;
    pucch_expected.crnti   = to_rnti(0x4601);
    pucch_expected.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected.resources.prbs            = prb_interval{0, 1};
    pucch_expected.resources.second_hop_prbs = prb_interval{NOF_RBS - 1, NOF_RBS};
    pucch_expected.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected.format_1.initial_cyclic_shift = 0;
    pucch_expected.format_1.sr_bits              = sr_nof_bits::one;
    pucch_expected.format_1.harq_ack_nof_bits    = 0;
    pucch_expected.format_1.time_domain_occ      = 0;

    pucch_expected.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected.format                   = pucch_format::FORMAT_1;
    pucch_expected.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  sr_periodicity sr_period;
  unsigned       sr_offset;
  unsigned       pucch_res_common{11};
  unsigned       n_cces{0};
  test_bench     t_bench;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
};

// Class to test PUCCH schedule with SR occasions & HARQ-ACK.
class test_pucch_output_sr_harq : public ::testing::TestWithParam<sr_periodicity>
{
public:
  test_pucch_output_sr_harq() :
    sr_period(GetParam()),
    sr_offset(test_rgen::uniform_int<unsigned>(0, sr_periodicity_to_slot(GetParam()) - 1)),
    t_bench{test_bench_params{.pucch_res_common = pucch_res_common,
                              .n_cces           = n_cces,
                              .period           = sr_period,
                              .offset           = sr_offset}}
  {
    // Set expected grant for PUCCH Format 1.
    pucch_expected.format  = srsgnb::pucch_format::FORMAT_1;
    pucch_expected.crnti   = to_rnti(0x4601);
    pucch_expected.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected.resources.prbs            = prb_interval{0, 1};
    pucch_expected.resources.second_hop_prbs = prb_interval{NOF_RBS - 1, NOF_RBS};
    pucch_expected.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected.format_1.initial_cyclic_shift = 0;
    pucch_expected.format_1.sr_bits              = sr_nof_bits::one;
    pucch_expected.format_1.harq_ack_nof_bits    = 1;
    pucch_expected.format_1.time_domain_occ      = 0;

    pucch_expected.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected.format                   = pucch_format::FORMAT_1;
    pucch_expected.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  sr_periodicity sr_period;
  unsigned       sr_offset;
  unsigned       pucch_res_common{11};
  unsigned       n_cces{0};
  const unsigned sl_point_harq_delay{0};
  test_bench     t_bench;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
};

// Tests the PUCCH scheduler with SR opportunities only.
TEST_P(test_pucch_output_sr_only, test_pucch_sr_only)
{
  const unsigned NOF_SLOTS_TO_TEST = sr_periodicity_to_slot(sr_period) * 4;

  for (; t_bench.sl_tx.to_uint() < NOF_SLOTS_TO_TEST; t_bench.slot_indication(++t_bench.sl_tx)) {
    // t_bench.pucch_alloc.slot_indication(t_bench.sl_tx);
    t_bench.uci_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

    if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
      ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
    } else {
      ASSERT_TRUE(t_bench.res_grid[0].result.ul.pucchs.empty());
    }
  }
}

// Tests the PUCCH scheduler with SR opportunities and HARQ-ACK.
TEST_P(test_pucch_output_sr_harq, test_pucch_sr_harq)
{
  const unsigned NOF_SLOTS_TO_TEST = sr_periodicity_to_slot(sr_period) * 4;

  for (; t_bench.sl_tx.to_uint() < NOF_SLOTS_TO_TEST; t_bench.slot_indication(++t_bench.sl_tx)) {
    t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, sl_point_harq_delay, t_bench.dci_info);
    t_bench.uci_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

    if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
      ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
      ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
    } else {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_only,
                         testing::Values(sr_periodicity::sl_1,
                                         sr_periodicity::sl_2,
                                         sr_periodicity::sl_4,
                                         sr_periodicity::sl_5,
                                         sr_periodicity::sl_8,
                                         sr_periodicity::sl_16,
                                         sr_periodicity::sl_20,
                                         sr_periodicity::sl_40,
                                         sr_periodicity::sl_80,
                                         sr_periodicity::sl_160,
                                         sr_periodicity::sl_320,
                                         sr_periodicity::sl_640));

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_harq,
                         testing::Values(sr_periodicity::sl_1,
                                         sr_periodicity::sl_2,
                                         sr_periodicity::sl_4,
                                         sr_periodicity::sl_5,
                                         sr_periodicity::sl_8,
                                         sr_periodicity::sl_16,
                                         sr_periodicity::sl_20,
                                         sr_periodicity::sl_40,
                                         sr_periodicity::sl_80,
                                         sr_periodicity::sl_160,
                                         sr_periodicity::sl_320,
                                         sr_periodicity::sl_640));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
