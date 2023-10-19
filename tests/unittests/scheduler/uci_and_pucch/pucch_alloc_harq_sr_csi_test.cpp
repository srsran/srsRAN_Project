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

const unsigned NOF_RBS = 52;

///////   Test allocation of dedicated PUCCH resources    ///////

class test_pucch_harq_allocator_ded_resources : public ::testing::Test
{
public:
  test_pucch_harq_allocator_ded_resources()
  {
    // Set expected grant for PUCCH Format 1.
    pucch_expected_f1.format  = srsran::pucch_format::FORMAT_1;
    pucch_expected_f1.crnti   = to_rnti(0x4601);
    pucch_expected_f1.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1.resources.prbs            = prb_interval{NOF_RBS - 1, NOF_RBS};
    pucch_expected_f1.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f1.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected_f1.format_1.initial_cyclic_shift = 0;
    pucch_expected_f1.format_1.sr_bits              = sr_nof_bits::no_sr;
    pucch_expected_f1.format_1.harq_ack_nof_bits    = 1;
    pucch_expected_f1.format_1.time_domain_occ      = 0;

    // Set expected grant for PUCCH Format 1.
    pucch_expected_f1.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected_f1.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected_f1.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

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
  pucch_info     pucch_expected_f1;
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

  pucch_harq_ack_grant add_format2_harq_grant()
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

  void add_ue_with_format2_harq_grant()
  {
    t_bench.add_ue();
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

  void add_ue_with_csi()
  {
    unsigned csi_part1_bits = 4;
    t_bench.add_ue();
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.last_allocated_rnti,
                                                       t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                       csi_part1_bits);
  }
};

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_sched_output)
{
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH PDU info.
TEST_F(test_pucch_harq_allocator_ded_resources, test_pucch_pdu)
{
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto&          slot_grid            = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  const unsigned EXPECTED_HARQ_GRANTS = 1;
  ASSERT_EQ(EXPECTED_HARQ_GRANTS, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1, slot_grid.result.ul.pucchs.back()));
}

// Tests whether PUCCH HARQ grant is allocated with an existing SR.
TEST_F(test_pucch_harq_allocator_ded_resources, test_update_sr_and_add_new_harq)
{
  add_sr_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[1]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1, slot_grid.result.ul.pucchs[1]));
  // Verify that the SR grants gets updated.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq)
{
  add_harq_grant();
  pucch_expected_f1.format_1.harq_ack_nof_bits = 2;
  pucch_harq_ack_grant test_pdu                = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1, slot_grid.result.ul.pucchs[0]));
}

// Tests whether existing PUCCH HARQ grant gets updated with an existing SR.
TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq_and_sr)
{
  add_harq_grant();
  pucch_expected_f1.format_1.harq_ack_nof_bits = 2;
  add_sr_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  // The function add_harq_grant() has set the expected harq_ack_nof_bits = 2.
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f1, slot_grid.result.ul.pucchs[0]));
  // Verify that the SR grants gets updated.
  const unsigned EXPECTED_HARQ_BITS_IN_SR = 2;
  ASSERT_EQ(EXPECTED_HARQ_BITS_IN_SR, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
}

// Tests whether allocator grants PUCCH HARQ for a second UE when another UE's PUCCH grant has already been allocated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_2_ues)
{
  add_ue_with_harq_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  const unsigned EXPECTED_PUCCH_RES_IDX = 1;
  ASSERT_EQ(EXPECTED_PUCCH_RES_IDX, test_pdu.pucch_res_indicator);
}

// Tests whether allocator grants PUCCH HARQ for a second UE when another UE's PUCCH grant has been allocated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_3_ues)
{
  // Add 2 UEs, each with their own HARQ grant allocated
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // The 2 HARQ grants should belong to UE different from 0x4601.
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  // The main UE (0x4601) should be allocated as the last one.
  ASSERT_EQ(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[2].crnti);
  const unsigned EXPECTED_PUCCH_RES_IDX = 2;
  ASSERT_EQ(EXPECTED_PUCCH_RES_IDX, test_pdu.pucch_res_indicator);
}

// Tests whether allocator grants PUCCH HARQ for a second UE when another UE's PUCCH grant has been allocated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_4_ues)
{
  // Add 2 UEs, each with their own HARQ grant allocated
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Verify that the UE 0x4601 does not get allocated any HARQ grant.
  ASSERT_EQ(nullptr, test_pdu.pucch_pdu);
  // The 2 HARQ grants should belong to UE different from 0x4601.
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[2].crnti);
}

///////   Test removal of dedicated PUCCH resources    ///////

TEST_F(test_pucch_harq_allocator_ded_resources, test_sr_removal)
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

TEST_F(test_pucch_harq_allocator_ded_resources, test_harq_removal)
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

TEST_F(test_pucch_harq_allocator_ded_resources, test_sr_harq_removal)
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

///////   Test allocation of format 2 PUCCH resources    ///////

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_alloction)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 2, as Format 1 can
  // carry 2 HARQ bits, at most.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, test_pdu.pucch_pdu->format);
  ASSERT_EQ(1, test_pdu.pucch_pdu->format_1.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);

  // 2 bits HARQ.
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, test_pdu.pucch_pdu->format);
  ASSERT_EQ(2, test_pdu.pucch_pdu->format_1.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);

  // Convert to Format 2 and with 3 bits HARQ.
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(3, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_alloction_pdu_content)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 2, as Format 1 can
  // carry 2 HARQ bits, at most.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs.back());
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = srsran::sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = 0;
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs.back()));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_removal)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 2, as Format 1 can
  // carry 2 HARQ bits, at most.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  pucch_uci_bits removed_bits = t_bench.pucch_alloc.remove_ue_uci_from_pucch(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(3, removed_bits.harq_ack_nof_bits);
  ASSERT_EQ(0, removed_bits.csi_part1_bits);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_sr_allocation_on_format2)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 2, as Format 1 can
  // carry 2 HARQ bits, at most.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(srsran::sr_nof_bits::no_sr, test_pdu.pucch_pdu->format_2.sr_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[t_bench.k0 + t_bench.k1], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(srsran::sr_nof_bits::one, test_pdu.pucch_pdu->format_2.sr_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
}

/////////////// Tests PUCCH allocator for CSI.

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi)
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
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f1_harq)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_harq_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f1_harq_2_ues)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  // Add a second UE and allocate a PUCCH resource (it'll get assigned a resource with PUCCH resource ind. = 0).
  t_bench.add_ue();
  const pucch_harq_ack_grant ue_2_res =
      t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                      t_bench.last_allocated_rnti,
                                                      t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                      t_bench.k0,
                                                      t_bench.k1);
  ASSERT_EQ(0, ue_2_res.pucch_res_indicator);

  // Allocate UE 1 a PUCCH resource (it'll get assigned a resource with PUCCH resource ind. = 1).
  const pucch_harq_ack_grant ue_1_res = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  const unsigned ue_1_pucch_f1_res_indicator = 1;
  ASSERT_EQ(ue_1_pucch_f1_res_indicator, ue_1_res.pucch_res_indicator);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  // In the following, we need to verify that the PUCCH F2 for CSI preserves the PUCCH resource indicator of the
  // previously allocated F1 resource. However, as the PUCCH resource indicator is hidden inside some private function,
  // we can't verify this straight away. What we do, instead, we verify that the configuration of the PUCCH grant
  // corresponds to the PUCCH F2 resource with PUCCH indicator 1.
  const pucch_config& pucch_cfg =
      t_bench.get_main_ue().get_pcell().cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const unsigned pucch_harq_f2_res_set_id = 1;
  const auto&    pucch_f2_res_id =
      pucch_cfg.pucch_res_set[pucch_harq_f2_res_set_id].pucch_res_id_list[ue_1_pucch_f1_res_indicator];
  const auto* res_cfg =
      std::find_if(pucch_cfg.pucch_res_list.begin(),
                   pucch_cfg.pucch_res_list.end(),
                   [pucch_f2_res_id](const pucch_resource& res) { return res.res_id == pucch_f2_res_id; });
  ASSERT_NE(pucch_cfg.pucch_res_list.end(), res_cfg);
  const pucch_resource& expected_res_cfg = *res_cfg;
  const auto&           pucch_csi_grant  = slot_grid.result.ul.pucchs[1];
  ASSERT_EQ(expected_res_cfg.format, pucch_csi_grant.format);
  ASSERT_EQ(expected_res_cfg.starting_prb, pucch_csi_grant.resources.prbs.start());
  ASSERT_EQ(variant_get<pucch_format_2_3_cfg>(expected_res_cfg.format_params).starting_sym_idx,
            pucch_csi_grant.resources.symbols.start());
  ASSERT_EQ(variant_get<pucch_format_2_3_cfg>(expected_res_cfg.format_params).nof_symbols,
            pucch_csi_grant.resources.symbols.length());
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f1_sr)
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
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f1_sr_and_harq)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_harq_grant();
  add_sr_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f2)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_format2_harq_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_csi_over_f2_harq_sr)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 3;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_format2_harq_grant();
  add_sr_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_harq_over_csi)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::no_sr;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_csi_grant();
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_harq_grant();

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_sr_over_csi)
{
  const unsigned csi_part1_bits                 = 4;
  pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
  pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_csi.format_2.csi_part1_bits    = csi_part1_bits;

  add_csi_grant();
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[t_bench.k0 + t_bench.k1], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_csi, slot_grid.result.ul.pucchs[0]));
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_allocate_harq_sr_over_csi)
{
  const unsigned csi_part1_bits                = 4;
  pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
  pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
  pucch_expected_f2.format_2.csi_part1_bits    = csi_part1_bits;

  add_csi_grant();
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      t_bench.res_grid[t_bench.k0 + t_bench.k1], t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, slot_grid.result.ul.pucchs[0]));
}

///////   Test allocation of format 2 PUCCH resources    ///////

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_alloction_2_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_ue_with_format2_harq_grant();
  // 1 PDU expected, as many as the number of UEs.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 1 for UE 0x4601.
  pucch_harq_ack_grant test_pdu = add_format2_harq_grant();
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(3, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  // PUCCH resource indicator expected is UE index - 1.
  ASSERT_EQ(1, test_pdu.pucch_res_indicator);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_alloction_multiple_ues)
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
  pucch_harq_ack_grant test_pdu = add_format2_harq_grant();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(3, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  // PUCCH resource indicator expected is UE index - 1.
  ASSERT_EQ(5, test_pdu.pucch_res_indicator);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_format_2_alloc_last_ue_not_allocated)
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
  pucch_harq_ack_grant test_pdu = add_format2_harq_grant();
  // 7 PDU expected, as many as the number of UEs; of these, the first 6 are expected to be Format 2, the last one is
  // Format 1, as the allocation of the Format 2 for the last UE failed for lack of available PUCCH format 2 resources.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(nullptr, test_pdu.pucch_pdu);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_multiple_ues_and_csi_alloc)
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

  // Allocate a CSI grant for UE 0x4601.
  add_csi_grant();
  // 7 PDU expected, as many as the number of UEs; of these, the first 6 are expected to be Format 2, the last one is
  // Format 1, as the allocation of the Format 2 for the last UE failed for lack of available PUCCH format 2 resources.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format);
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_csi_alloc_for_2_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_csi();
  // 1 PDU expected, as many as the number of UEs.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  add_csi_grant();
  // The CSI should not get allocated, as the previous UE is holding the only CSI resource.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

///////   Test PUCCH resource indicator with conversion F1 to F2.    ///////

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_res_indicator_with_format_conversion)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Add and allocate a UE in order to occupy PUCCH res with resource indicator 0.
  add_ue_with_harq_grant();
  // Allocate PUCCH F1 to UE 0x4601; this resource should have resource indicator 1.
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  // Allocate CSI to UE 0x4601; this will trigger the conversion to F2.
  add_csi_grant();
  // Allocate extra HARQ to UE 0x4601; this will preserve resource indicator 1.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator.
TEST_F(test_pucch_harq_allocator_ded_resources, test_res_indicator_harq_after_csi)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Add and allocate a UE in order to occupy PUCCH res with resource indicator 0.
  add_ue_with_harq_grant();
  // Allocate 2 UE as to occupy PUCCH F2 res with resource indicator 0 and 1.
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();

  // Allocate PUCCH F1 to UE 0x4601; this resource should have resource indicator 1.
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  // 4 PDU expected, as many as the number of UEs.
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.size());

  // Attempt to allocate a CSI for UE 0x4601. This should not be allocated, as the PUCCH Resource Format 2 with resource
  // indicator 1 is already used by another UE.
  add_csi_grant();
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.size());
  // Allocate 2 extra HARQ to UE 0x4601; this trigger conversion to F2 and assign a new Resource Indicator.
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_NE(nullptr, test_pdu.pucch_pdu);
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(2, test_pdu.pucch_res_indicator);
}

///////   Test allocation over different slots.    ///////

// Allocate multiple HARQ-ACK grants over the same target slot.
TEST_F(test_pucch_harq_allocator_ded_resources, test_tdd_harq_allocation_over_time)
{
  // All the allocation allocate a HARQ-ACK grant at slot 5.
  // t_bench.sl_tx = 0; k0 = 0; k1 = 5  =>  t_bench.sl_tx + k0 + k1 = 5.
  unsigned k1          = 5;
  auto&    slot_grid_1 = t_bench.res_grid[t_bench.k0 + k1];

  // Allocate 1 HARQ at k1 = 5.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_1.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, test_pdu.pucch_pdu->format);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_2 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 4.
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_2.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, test_pdu.pucch_pdu->format);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 2; k0 = 0; k1 = 3  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_3 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 3. This will convert the PUCCH grant to Format 2.
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_3.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid_3.result.ul.pucchs.back());
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(3, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 3; k0 = 0; k1 = 2  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_4 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 2.
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_4.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid_4.result.ul.pucchs.back());
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(4, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 4; k0 = 0; k1 = 1  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_5 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 1.
  test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_5.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid_5.result.ul.pucchs.back());
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
  ASSERT_EQ(pucch_format::FORMAT_2, test_pdu.pucch_pdu->format);
  ASSERT_EQ(5, test_pdu.pucch_pdu->format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, test_pdu.pucch_res_indicator);
}

///////   Test allocation over of PUCCH resource when exceeding payload.    ///////

// Allocate multiple HARQ-ACK grants and verify that the allocator fails when it reaches the maximum payload.
TEST_F(test_pucch_harq_allocator_ded_resources, test_harq_ack_alloc_with_csi_fails_when_exceeding_payload)
{
  const unsigned csi_offset = srsran::variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                  t_bench.get_main_ue()
                                      .get_pcell()
                                      .cfg()
                                      .cfg_dedicated()
                                      .csi_meas_cfg.value()
                                      .csi_report_cfg_list.front()
                                      .report_cfg_type)
                                  .report_slot_offset;

  // Advance slot until we reach the slot_td + k0 + k1 = csi_offset.
  const unsigned k1 = 4;
  while (t_bench.res_grid[t_bench.k0 + k1].slot.to_uint() < csi_offset) {
    t_bench.slot_indication(++t_bench.sl_tx);
  }

  // Expect the first HARQ-ACK grant allocation to succeed.
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_NE(nullptr, test_pdu.pucch_pdu);
  ASSERT_EQ(1, test_pdu.pucch_pdu->format_1.harq_ack_nof_bits);

  // Expect the second HARQ-ACK grant allocation to succeed (it should be the same grant as the previous one, with one
  // extra HARQ-ACK bit).
  pucch_harq_ack_grant test_pdu_2nd_alloc = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(test_pdu.pucch_pdu, test_pdu_2nd_alloc.pucch_pdu);
  ASSERT_EQ(2, test_pdu_2nd_alloc.pucch_pdu->format_1.harq_ack_nof_bits);

  // Expect the third HARQ-ACK grant allocation to succeed (it should be a new Format 2 grant).
  pucch_harq_ack_grant test_pdu_3rd_alloc = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_NE(nullptr, test_pdu_3rd_alloc.pucch_pdu);
  ASSERT_EQ(3, test_pdu_3rd_alloc.pucch_pdu->format_2.harq_ack_nof_bits);

  // Expect the fourth HARQ-ACK grant allocation to succeed (it should be the same grant as the previous one, with one
  // extra HARQ-ACK bit).
  pucch_harq_ack_grant test_pdu_4th_alloc = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(test_pdu_3rd_alloc.pucch_pdu, test_pdu_4th_alloc.pucch_pdu);
  ASSERT_EQ(4, test_pdu_4th_alloc.pucch_pdu->format_2.harq_ack_nof_bits);

  // Expect the fifth HARQ-ACK grant allocation to fail, as it falls on a CSI slot (4 bits) and the max payload has been
  // reached.
  pucch_harq_ack_grant test_pdu_5th_alloc = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(nullptr, test_pdu_5th_alloc.pucch_pdu);
}

TEST_F(test_pucch_harq_allocator_ded_resources, test_harq_ack_alloc_with_sr_fails_when_exceeding_payload)
{
  const pucch_config& pucch_cfg =
      t_bench.get_main_ue().get_pcell().cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Advance slot until we reach the slot_td + k0 + k1 = sr_offset. As the SR offset is 0, we add a SR period, otherwise
  // the while loop won't start.
  const unsigned k1 = 4;
  while (t_bench.res_grid[t_bench.k0 + k1].slot.to_uint() <
         sr_periodicity_to_slot(pucch_cfg.sr_res_list.front().period) + pucch_cfg.sr_res_list.front().offset) {
    t_bench.slot_indication(++t_bench.sl_tx);
  }

  // Expect the first 7 HARQ-ACK grant allocation to succeed.
  for (unsigned alloc_cnt = 0; alloc_cnt != 7; ++alloc_cnt) {
    pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
    ASSERT_NE(nullptr, test_pdu.pucch_pdu);
  }

  // Expect the 8th HARQ-ACK grant allocation to fail, as it falls on a SR slot (1 bit) and the max payload has been
  // reached.
  pucch_harq_ack_grant test_pdu_8th_alloc = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(nullptr, test_pdu_8th_alloc.pucch_pdu);
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
