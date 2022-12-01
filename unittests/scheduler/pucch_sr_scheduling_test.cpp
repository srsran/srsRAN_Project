/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_test_utils.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters (default values are for SR only).
struct expected_output_params {
  pucch_format      format{pucch_format::FORMAT_1};
  prb_interval      prbs{prb_interval{0, 1}};
  prb_interval      second_hop_prbs{prb_interval{51, 52}};
  ofdm_symbol_range symbols{ofdm_symbol_range{0, 14}};
  uint8_t           initial_cyclic_shift{0};
  sr_nof_bits       sr_bits{sr_nof_bits::one};
  unsigned          harq_ack_nof_bits{0};
  uint8_t           time_domain_occ{0};
};

// Make expected results parameters (default values are for SR only).
static expected_output_params make_expected_output_params_sr_harq()
{
  return expected_output_params{.harq_ack_nof_bits = 1};
}

// Class to test PUCCH schedule with SR occasions only.
class test_pucch_output_sr_only : public ::testing::TestWithParam<std::tuple<sr_periodicity, unsigned>>
{
public:
  test_pucch_output_sr_only() :
    t_bench{pucch_res_common, n_cces, std::get<0>(GetParam()), std::get<1>(GetParam())},
    pucch_expected(build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    expected_output_sr.format,
                                    expected_output_sr.prbs,
                                    expected_output_sr.second_hop_prbs,
                                    expected_output_sr.symbols,
                                    expected_output_sr.initial_cyclic_shift,
                                    expected_output_sr.sr_bits,
                                    expected_output_sr.harq_ack_nof_bits,
                                    expected_output_sr.time_domain_occ)){};

protected:
  // Parameters that are passed by the routing to run the tests.
  unsigned   pucch_res_common{11};
  unsigned   n_cces{0};
  test_bench t_bench;
  // This contains the list of output expected parameters for SR PUCCH PDU.
  expected_output_params expected_output_sr;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
};

// Class to test PUCCH schedule with SR occasions & HARQ-ACK.
class test_pucch_output_sr_harq : public ::testing::TestWithParam<std::tuple<sr_periodicity, unsigned>>
{
public:
  test_pucch_output_sr_harq() :
    t_bench{pucch_res_common, n_cces, std::get<0>(GetParam()), std::get<1>(GetParam())},
    expected_output_sr{make_expected_output_params_sr_harq()},
    pucch_expected(build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    expected_output_sr.format,
                                    expected_output_sr.prbs,
                                    expected_output_sr.second_hop_prbs,
                                    expected_output_sr.symbols,
                                    expected_output_sr.initial_cyclic_shift,
                                    expected_output_sr.sr_bits,
                                    expected_output_sr.harq_ack_nof_bits,
                                    expected_output_sr.time_domain_occ)){};

protected:
  // Parameters that are passed by the routing to run the tests.
  unsigned       pucch_res_common{11};
  unsigned       n_cces{0};
  const unsigned sl_point_harq_delay{0};
  test_bench     t_bench;
  // This contains the list of output expected parameters for SR PUCCH PDU.
  expected_output_params expected_output_sr;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
};

// Tests the PUCCH scheduler with SR opportunities only.
TEST_P(test_pucch_output_sr_only, test_pucch_sr_only)
{
  sr_periodicity sr_period         = std::get<0>(GetParam());
  unsigned       sr_offset         = std::get<1>(GetParam());
  const unsigned NOF_SLOTS_TO_TEST = 1921;

  // Skip test if sr_period <= sr_offset.
  if (sr_periodicity_to_slot(sr_period) > sr_offset) {
    for (; t_bench.sl_tx.to_uint() < NOF_SLOTS_TO_TEST; t_bench.slot_indication(++t_bench.sl_tx)) {
      // t_bench.pucch_alloc.slot_indication(t_bench.sl_tx);
      t_bench.pucch_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

      if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
        ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
        ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, 0));
      } else {
        ASSERT_TRUE(t_bench.res_grid[0].result.ul.pucchs.empty());
      }
    }
  }
}

// Tests the PUCCH scheduler with SR opportunities and HARQ-ACK.
TEST_P(test_pucch_output_sr_harq, test_pucch_sr_harq)
{
  sr_periodicity sr_period         = std::get<0>(GetParam());
  unsigned       sr_offset         = std::get<1>(GetParam());
  const unsigned NOF_SLOTS_TO_TEST = 1921;

  // Skip test if sr_period <= sr_offset.
  if (sr_periodicity_to_slot(sr_period) > sr_offset) {
    for (; t_bench.sl_tx.to_uint() < NOF_SLOTS_TO_TEST; t_bench.slot_indication(++t_bench.sl_tx)) {
      t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
          t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, sl_point_harq_delay, t_bench.dci_info);
      t_bench.pucch_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

      if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
        ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
        ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, sl_point_harq_delay));
      } else {
        ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
    test_pucch_output_for_dci,
    test_pucch_output_sr_only,
    testing::Combine(testing::Values(sr_periodicity::sl_1,
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
                                     sr_periodicity::sl_640),
                     testing::Values(0, 1, 2, 3, 4, 7, 13, 15, 19, 23, 39, 43, 79, 91, 159, 243, 319, 473, 639)));

INSTANTIATE_TEST_SUITE_P(
    test_pucch_output_for_dci,
    test_pucch_output_sr_harq,
    testing::Combine(testing::Values(sr_periodicity::sl_1,
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
                                     sr_periodicity::sl_640),
                     testing::Values(0, 1, 2, 3, 4, 7, 13, 15, 19, 23, 39, 43, 79, 91, 159, 243, 319, 473, 639)));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
