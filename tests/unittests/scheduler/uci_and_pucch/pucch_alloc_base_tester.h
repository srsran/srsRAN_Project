/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "uci_test_utils.h"

using namespace srsran;

// Number of RBs configured for the test cases.
constexpr unsigned NOF_RBS = 52;

class pucch_allocator_base_tester
{
public:
  pucch_allocator_base_tester(test_bench_params params_ = test_bench_params{.pucch_res_common = 11, .n_cces = 1},
                              unsigned          max_pucchs_per_slot_    = 32U,
                              unsigned          max_ul_grants_per_slot_ = 32U) :
    t_bench(params_, max_pucchs_per_slot_, max_ul_grants_per_slot_)
  {
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  test_bench     t_bench;
  const unsigned pucch_res_idx{0};

  // Add a SR grant to the main UE.
  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg());
  }

  // Add a HARQ grant of PUCCH Formats 0/1 (1 bit) to the main UE.
  void add_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  // Add a HARQ grant of PUCCH Formats 2/3/4 (> 2 bits) to the main UE.
  std::optional<unsigned> add_format2_3_4_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    return t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  // Add a CSI grant to the main UE.
  void add_csi_grant(unsigned csi_part1_bits = 4)
  {
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits);
  }

  // Add an extra UE and allocate a HARQ grant to it.
  void add_ue_with_harq_grant()
  {
    t_bench.add_ue();
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
  }

  // Add a HARQ grant of PUCCH Formats 2/3/4 (> 2 bits) to the last allocated UE.
  void add_harq_f2_3_4_to_last_allocated_ue()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
  }

  // Add an extra UE and allocate a HARQ grant of PUCCH Formats 2/3/4 (> 2 bits) to it.
  void add_ue_with_format2_harq_grant()
  {
    t_bench.add_ue();
    add_harq_f2_3_4_to_last_allocated_ue();
  }

  // Add an extra UE and allocate a SR  HARQ grant of PUCCH Formats 2/3/4 (> 2 bits) to it.
  void add_ue_with_sr_and_harq_f2_3_4()
  {
    t_bench.add_ue();
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.last_allocated_rnti,
                                                      t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg());
    add_harq_f2_3_4_to_last_allocated_ue();
  }

  // Add an extra UE and allocate a CSI  HARQ grant of PUCCH Formats 2/3/4 (> 2 bits) to it.
  void add_ue_with_csi_and_harq_f2()
  {
    unsigned csi_part1_bits = 4;
    t_bench.add_ue();
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.last_allocated_rnti,
                                                       t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                       csi_part1_bits);
    add_harq_f2_3_4_to_last_allocated_ue();
  }
};
