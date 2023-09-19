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

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

const unsigned NOF_RBS = 52;

namespace pucch_sr_test {
// Expected results parameters.
struct expected_output_params_f1 {
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

// Expected results parameters.
struct expected_output_params_f2 {
  pucch_format      format;
  prb_interval      prbs;
  prb_interval      second_hop_prbs;
  ofdm_symbol_range symbols;
  bool              intra_slot_freq_hop;
  sr_nof_bits       sr_bits;
  unsigned          harq_ack_nof_bits;
  unsigned          csi_part1_nof_bits;
};

// Parameters for PUCCH scheduler initialization.
struct pucch_params {
  // Input parameters.
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_test_parameters {
  unsigned                  dci_pucch_res_indicator;
  expected_output_params_f1 output_params;
  pucch_params              pucch_input_params;
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const pucch_test_parameters& value, ::std::ostream* os)
{
  return;
}

static expected_output_params_f1 make_expected_output_params_sr_only()
{
  return expected_output_params_f1{.format               = pucch_format::FORMAT_1,
                                   .prbs                 = prb_interval{NOF_RBS - 3, NOF_RBS - 2},
                                   .second_hop_prbs      = prb_interval{0, 0},
                                   .symbols              = ofdm_symbol_range{0, 14},
                                   .intra_slot_freq_hop  = false,
                                   .initial_cyclic_shift = 0,
                                   .sr_bits              = sr_nof_bits::one,
                                   .harq_ack_nof_bits    = 0,
                                   .time_domain_occ      = 0};
}

// Make expected results parameters (default values are for SR only).
expected_output_params_f1 make_expected_output_params_sr_harq()
{
  expected_output_params_f1 out = make_expected_output_params_sr_only();
  out.harq_ack_nof_bits         = 1;
  return out;
}
} // namespace pucch_sr_test

using namespace pucch_sr_test;

///////   Test allocation of PUCCH SR resources    ///////

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
  expected_output_params_f1 sr_expected_params;
  const unsigned            sl_point_harq_delay{0};
  test_bench                t_bench;
  pucch_info                pucch_expected;
};

// Tests whether PUCCH.
TEST_F(test_pucch_sr_allocator, test_pucch_sr_only_output_info)
{
  ASSERT_EQ(1, t_bench.get_main_ue().nof_cells());
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
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
  expected_output_params_f1 sr_expected_params;
  const unsigned            sl_point_harq_delay{0};
  test_bench                t_bench;
  pucch_info                pucch_expected;
};

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_F(test_pucch_sr_allocator_with_harq, test_pucch_sr_harq_grid)
{
  ASSERT_EQ(1, t_bench.get_main_ue().nof_cells());
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                  t_bench.get_main_ue().crnti,
                                                  t_bench.get_main_ue().get_pcell().cfg(),
                                                  t_bench.k0,
                                                  sl_point_harq_delay);
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_F(test_pucch_sr_allocator_with_harq, test_pucch_sr_with_harq_format2)
{
  ASSERT_EQ(1, t_bench.get_main_ue().nof_cells());
  // Call the function 3 to allocate 3 HARQ bits (this forces the allocator to convert Format-1 grant into Format-2).
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                  t_bench.get_main_ue().crnti,
                                                  t_bench.get_main_ue().get_pcell().cfg(),
                                                  t_bench.k0,
                                                  sl_point_harq_delay);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                  t_bench.get_main_ue().crnti,
                                                  t_bench.get_main_ue().get_pcell().cfg(),
                                                  t_bench.k0,
                                                  sl_point_harq_delay);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                  t_bench.get_main_ue().crnti,
                                                  t_bench.get_main_ue().get_pcell().cfg(),
                                                  t_bench.k0,
                                                  sl_point_harq_delay);

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[0], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[0].result.ul.pucchs.front().format);
  ASSERT_EQ(3, t_bench.res_grid[0].result.ul.pucchs.front().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[0].result.ul.pucchs.front().format_2.sr_bits);
}
