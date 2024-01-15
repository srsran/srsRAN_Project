/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

const unsigned NOF_RBS = 52;

///////   Test allocation of dedicated PUCCH resources    ///////

class test_pucch_allocator_ded_resources : public ::testing::Test
{
public:
  test_pucch_allocator_ded_resources(unsigned max_pucchs_per_slot_ = 32U, unsigned max_ul_grants_per_slot_ = 32U) :
    t_bench(test_bench_params{}, max_pucchs_per_slot_, max_ul_grants_per_slot_)
  {
    // Set expected grant for PUCCH Format 1 SR.
    pucch_expected_f1_sr.format  = pucch_format::FORMAT_1;
    pucch_expected_f1_sr.crnti   = to_rnti(0x4601);
    pucch_expected_f1_sr.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_sr.resources.prbs            = prb_interval{NOF_RBS - 3, NOF_RBS - 2};
    pucch_expected_f1_sr.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f1_sr.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected_f1_sr.format_1.initial_cyclic_shift = 0;
    pucch_expected_f1_sr.format_1.sr_bits              = sr_nof_bits::one;
    pucch_expected_f1_sr.format_1.harq_ack_nof_bits    = 0;
    pucch_expected_f1_sr.format_1.time_domain_occ      = 0;

    pucch_expected_f1_sr.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected_f1_sr.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected_f1_sr.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    // Set expected grant for PUCCH Format 1 HARQ.
    pucch_expected_f1_harq.format  = srsran::pucch_format::FORMAT_1;
    pucch_expected_f1_harq.crnti   = to_rnti(0x4601);
    pucch_expected_f1_harq.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_harq.resources.prbs            = prb_interval{NOF_RBS - 1, NOF_RBS};
    pucch_expected_f1_harq.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f1_harq.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected_f1_harq.format_1.initial_cyclic_shift = 0;
    pucch_expected_f1_harq.format_1.sr_bits              = sr_nof_bits::no_sr;
    pucch_expected_f1_harq.format_1.harq_ack_nof_bits    = 1;
    pucch_expected_f1_harq.format_1.time_domain_occ      = 0;

    // Set expected grant for PUCCH Format 1.
    pucch_expected_f1_harq.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected_f1_harq.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected_f1_harq.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    pucch_expected_f2.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_f2.crnti                     = to_rnti(0x4601);
    pucch_expected_f2.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2.resources.prbs            = prb_interval{2, 3};
    pucch_expected_f2.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2.resources.symbols         = ofdm_symbol_range{0, 2};

    pucch_expected_f2.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_f2.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_f2.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    pucch_expected_csi.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_csi.crnti                     = to_rnti(0x4601);
    pucch_expected_csi.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi.resources.prbs            = prb_interval{2, 3};
    pucch_expected_csi.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_csi.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_csi.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_csi.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;
  };

protected:
  // Parameters that are passed by the routine to run the tests.
  test_bench     t_bench;
  pucch_info     pucch_expected_f1_sr;
  pucch_info     pucch_expected_f1_harq;
  pucch_info     pucch_expected_f2;
  pucch_info     pucch_expected_csi;
  const unsigned pucch_res_idx{0};

  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  optional<unsigned> add_format2_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    return t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  void add_csi_grant(unsigned csi_part1_bits = 4)
  {
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits);
  }

  void add_ue_with_harq_grant()
  {
    t_bench.add_ue();
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
  }

  void add_harq_f2_to_last_allocated_ue()
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

  void add_ue_with_format2_harq_grant()
  {
    t_bench.add_ue();
    add_harq_f2_to_last_allocated_ue();
  }

  void add_ue_with_sr_and_harq_f2()
  {
    t_bench.add_ue();
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.last_allocated_rnti,
                                                      t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg());
    add_harq_f2_to_last_allocated_ue();
  }

  void add_ue_with_csi_and_harq_f2()
  {
    unsigned csi_part1_bits = 4;
    t_bench.add_ue();
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.last_allocated_rnti,
                                                       t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                       csi_part1_bits);
    add_harq_f2_to_last_allocated_ue();
  }
};

class test_pucch_f2_alloc_several_prbs : public ::testing::Test
{
public:
  test_pucch_f2_alloc_several_prbs() : t_bench(test_bench_params{.pucch_f2_more_prbs = true})
  {
    // This PUCCH grant will be for 5 HARQ bits, which fit in 1 PRB.
    pucch_expected_harq_only.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_harq_only.crnti                     = to_rnti(0x4601);
    pucch_expected_harq_only.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_harq_only.resources.prbs            = prb_interval{2, 3};
    pucch_expected_harq_only.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_harq_only.resources.symbols         = ofdm_symbol_range{0, 2};

    pucch_expected_harq_only.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_harq_only.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_harq_only.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    // This PUCCH grant will be for 5 HARQ bits + 4 CSI bits, which fit in 2 PRBs.
    pucch_expected_harq_csi                = pucch_expected_harq_only;
    pucch_expected_harq_csi.resources.prbs = prb_interval{2, 4};

    // This PUCCH grant will be for 4 CSI bits only, which are endoced in the maximum number of PRBs.
    pucch_expected_csi_only.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_csi_only.crnti                     = to_rnti(0x4601);
    pucch_expected_csi_only.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi_only.resources.prbs            = prb_interval{2, 5};
    pucch_expected_csi_only.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi_only.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_csi_only.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_csi_only.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_csi_only.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;
  };

protected:
  // Parameters that are passed by the routine to run the tests.
  test_bench t_bench;
  pucch_info pucch_expected_harq_only;
  pucch_info pucch_expected_harq_csi;
  pucch_info pucch_expected_csi_only;

  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  optional<unsigned> add_format2_harq_grant()
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    return t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  void add_csi_grant(unsigned csi_part1_bits = 4)
  {
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits);
  }
};

/////////////// Tests PUCCH allocator for SR.

TEST_F(test_pucch_allocator_ded_resources, test_sr_allocation_only)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_sr, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_allocator_ded_resources, test_sr_allocation_when_no_free_sr_resources)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.add_ue();
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.last_allocated_rnti, t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg());
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, test_sr_alloc_over_common_harq_grant)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_indicator.has_value());
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_sr, slot_grid.result.ul.pucchs[1]));
}

/////////////// Tests PUCCH allocator for CSI.

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_only)
{
  const unsigned csi_part1_bits                 = 4;
  pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_csi.format_2.csi_part1_bits    = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_over_sr)
{
  const unsigned csi_part1_bits                 = 4;
  pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_csi.format_2.csi_part1_bits    = csi_part1_bits;

  add_sr_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_when_no_free_csi_resources)
{
  const unsigned csi_part1_bits                 = 4;
  pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_csi.format_2.csi_part1_bits    = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.add_ue();
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(slot_grid,
                                                     t_bench.last_allocated_rnti,
                                                     t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                     csi_part1_bits);
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // The last CSI allocation should have failed. Expect still the same nof of PUCCH PDUs.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_over_common_harq_grant)
{
  const unsigned csi_part1_bits                 = 4;
  pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_csi.format_2.csi_part1_bits    = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_indicator.has_value());
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 2 PUCCH PDU; the second one should be Format 2 for CSI only.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi, slot_grid.result.ul.pucchs[1]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[1].csi_rep_cfg.has_value());
}

///////  Test HARQ-ACK allocation on ded. resources  - Format 1   ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_allocation_only)
{
  auto&                    slot_grid                = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator);
  const unsigned EXPECTED_HARQ_GRANTS = 1;
  ASSERT_EQ(EXPECTED_HARQ_GRANTS, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_harq, slot_grid.result.ul.pucchs.back()));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_allocation_over_sr)
{
  add_sr_grant();
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_harq, slot_grid.result.ul.pucchs[1]));
  // Verify that the SR grants gets updated.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[0].format_1.sr_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2bits)
{
  add_harq_grant();
  pucch_expected_f1_harq.format_1.harq_ack_nof_bits = 2;
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_harq, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2bits_over_sr)
{
  add_sr_grant();
  add_harq_grant();
  pucch_expected_f1_harq.format_1.harq_ack_nof_bits = 2;
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1_harq, slot_grid.result.ul.pucchs[1]));
  const unsigned EXPECTED_HARQ_BITS_IN_SR = 2;
  ASSERT_EQ(EXPECTED_HARQ_BITS_IN_SR, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[0].format_1.sr_bits);
}

///////  Test HARQ-ACK allocation on ded. resources - Format 1  - Multi UEs ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2_ues)
{
  add_ue_with_harq_grant();
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  const unsigned EXPECTED_PUCCH_RES_IDX = 1;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(EXPECTED_PUCCH_RES_IDX, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3_ues)
{
  // Add 2 UEs, each with their own HARQ grant allocated
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // The 2 HARQ grants should belong to UE different from 0x4601.
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  // The main UE (0x4601) should be allocated as the last one.
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[2].crnti);
  const unsigned EXPECTED_PUCCH_RES_IDX = 2;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(EXPECTED_PUCCH_RES_IDX, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_4_ues)
{
  // Add 2 UEs, each with their own HARQ grant allocated
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  const optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Verify that the UE 0x4601 does not get allocated any HARQ grant.
  ASSERT_FALSE(test_pucch_res_indicator.has_value());
  // The 2 HARQ grants should belong to UE different from 0x4601.
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[2].crnti);
}

///////  Test HARQ-ACK allocation on ded. resources - Format 2   ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 2, as Format 1 can
  // carry 2 HARQ bits, at most.
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);

  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);

  // Convert to Format 2 and with 3 bits HARQ.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs[0].format_2.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_sr)
{
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = 0;

  add_sr_grant();
  add_harq_grant();
  add_harq_grant();
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_csi)
{
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = 4;

  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_sr_and_csi)
{
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = 4;

  add_sr_grant();
  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_4bits_over_sr_and_csi)
{
  pucch_expected_f2.format_2.harq_ack_nof_bits = 4;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = 3;

  add_sr_grant();
  // Only add 3 CSI bits, otherwise the payload isn't big enough for all UCI bits.
  add_csi_grant(/*csi_part1_bits*/ 3);
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_4bits_over_sr_and_csi_fails)
{
  add_sr_grant();
  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  // This should fail, as the PUCCH F2 payload for this configuration allows max 8 UCI bits.
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

///////   Test PUCCH Format 2 PRB configuration.    ///////

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_csi_only)
{
  pucch_expected_csi_only.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi_only.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_csi_only.format_2.csi_part1_bits    = 4;

  add_csi_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi_only, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_csi_sr)
{
  pucch_expected_csi_only.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi_only.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_csi_only.format_2.csi_part1_bits    = 4;

  add_sr_grant();
  add_csi_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi_only, slot_grid.result.ul.pucchs[0]));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_harq_only)
{
  pucch_expected_harq_only.format_2.harq_ack_nof_bits = 5;
  pucch_expected_harq_only.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_harq_only.format_2.csi_part1_bits    = 0;

  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_harq_only, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_harq_csi_only)
{
  pucch_expected_harq_csi.format_2.harq_ack_nof_bits = 5;
  pucch_expected_harq_csi.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_harq_csi.format_2.csi_part1_bits    = 4;

  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_harq_csi, slot_grid.result.ul.pucchs[0]));
}

///////  Test HARQ-ACK allocation on ded. resources - Format 2  - Multi UEs ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_f2_alloc_2_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_ue_with_format2_harq_grant();
  // 1 PDU expected, as many as the number of UEs.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  const optional<unsigned> test_pucch_res_indicator = add_format2_harq_grant();
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH res indicator is 1, as 0 was assigned to the first UE.
  ASSERT_EQ(1, test_pucch_res_indicator.value());
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_f2_alloc_multiple_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  // 5 PDU expected, as many as the number of UEs.
  ASSERT_EQ(5, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  const optional<unsigned> test_pucch_res_indicator = add_format2_harq_grant();
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator expected is 5 (the first 5 UEs got PUCCH res indicator 0,..,4).
  ASSERT_EQ(5, test_pucch_res_indicator.value());
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_f2_alloc_last_ue_not_allocated)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  const optional<unsigned> test_pucch_res_indicator = add_format2_harq_grant();
  // 7 PDU expected, as many as the number of UEs; of these, the first 6 are expected to be Format 2, the last one is
  // Format 1, as the allocation of the Format 2 for the last UE failed for lack of available PUCCH format 2 resources.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs.back().format);
  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_multiple_ues_and_csi_alloc)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate a CSI grant for UE 0x4601.
  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_sr_f2_alloc_multiple_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_sr_and_harq_f2();
  add_ue_with_sr_and_harq_f2();
  add_ue_with_sr_and_harq_f2();
  add_ue_with_sr_and_harq_f2();
  add_ue_with_sr_and_harq_f2();
  // 5 PDU expected, as many as the number of UEs.
  ASSERT_EQ(5, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  add_ue_with_sr_and_harq_f2();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs.back().format_2.sr_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_csi_f2_alloc_multiple_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_csi_and_harq_f2();
  add_ue_with_csi_and_harq_f2();
  add_ue_with_csi_and_harq_f2();
  add_ue_with_csi_and_harq_f2();
  add_ue_with_csi_and_harq_f2();
  // 5 PDU expected, as many as the number of UEs.
  ASSERT_EQ(5, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  add_ue_with_csi_and_harq_f2();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);
}

///////   Test removal of dedicated PUCCH resources    ///////

TEST_F(test_pucch_allocator_ded_resources, test_sr_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_sr_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(0, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(0, removed_bits.csi_part1_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(0, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(4, removed_bits.csi_part1_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_harq_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(0, removed_bits.csi_part1_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_sr_harq_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_sr_grant();
  add_harq_grant();
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(0, removed_bits.csi_part1_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_harq_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_csi_grant();
  add_harq_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(4, removed_bits.csi_part1_bits);
}

///////   Test allocation over different slots.    ///////

// Allocate multiple HARQ-ACK grants over the same target slot.
TEST_F(test_pucch_allocator_ded_resources, test_tdd_harq_allocation_over_time)
{
  // All the allocation allocate a HARQ-ACK grant at slot 5.
  // t_bench.sl_tx = 0; k0 = 0; k1 = 5  =>  t_bench.sl_tx + k0 + k1 = 5.
  unsigned k1          = 5;
  auto&    slot_grid_1 = t_bench.res_grid[t_bench.k0 + k1];

  // Allocate 1 HARQ at k1 = 5.
  optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_1.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid_1.result.ul.pucchs.back().format);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_2 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 4.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_2.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid_2.result.ul.pucchs.back().format);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 2; k0 = 0; k1 = 3  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_3 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 3. This will convert the PUCCH grant to Format 2.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_3.result.ul.pucchs.size());
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(0, test_pucch_res_indicator.value());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_3.result.ul.pucchs.back().format);
  ASSERT_EQ(3, slot_grid_3.result.ul.pucchs.back().format_2.harq_ack_nof_bits);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 3; k0 = 0; k1 = 2  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_4 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 2.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_4.result.ul.pucchs.size());
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(0, test_pucch_res_indicator.value());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_4.result.ul.pucchs.back().format);
  ASSERT_EQ(4, slot_grid_4.result.ul.pucchs.back().format_2.harq_ack_nof_bits);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 4; k0 = 0; k1 = 1  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_5 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 1.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_5.result.ul.pucchs.size());
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(0, test_pucch_res_indicator.value());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_5.result.ul.pucchs.back().format);
  ASSERT_EQ(5, slot_grid_5.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_for_private_fnc_retrieving_existing_grants)
{
  // All the allocation allocate a HARQ-ACK grant at slot 5.
  // t_bench.sl_tx = 0; k0 = 0; k1 = 7  =>  t_bench.sl_tx + k0 + k1 = 7.
  unsigned         k1         = 7;
  auto&            slot_grid  = t_bench.res_grid[t_bench.k0 + k1];
  const slot_point pucch_slot = slot_grid.slot;

  // Allocate 1 HARQ at k1 = 5.
  t_bench.add_ue();
  du_ue_index_t ue1_idx = t_bench.last_allocated_ue_idx;
  t_bench.add_ue();
  du_ue_index_t ue2_idx = t_bench.last_allocated_ue_idx;

  // NOTE: In the following, allocate first the PUCCH dedicated resource and then the common resource. This is to test
  // that the function retrieving the existing PUCCH resources does not mess up when with common resources when PUCCH
  // grants are removed from the scheduler output.

  // Allocate:
  // - 1 PUCCH ded with 1 HARQ-ACK bit to UE 1 (Format 1).
  // - 1 PUCCH common with 1 HARQ-ACK bit to UE 0 (Format 1).
  // - 1 PUCCH ded with 1 HARQ-ACK bit to UE 2 (Format 1).
  // The scheduler output should have 3 PUCCH resources.
  // - 1 PUCCH ded F1    - RNTI = UE1 - HARQ-BITS = 1.
  // - 1 PUCCH common F1 - RNTI = UE0 - HARQ-BITS = 1.
  // - 1 PUCCH ded F1    - RNTI = UE2 - HARQ-BITS = 1.
  optional<unsigned> pucch_res_ind_ue1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue1_idx).crnti, t_bench.get_ue(ue1_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue1.has_value());
  ASSERT_EQ(0, pucch_res_ind_ue1.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(t_bench.get_ue(ue1_idx).crnti, slot_grid.result.ul.pucchs[0].crnti);

  optional<unsigned> pucch_res_ind_ue0 = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_ind_ue0.has_value());
  ASSERT_EQ(0, pucch_res_ind_ue0.value());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[1].format);
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);

  optional<unsigned> pucch_res_ind_ue2 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue2_idx).crnti, t_bench.get_ue(ue2_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue2.has_value());
  ASSERT_EQ(1, pucch_res_ind_ue2.value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[2].format);
  ASSERT_EQ(t_bench.get_ue(ue1_idx).crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_EQ(t_bench.get_ue(ue2_idx).crnti, slot_grid.result.ul.pucchs[2].crnti);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[2].format_1.harq_ack_nof_bits);

  // Advance by 1 slot. Allocate:
  // - 1 PUCCH ded with 1 HARQ-ACK bit to UE 1 (Format 1).
  // The scheduler output should have 3 PUCCH resources.
  // - 1 PUCCH ded F1    - RNTI = UE1 - HARQ-BITS = 2.
  // - 1 PUCCH common F1 - RNTI = UE0 - HARQ-BITS = 1.
  // - 1 PUCCH ded F1    - RNTI = UE2 - HARQ-BITS = 1.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 5.
  --k1;
  ASSERT_EQ(pucch_slot, slot_grid.slot);

  pucch_res_ind_ue1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue1_idx).crnti, t_bench.get_ue(ue1_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue1.has_value());
  ASSERT_EQ(0, pucch_res_ind_ue1.value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(t_bench.get_ue(ue1_idx).crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_EQ(t_bench.get_ue(ue2_idx).crnti, slot_grid.result.ul.pucchs[2].crnti);
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[2].format_1.harq_ack_nof_bits);

  // Advance by 1 slot. Allocate:
  // - 1 PUCCH ded with 1 HARQ-ACK bit to UE 1 (Convert to Format 2).
  // NOTE: This change the other in which the PUCCH grants are stored in the scheduler output.
  // The scheduler output should have 3 PUCCH resources.
  // - 1 PUCCH common F1 - RNTI = UE0 - HARQ-BITS = 1.
  // - 1 PUCCH ded F1    - RNTI = UE2 - HARQ-BITS = 1.
  // - 1 PUCCH ded F2    - RNTI = UE1 - HARQ-BITS = 3.
  t_bench.slot_indication(++t_bench.sl_tx);
  --k1;
  ASSERT_EQ(pucch_slot, slot_grid.slot);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 5.
  //  auto& slot_grid_3 = t_bench.res_grid[t_bench.k0 + --k1];

  pucch_res_ind_ue1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue1_idx).crnti, t_bench.get_ue(ue1_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue1.has_value());
  ASSERT_EQ(0, pucch_res_ind_ue1.value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[1].format);
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[2].format);
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_EQ(t_bench.get_ue(ue2_idx).crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_EQ(t_bench.get_ue(ue1_idx).crnti, slot_grid.result.ul.pucchs[2].crnti);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs[2].format_1.harq_ack_nof_bits);

  // Advance by 1 slot. Allocate:
  // - 1 PUCCH ded with 1 HARQ-ACK bit to UE 2 (Multiplex on existing Format 1).
  // NOTE: This change the other in which the PUCCH grants are stored in the scheduler output.
  // The scheduler output should have 3 PUCCH resources.
  // - 1 PUCCH common F1 - RNTI = UE0 - HARQ-BITS = 1.
  // - 1 PUCCH ded F1    - RNTI = UE2 - HARQ-BITS = 2.
  // - 1 PUCCH ded F2    - RNTI = UE1 - HARQ-BITS = 3.
  t_bench.slot_indication(++t_bench.sl_tx);
  --k1;
  ASSERT_EQ(pucch_slot, slot_grid.slot);
  // t_bench.sl_tx = 2; k0 = 0; k1 = 3  =>  t_bench.sl_tx + k0 + k1 = 5.
  //  auto& slot_grid_4 = t_bench.res_grid[t_bench.k0 + --k1];

  pucch_res_ind_ue2 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue2_idx).crnti, t_bench.get_ue(ue2_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue2.has_value());
  ASSERT_EQ(1, pucch_res_ind_ue2.value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format);
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[1].format);
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[2].format);
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_EQ(t_bench.get_ue(ue2_idx).crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_EQ(t_bench.get_ue(ue1_idx).crnti, slot_grid.result.ul.pucchs[2].crnti);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs[2].format_1.harq_ack_nof_bits);
}

///////   Test UL grants reached and PUCCH fails.    ///////

class test_pucch_allocator_ded_resources_reached_ul_grants : public test_pucch_allocator_ded_resources
{
public:
  test_pucch_allocator_ded_resources_reached_ul_grants() : test_pucch_allocator_ded_resources(3U, 6U) {}
};

TEST_F(test_pucch_allocator_ded_resources_reached_ul_grants, test_max_pucch_allocation_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources_reached_ul_grants, test_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  slot_grid.result.ul.puschs.emplace_back();
  slot_grid.result.ul.puschs.emplace_back();
  slot_grid.result.ul.puschs.emplace_back();

  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());

  add_ue_with_harq_grant();
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources_reached_ul_grants, test_sr_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  for (unsigned n = 0; n != 6U; ++n) {
    slot_grid.result.ul.puschs.emplace_back();
  }

  add_sr_grant();
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources_reached_ul_grants, test_csi_max_ul_allocations_reached)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  for (unsigned n = 0; n != 6U; ++n) {
    slot_grid.result.ul.puschs.emplace_back();
  }

  add_csi_grant();
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
