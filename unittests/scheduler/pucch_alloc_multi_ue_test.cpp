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

expected_output_params make_expected_output_params_sr_only()
{
  return expected_output_params{.format               = pucch_format::FORMAT_1,
                                .prbs                 = prb_interval{0, 1},
                                .second_hop_prbs      = prb_interval{51, 52},
                                .symbols              = ofdm_symbol_range{0, 14},
                                .intra_slot_freq_hop  = true,
                                .initial_cyclic_shift = 0,
                                .sr_bits              = sr_nof_bits::one,
                                .harq_ack_nof_bits    = 0,
                                .time_domain_occ      = 0};
}

// Make expected results parameters (default values are for SR only).
expected_output_params make_expected_output_params_sr_harq()
{
  expected_output_params out = make_expected_output_params_sr_only();
  out.harq_ack_nof_bits      = 1;
  return out;
}

static expected_output_params make_expected_output_params_harq_ded_resources()
{
  return expected_output_params{.format               = pucch_format::FORMAT_1,
                                .prbs                 = prb_interval{51, 52},
                                .second_hop_prbs      = prb_interval{0, 1},
                                .symbols              = ofdm_symbol_range{0, 14},
                                .intra_slot_freq_hop  = true,
                                .initial_cyclic_shift = 0,
                                .sr_bits              = sr_nof_bits::no_sr,
                                .harq_ack_nof_bits    = 1,
                                .time_domain_occ      = 0};
}

class test_pucch_harq_allocator_ded_resources : public ::testing::Test
{
public:
  test_pucch_harq_allocator_ded_resources() :
    harq_expected_params{make_expected_output_params_harq_ded_resources()},
    pucch_expected{build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    harq_expected_params.format,
                                    harq_expected_params.prbs,
                                    harq_expected_params.second_hop_prbs,
                                    harq_expected_params.symbols,
                                    harq_expected_params.initial_cyclic_shift,
                                    harq_expected_params.sr_bits,
                                    harq_expected_params.harq_ack_nof_bits,
                                    harq_expected_params.time_domain_occ)} {};

protected:
  // Parameters that are passed by the routing to run the tests.
  expected_output_params harq_expected_params;
  const unsigned         sl_point_harq_delay{0};
  test_bench             t_bench;
  pucch_info             pucch_expected;
  const unsigned         pucch_res_idx{0};

  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
        t_bench.res_grid[t_bench.k0 + t_bench.k1], t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg());
  }

  void add_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

    pucch_expected.format_1.harq_ack_nof_bits = 2;
  }
};

// Tests whether PUCCH.
TEST_F(test_pucch_harq_allocator_ded_resources, test_sched_output)
{
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
}

// Tests whether PUCCH.
TEST_F(test_pucch_harq_allocator_ded_resources, test_pucch_pdu)
{
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs.back()));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_update_sr_and_add_new_harq)
{
  add_sr_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[1]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[1]));
  // Verify that the SR grants gets updated.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq)
{
  add_harq_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  // The function add_harq_grant() has set the expected harq_ack_nof_bits = 2.
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq_and_sr)
{
  add_harq_grant();
  add_sr_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  // The function add_harq_grant() has set the expected harq_ack_nof_bits = 2.
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[0]));
  // Verify that the SR grants gets updated.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
