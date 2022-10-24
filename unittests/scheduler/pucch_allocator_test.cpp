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

static expected_output_params make_expected_output_params_sr_only()
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

class test_pucch_harq_common_output : public ::testing::TestWithParam<pucch_test_parameters>
{
public:
  test_pucch_harq_common_output() :
    params{GetParam()},
    t_bench(params.pucch_input_params.pucch_res_common, params.pucch_input_params.n_cces),
    pucch_expected(build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    params.output_params.format,
                                    params.output_params.prbs,
                                    params.output_params.second_hop_prbs,
                                    params.output_params.symbols,
                                    params.output_params.initial_cyclic_shift,
                                    params.output_params.sr_bits,
                                    params.output_params.harq_ack_nof_bits,
                                    params.output_params.time_domain_occ)){};

protected:
  // Parameters that are passed by the routing to run the tests.
  pucch_test_parameters params;
  test_bench            t_bench;
  pucch_info            pucch_expected;
};

// Tests the output of the PUCCH allocator (or PUCCH PDU).
TEST_P(test_pucch_harq_common_output, test_pucch_output_info)
{
  pucch_harq_ack_grant pucch_test = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_TRUE(pucch_test.pucch_pdu != nullptr);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, *pucch_test.pucch_pdu));
}

// Tests whether PUCCH allocator fills the scheduler grid correctly.
TEST_P(test_pucch_harq_common_output, test_pucch_grid_filling)
{
  t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, t_bench.k1));
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_P(test_pucch_harq_common_output, test_pucch_output_for_dci)
{
  pucch_harq_ack_grant pucch_test = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  ASSERT_EQ(GetParam().dci_pucch_res_indicator, pucch_test.pucch_res_indicator);
}

INSTANTIATE_TEST_SUITE_P(
    test_pucch_output_for_dci,
    test_pucch_harq_common_output,
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

class test_pucch_sr_allocator : public ::testing::Test
{
public:
  test_pucch_sr_allocator() :
    sr_expected_params{make_expected_output_params_sr_only()},
    pucch_expected{build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    sr_expected_params.format,
                                    sr_expected_params.prbs,
                                    sr_expected_params.second_hop_prbs,
                                    sr_expected_params.symbols,
                                    sr_expected_params.initial_cyclic_shift,
                                    sr_expected_params.sr_bits,
                                    sr_expected_params.harq_ack_nof_bits,
                                    sr_expected_params.time_domain_occ)} {};

protected:
  // Parameters that are passed by the routing to run the tests.
  expected_output_params sr_expected_params;
  const unsigned         sl_point_harq_delay{0};
  test_bench             t_bench;
  pucch_info             pucch_expected;
};

// Tests whether PUCCH.
TEST_F(test_pucch_sr_allocator, test_pucch_sr_only_output_info)
{
  ASSERT_EQ(1, t_bench.get_ue().nof_cells());
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg());

  ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_F(test_pucch_sr_allocator, test_pucch_sr_only_grid_filled)
{
  ASSERT_EQ(1, t_bench.get_ue().nof_cells());
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg());

  ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, sl_point_harq_delay));
}

class test_pucch_sr_allocator_with_harq : public ::testing::Test
{
public:
  test_pucch_sr_allocator_with_harq() :
    sr_expected_params{make_expected_output_params_sr_harq()},
    pucch_expected{build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    sr_expected_params.format,
                                    sr_expected_params.prbs,
                                    sr_expected_params.second_hop_prbs,
                                    sr_expected_params.symbols,
                                    sr_expected_params.initial_cyclic_shift,
                                    sr_expected_params.sr_bits,
                                    sr_expected_params.harq_ack_nof_bits,
                                    sr_expected_params.time_domain_occ)} {};

protected:
  // Parameters that are passed by the routing to run the tests.
  expected_output_params sr_expected_params;
  const unsigned         sl_point_harq_delay{0};
  test_bench             t_bench;
  pucch_info             pucch_expected;
};

// Tests whether PUCCH.
TEST_F(test_pucch_sr_allocator_with_harq, test_pucch_sr_harq_output_info)
{
  ASSERT_EQ(1, t_bench.get_ue().nof_cells());
  t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.k0, sl_point_harq_delay, t_bench.dci_info);
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg());

  ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_F(test_pucch_sr_allocator_with_harq, test_pucch_sr_harq_grid_filled)
{
  ASSERT_EQ(1, t_bench.get_ue().nof_cells());
  t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue().crnti, t_bench.k0, sl_point_harq_delay, t_bench.dci_info);
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_ue().crnti, t_bench.get_ue().get_pcell().cfg());

  ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, sl_point_harq_delay));
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
