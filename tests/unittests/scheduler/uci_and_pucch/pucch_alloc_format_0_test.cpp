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

#include "../test_utils/scheduler_test_suite.h"
#include "pucch_alloc_base_tester.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

///////   Test PUCCH Format 0.    ///////

class test_pucch_allocator_format_0 : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_format_0() :
    pucch_allocator_base_tester(test_bench_params{.pucch_res_common = 0, .use_format_0 = true}),
    pucch_res_idx_f0_for_sr(t_bench.get_main_ue()
                                .get_pcell()
                                .cfg()
                                .init_bwp()
                                .ul_ded->pucch_cfg.value()
                                .pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_0)]
                                .pucch_res_id_list.size() -
                            1U),
    pucch_res_idx_f0_for_csi(t_bench.get_main_ue()
                                 .get_pcell()
                                 .cfg()
                                 .init_bwp()
                                 .ul_ded->pucch_cfg.value()
                                 .pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_0)]
                                 .pucch_res_id_list.size() -
                             2U),
    pucch_res_idx_f2_for_sr(t_bench.get_main_ue()
                                .get_pcell()
                                .cfg()
                                .init_bwp()
                                .ul_ded->pucch_cfg.value()
                                .pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_1)]
                                .pucch_res_id_list.size() -
                            1U),
    pucch_res_idx_f2_for_csi(t_bench.get_main_ue()
                                 .get_pcell()
                                 .cfg()
                                 .init_bwp()
                                 .ul_ded->pucch_cfg.value()
                                 .pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_1)]
                                 .pucch_res_id_list.size() -
                             2U)
  {
    // Set expected grant for PUCCH Format 0 SR.
    auto& format0_sr             = pucch_expected_f0_sr.format_params.emplace<pucch_format_0>();
    pucch_expected_f0_sr.crnti   = to_rnti(0x4601);
    pucch_expected_f0_sr.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f0_sr.resources.prbs             = prb_interval{0, 1};
    pucch_expected_f0_sr.resources.second_hop_prbs  = prb_interval{0, 0};
    pucch_expected_f0_sr.resources.symbols          = ofdm_symbol_range{12, 14};
    pucch_expected_f0_sr.uci_bits.sr_bits           = sr_nof_bits::one;
    pucch_expected_f0_sr.uci_bits.harq_ack_nof_bits = 0;

    format0_sr.initial_cyclic_shift = 0;

    format0_sr.group_hopping = pucch_group_hopping::NEITHER;
    format0_sr.n_id_hopping  = t_bench.cell_cfg.pci;

    // Set expected grant for PUCCH Format 0 HARQ.
    auto& format0_harq             = pucch_expected_f0_harq.format_params.emplace<pucch_format_0>();
    pucch_expected_f0_harq.crnti   = to_rnti(0x4601);
    pucch_expected_f0_harq.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f0_harq.resources.prbs             = prb_interval{0, 1};
    pucch_expected_f0_harq.resources.second_hop_prbs  = prb_interval{0, 0};
    pucch_expected_f0_harq.resources.symbols          = ofdm_symbol_range{0, 2};
    pucch_expected_f0_harq.uci_bits.sr_bits           = sr_nof_bits::no_sr;
    pucch_expected_f0_harq.uci_bits.harq_ack_nof_bits = 1;

    format0_harq.initial_cyclic_shift = 0;

    format0_harq.group_hopping = pucch_group_hopping::NEITHER;
    format0_harq.n_id_hopping  = t_bench.cell_cfg.pci;

    auto& format2                               = pucch_expected_f2.format_params.emplace<pucch_format_2>();
    pucch_expected_f2.crnti                     = to_rnti(0x4601);
    pucch_expected_f2.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2.resources.prbs            = prb_interval{1, 2};
    pucch_expected_f2.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2.resources.symbols         = ofdm_symbol_range{0, 2};

    format2.max_code_rate     = max_pucch_code_rate::dot_25;
    format2.n_id_scambling    = t_bench.cell_cfg.pci;
    format2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    // This PUCCH resource is located on the same symbols and PRBs as the PUCCH Format 0 resource for SR.
    auto& format2_sr                      = pucch_expected_f2_harq_for_sr.format_params.emplace<pucch_format_2>();
    pucch_expected_f2_harq_for_sr.crnti   = to_rnti(0x4601);
    pucch_expected_f2_harq_for_sr.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2_harq_for_sr.resources.prbs            = prb_interval{0, 1};
    pucch_expected_f2_harq_for_sr.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2_harq_for_sr.resources.symbols         = ofdm_symbol_range{12, 14};

    format2_sr.max_code_rate     = max_pucch_code_rate::dot_25;
    format2_sr.n_id_scambling    = t_bench.cell_cfg.pci;
    format2_sr.n_id_0_scrambling = t_bench.cell_cfg.pci;

    auto& format2_csi                            = pucch_expected_csi.format_params.emplace<pucch_format_2>();
    pucch_expected_csi.crnti                     = to_rnti(0x4601);
    pucch_expected_csi.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi.resources.prbs            = prb_interval{1, 2};
    pucch_expected_csi.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi.resources.symbols         = ofdm_symbol_range{12, 14};

    format2_csi.max_code_rate     = max_pucch_code_rate::dot_25;
    format2_csi.n_id_scambling    = t_bench.cell_cfg.pci;
    format2_csi.n_id_0_scrambling = t_bench.cell_cfg.pci;
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info     pucch_expected_f0_sr;
  pucch_info     pucch_expected_f0_harq;
  pucch_info     pucch_expected_f2_harq_for_sr;
  pucch_info     pucch_expected_f2;
  pucch_info     pucch_expected_csi;
  const unsigned pucch_res_idx_f0_for_sr;
  const unsigned pucch_res_idx_f0_for_csi;
  const unsigned pucch_res_idx_f2_for_sr;
  const unsigned pucch_res_idx_f2_for_csi;
};

TEST_F(test_pucch_allocator_format_0, test_sr_allocation_only)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f0_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_only)
{
  auto&                         slot_grid                = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f0_harq](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_2_bits)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  const auto first_alloc = slot_grid.result.ul.pucchs.front();

  const std::optional<unsigned> test_pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_f0_harq.uci_bits.harq_ack_nof_bits = 2U;
  ASSERT_TRUE(test_pucch_res_indicator_1.has_value());
  // PUCCH resource indicator after the second allocation should not have changed.
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator_1);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f0_harq](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));

  const auto second_alloc = slot_grid.result.ul.pucchs.front();
  ASSERT_EQ(first_alloc.resources.prbs, second_alloc.resources.prbs);
  ASSERT_EQ(first_alloc.resources.symbols, second_alloc.resources.symbols);
  ASSERT_EQ(first_alloc.resources.second_hop_prbs, second_alloc.resources.second_hop_prbs);
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_over_sr)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  // According to the multiplexing procedure defined by TS 38.213, Section 9.2.5, the resource to use to report 1
  // HARQ-ACK bit + 1 SR bit is the HARQ-ACK resource. However, to circumvent the lack of capability of some UES (that
  // cannot transmit more than 1 PUCCH), we set last resource of PUCCH resource set 0 to be the SR resource and the UE
  // will use this.
  pucch_expected_f0_sr.uci_bits.harq_ack_nof_bits = 1U;
  pucch_expected_f0_sr.uci_bits.sr_bits           = sr_nof_bits::one;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f0_for_sr, test_pucch_res_indicator);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f0_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_2_bits_over_sr)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f0_for_sr, test_pucch_res_indicator);

  const auto first_alloc = slot_grid.result.ul.pucchs.front();

  const std::optional<unsigned> test_pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_f0_sr.uci_bits.harq_ack_nof_bits = 2U;
  pucch_expected_f0_sr.uci_bits.sr_bits           = sr_nof_bits::one;
  ASSERT_TRUE(test_pucch_res_indicator_1.has_value());
  ASSERT_EQ(pucch_res_idx_f0_for_sr, test_pucch_res_indicator_1);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f0_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));

  // Make sure the resource used for the second HARQ-ACK allocation uses the same PRBs and symbols as the first
  // allocation.
  const auto second_alloc = slot_grid.result.ul.pucchs.front();
  ASSERT_EQ(first_alloc.resources.prbs, second_alloc.resources.prbs);
  ASSERT_EQ(first_alloc.resources.symbols, second_alloc.resources.symbols);
  ASSERT_EQ(first_alloc.resources.second_hop_prbs, second_alloc.resources.second_hop_prbs);
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_3_bits_over_sr)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_f2_harq_for_sr.uci_bits.harq_ack_nof_bits  = 3U;
  pucch_expected_f2_harq_for_sr.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f2_harq_for_sr.uci_bits.csi_part1_nof_bits = 0U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f2_for_sr, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f2_harq_for_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_4_bits_over_sr)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  add_format2_3_4_harq_grant();
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_f2_harq_for_sr.uci_bits.harq_ack_nof_bits  = 4U;
  pucch_expected_f2_harq_for_sr.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f2_harq_for_sr.uci_bits.csi_part1_nof_bits = 0U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f2_for_sr, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f2_harq_for_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_over_csi)
{
  const unsigned csi_part1_bits = 4;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  // After the multiplexing, the PUCCH F2 resource is that one that have the same PUCCH resource indicator as
  // pucch_res_idx_f0_for_csi; we need to update the PRBs and symbols accordingly. With the given configuration, this
  // resource will have the same PRBs and symbols as the F2 resource for SR.
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 1U;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // The allocation should preserve the pucch_res_idx_f0_for_csi
  ASSERT_EQ(pucch_res_idx_f2_for_csi, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_2_bits_over_csi)
{
  const unsigned csi_part1_bits = 4;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 2U;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f2_for_csi, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_3_bits_over_csi)
{
  const unsigned csi_part1_bits = 4;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 3U;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f2_for_csi, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_0, test_harq_allocation_4_bits_over_csi)
{
  const unsigned csi_part1_bits = 4;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  add_format2_3_4_harq_grant();
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 4U;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4U;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx_f2_for_csi, test_pucch_res_indicator);
  ASSERT_EQ(1U, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}
