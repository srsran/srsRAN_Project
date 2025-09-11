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
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;

///////   Test allocation of dedicated PUCCH resources    ///////

class test_pucch_allocator_ded_resources : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_ded_resources()
  {
    // Set expected grant for PUCCH Format 1 SR.
    auto& format1_sr             = pucch_expected_f1_sr.format_params.emplace<pucch_format_1>();
    pucch_expected_f1_sr.crnti   = to_rnti(0x4601);
    pucch_expected_f1_sr.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_sr.resources.prbs             = prb_interval{NOF_RBS - 3, NOF_RBS - 2};
    pucch_expected_f1_sr.resources.second_hop_prbs  = prb_interval{0, 0};
    pucch_expected_f1_sr.resources.symbols          = ofdm_symbol_range{0, 14};
    pucch_expected_f1_sr.uci_bits.sr_bits           = sr_nof_bits::one;
    pucch_expected_f1_sr.uci_bits.harq_ack_nof_bits = 0;

    format1_sr.initial_cyclic_shift = 0;
    format1_sr.time_domain_occ      = 0;

    format1_sr.group_hopping   = pucch_group_hopping::NEITHER;
    format1_sr.n_id_hopping    = t_bench.cell_cfg.pci;
    format1_sr.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    // Set expected grant for PUCCH Format 1 HARQ.
    auto& format1_harq             = pucch_expected_f1_harq.format_params.emplace<pucch_format_1>();
    pucch_expected_f1_harq.crnti   = to_rnti(0x4601);
    pucch_expected_f1_harq.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_harq.resources.prbs             = prb_interval{NOF_RBS - 1, NOF_RBS};
    pucch_expected_f1_harq.resources.second_hop_prbs  = prb_interval{0, 0};
    pucch_expected_f1_harq.resources.symbols          = ofdm_symbol_range{0, 14};
    pucch_expected_f1_harq.uci_bits.sr_bits           = sr_nof_bits::no_sr;
    pucch_expected_f1_harq.uci_bits.harq_ack_nof_bits = 1;

    format1_harq.initial_cyclic_shift = 0;
    format1_harq.time_domain_occ      = 0;

    format1_harq.group_hopping   = pucch_group_hopping::NEITHER;
    format1_harq.n_id_hopping    = t_bench.cell_cfg.pci;
    format1_harq.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    auto& format2                               = pucch_expected_f2.format_params.emplace<pucch_format_2>();
    pucch_expected_f2.crnti                     = to_rnti(0x4601);
    pucch_expected_f2.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2.resources.prbs            = prb_interval{2, 3};
    pucch_expected_f2.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2.resources.symbols         = ofdm_symbol_range{0, 2};

    format2.max_code_rate     = max_pucch_code_rate::dot_25;
    format2.n_id_scambling    = t_bench.cell_cfg.pci;
    format2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    auto& format2_csi                            = pucch_expected_csi.format_params.emplace<pucch_format_2>();
    pucch_expected_csi.crnti                     = to_rnti(0x4601);
    pucch_expected_csi.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi.resources.prbs            = prb_interval{2, 3};
    pucch_expected_csi.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi.resources.symbols         = ofdm_symbol_range{12, 14};

    format2_csi.max_code_rate     = max_pucch_code_rate::dot_25;
    format2_csi.n_id_scambling    = t_bench.cell_cfg.pci;
    format2_csi.n_id_0_scrambling = t_bench.cell_cfg.pci;
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info pucch_expected_f1_sr;
  pucch_info pucch_expected_f1_harq;
  pucch_info pucch_expected_f2;
  pucch_info pucch_expected_csi;
};

/////////////// Tests PUCCH allocator for SR.

TEST_F(test_pucch_allocator_ded_resources, test_sr_allocation_only)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_sr](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
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

  std::optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_indicator.has_value());
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // The SR won't be allocated if there is an existing PUCCH common grant. It is possible to have both SR and HARQ if
  // the SR is scheduled first.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

/////////////// Tests PUCCH allocator for CSI.

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_only)
{
  const unsigned csi_part1_bits                  = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_over_sr)
{
  const unsigned csi_part1_bits                  = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  add_sr_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_csi_alloc_when_no_free_csi_resources)
{
  const unsigned csi_part1_bits                  = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

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
  const unsigned csi_part1_bits                  = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  std::optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_indicator.has_value());
  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU, as the CSI over common PUCCH should be scheduled following CSI first, then the PUCCH common,
  // not the other way around.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

///////  Test HARQ-ACK allocation on ded. resources  - Format 1   ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_allocation_only)
{
  auto&                         slot_grid                = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_allocation_over_sr)
{
  pucch_expected_f1_sr.uci_bits.harq_ack_nof_bits = 1;

  add_sr_grant();
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  // Verify that the UCI bits grants are correct.
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_sr](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2bits)
{
  pucch_expected_f1_harq.uci_bits.harq_ack_nof_bits = 2;

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const auto first_alloc = slot_grid.result.ul.pucchs.front();

  const std::optional<unsigned> test_pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator_1.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator_1.value());
  // Expect 1 PUCCH HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));

  // Make sure the second PUCCH allocation uses the same PRBs and symbols as the first one.
  const auto second_alloc = slot_grid.result.ul.pucchs.front();
  ASSERT_EQ(first_alloc.resources.prbs, second_alloc.resources.prbs);
  ASSERT_EQ(first_alloc.resources.symbols, second_alloc.resources.symbols);
  ASSERT_EQ(first_alloc.resources.second_hop_prbs, second_alloc.resources.second_hop_prbs);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2bits_over_sr)
{
  pucch_expected_f1_harq.uci_bits.harq_ack_nof_bits = 2;
  pucch_expected_f1_sr.uci_bits.harq_ack_nof_bits   = 2;

  add_sr_grant();

  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const auto* first_alloc = std::find_if(slot_grid.result.ul.pucchs.begin(),
                                         slot_grid.result.ul.pucchs.end(),
                                         [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
                                           return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and
                                                  pdu.uci_bits.harq_ack_nof_bits == 1U and
                                                  pdu.uci_bits.sr_bits == sr_nof_bits::no_sr;
                                         });
  ASSERT_TRUE(first_alloc != slot_grid.result.ul.pucchs.end());

  const std::optional<unsigned> test_pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator_1.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator_1.value());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  // Verify that the UCI bits grants are correct.
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_sr](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));

  // Make sure the second PUCCH allocation uses the same PRBs and symbols as the first one.
  const auto* second_alloc = std::find_if(slot_grid.result.ul.pucchs.begin(),
                                          slot_grid.result.ul.pucchs.end(),
                                          [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
                                            return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and
                                                   pdu.uci_bits.harq_ack_nof_bits == 2U and
                                                   pdu.uci_bits.sr_bits == sr_nof_bits::no_sr;
                                          });
  ASSERT_TRUE(first_alloc != slot_grid.result.ul.pucchs.end());
  ASSERT_EQ(first_alloc->resources.prbs, second_alloc->resources.prbs);
  ASSERT_EQ(first_alloc->resources.symbols, second_alloc->resources.symbols);
  ASSERT_EQ(first_alloc->resources.second_hop_prbs, second_alloc->resources.second_hop_prbs);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_7bits_over_sr)
{
  add_sr_grant();
  // Add 6 HARQ grants to reach 7 UCI bits.
  for (unsigned n = 0; n != 6; ++n) {
    add_harq_grant();
  }
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_7bits_over_sr_should_fail)
{
  add_sr_grant();
  // Add 7 HARQ grants to reach 8 UCI bits.
  for (unsigned n = 0; n != 7; ++n) {
    add_harq_grant();
  }
  // The 8th HARQ grant should not be allocated, as the 8-bit max PUCCH payload has already been reached.
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

///////  Test HARQ-ACK allocation on ded. resources - Format 1  - Multi UEs ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2_ues)
{
  add_ue_with_harq_grant();
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
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
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
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
  const std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
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
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].uci_bits.harq_ack_nof_bits);

  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[0].uci_bits.harq_ack_nof_bits);

  // Convert to Format 2 and with 3 bits HARQ.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs[0].uci_bits.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_sr)
{
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 0;

  add_sr_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f2](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_1bit_over_csi)
{
  // With 1 HARQ-ACK bit, the F1 HARQ-ACK resource overlaps with the F2 resource for CSI, thus the 2 resource will be
  // multiplexed into 1, which is the PUCCH HARQ resource from set 1 (Format 2).
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 1;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) { return pdu.csi_rep_cfg.has_value(); }));

  ASSERT_TRUE(find_pucch_pdu(
      pucch_pdus, [&expected = pucch_expected_f2](const pucch_info& pdu) { return pucch_info_match(expected, pdu); }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_2bits_over_csi)
{
  // With 2 HARQ-ACK bits, the F1 HARQ-ACK resource overlaps with the F2 resource for CSI, thus the 2 resource will be
  // multiplexed into 1, which is the PUCCH HARQ resource from set 1 (Format 2).
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 2;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) { return pdu.csi_rep_cfg.has_value(); }));

  ASSERT_TRUE(find_pucch_pdu(
      pucch_pdus, [&expected = pucch_expected_f2](const pucch_info& pdu) { return pucch_info_match(expected, pdu); }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_csi)
{
  // We don't know a-priori whether CSI and HARQ will be multilplexed within the same resource; we need to consider both
  // possibilities, (i) 2 separate PUCCH resources HARQ + CSI, and (ii) 1 PUCCH resource with both HARQ and CSI.
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 0;

  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4;

  pucch_info pucch_f2_harq_csi_mplexed                  = pucch_expected_f2;
  pucch_f2_harq_csi_mplexed.uci_bits.harq_ack_nof_bits  = 3;
  pucch_f2_harq_csi_mplexed.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_f2_harq_csi_mplexed.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_TRUE(slot_grid.result.ul.pucchs.size() == 1 or slot_grid.result.ul.pucchs.size() == 2);

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) { return pdu.csi_rep_cfg.has_value(); }));
  if (slot_grid.result.ul.pucchs.size() == 2) {
    // Separate resources.
    ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_expected_f2](const pucch_info& pdu) {
      return pucch_info_match(expected, pdu);
    }));
    ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_expected_csi](const pucch_info& pdu) {
      return pucch_info_match(expected, pdu);
    }));
  } else {
    // Multiplexed.
    ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_f2_harq_csi_mplexed](const pucch_info& pdu) {
      return pucch_info_match(expected, pdu);
    }));
  }
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_3bits_over_sr_and_csi)
{
  // With SR and with PUCCH Format 1 it is guaranteed that the resources will be multiplexed, as PUCCH Format 1 for SR
  // spans over the 14 symbols.
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 4;

  add_sr_grant();
  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f2](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_4bits_over_sr_and_csi)
{
  // With SR and with PUCCH Format 1 it is guaranteed that the resources will be multiplexed, as PUCCH Format 1 for SR
  // spans over the 14 symbols.
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 4;
  pucch_expected_f2.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 3;

  add_sr_grant();
  // Only add 3 CSI bits, otherwise the payload isn't big enough for all UCI bits.
  add_csi_grant(/*csi_part1_bits*/ 3);
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f2](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(
      find_pucch_pdu(slot_grid.result.ul.pucchs, [](const pucch_info& pdu) { return pdu.csi_rep_cfg.has_value(); }));
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_alloc_4bits_over_sr_and_csi_fails)
{
  add_sr_grant();
  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  // This should fail, as the PUCCH F2 payload for this configuration allows max 8 UCI bits.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, when_converting_harq_f1_to_f2_during_mplexing_csi_preserve_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_csi_grant();

  // At the end of the PUCCH allocation with Format 2, we expect the same PUCCH as for PUCCH format 1.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator 0 is used by the first UE that got allocated.
  ASSERT_EQ(1U, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_ded_resources, when_converting_harq_f1_to_f2_during_mplexing_sr_csi_preserve_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_sr_grant();
  add_csi_grant();

  // At the end of the PUCCH allocation with Format 2, we expect the same PUCCH as for PUCCH format 1.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator 0 is used by the first UE that got allocated.
  ASSERT_EQ(1U, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_ded_resources, with_f2_res_1_harq_bit_adding_adding_extra_bit_doesnt_change_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_csi_grant();

  // After the second PUCCH allocation with Format 2, we expect the same PUCCH res indicator as for the first
  // allocation of HARQ-ACK bit.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator 0 is used by the first UE that got allocated.
  ASSERT_EQ(1U, test_pucch_res_indicator.value());

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const auto* first_alloc =
      std::find_if(slot_grid.result.ul.pucchs.begin(),
                   slot_grid.result.ul.pucchs.end(),
                   [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) { return pdu.crnti == rnti; });
  ASSERT_TRUE(first_alloc != slot_grid.result.ul.pucchs.end());

  std::optional<unsigned> test_pucch_res_indicator_new = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator_new.has_value());
  // PUCCH resource indicator after the second allocation should not have changed.
  ASSERT_EQ(test_pucch_res_indicator.value(), test_pucch_res_indicator_new.value());

  // Make sure the second PUCCH allocation uses the same PRBs and symbols as the first one.
  const auto* second_alloc =
      std::find_if(slot_grid.result.ul.pucchs.begin(),
                   slot_grid.result.ul.pucchs.end(),
                   [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) { return pdu.crnti == rnti; });
  ASSERT_TRUE(second_alloc != slot_grid.result.ul.pucchs.end());
  ASSERT_EQ(first_alloc->resources.prbs, second_alloc->resources.prbs);
  ASSERT_EQ(first_alloc->resources.symbols, second_alloc->resources.symbols);
  ASSERT_EQ(first_alloc->resources.second_hop_prbs, second_alloc->resources.second_hop_prbs);
}

///////   Test allocation of common + dedicated resources.    ///////

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_resource_without_existing_grants)
{
  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  // PUCCH dedicated resource.
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs[0].uci_bits.sr_bits);
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  // PUCCH common resource.
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs[1].format());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs[1].uci_bits.sr_bits);
  ASSERT_FALSE(slot_grid.result.ul.pucchs[1].resources.second_hop_prbs.empty());
}

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_resource_with_existing_f1_sr)
{
  pucch_expected_f1_harq.uci_bits.harq_ack_nof_bits = 1U;
  pucch_expected_f1_harq.uci_bits.sr_bits           = sr_nof_bits::no_sr;

  pucch_expected_f1_sr.uci_bits.harq_ack_nof_bits = 1U;
  pucch_expected_f1_sr.uci_bits.sr_bits           = sr_nof_bits::one;

  add_sr_grant();

  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  // All resources are Format 1.
  ASSERT_TRUE(std::all_of(pucch_pdus.begin(), pucch_pdus.end(), [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1;
  }));
  // We expect 2 PUCCH dedicated resource with HARQ-ACK and SR bits.
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_expected_f1_harq](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_expected_f1_sr](const pucch_info& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  // PUCCH common resource.
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.sr_bits == sr_nof_bits::no_sr and
           pdu.uci_bits.harq_ack_nof_bits == 1U and pdu.pdu_context.is_common;
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_resource_with_existing_f2_csi)
{
  pucch_expected_f2.uci_bits.harq_ack_nof_bits  = 1U;
  pucch_expected_f2.uci_bits.sr_bits            = srsran::sr_nof_bits::no_sr;
  pucch_expected_f2.uci_bits.csi_part1_nof_bits = 4U;

  add_csi_grant();

  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(2, pucch_pdus.size());
  // PUCCH dedicated resource for HARQ-ACK.
  ASSERT_TRUE(find_pucch_pdu(
      pucch_pdus, [&expected = pucch_expected_f2](const pucch_info& pdu) { return pucch_info_match(expected, pdu); }));
  // PUCCH common resource.
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.uci_bits.sr_bits == sr_nof_bits::no_sr and
           pdu.uci_bits.harq_ack_nof_bits == 1U and pdu.pdu_context.is_common;
  }));
}

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_res_fails_due_to_no_free_resources)
{
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());

  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
  // The number of grants hasn't changed.
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_with_sr_res_fails_due_to_no_free_f1_resources)
{
  add_sr_grant();
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  add_ue_with_harq_grant();
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.size());

  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
  // The number of grants hasn't changed.
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, test_common_plus_ded_with_csi_res_fails_due_to_no_free_f2_resources)
{
  add_csi_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());

  std::optional<unsigned> test_pucch_res_indicator =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
  // The number of grants hasn't changed.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_pucch_allocator_ded_resources, ded_resource_allocation_when_common_resource_is_present_is_not_allowed)
{
  // NOTE: this allocation should be done with the function alloc_common_and_ded_harq_res(), which handles this special
  // case.
  t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);

  std::optional<unsigned> pucch_res_ind = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_FALSE(pucch_res_ind.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, if_ded_common_alloc_fails_no_harq_grants_should_be_kept_in_the_scheduler)
{
  // This test recreates an edge-case scenario where the allocation of common + dedicated resources could fail if the
  // scheduler didn't clean the HARQ grant after the resource multiplexing fails.
  // The conditions to recreate this scenario are:
  // - Occupy all PUCCH resources from PUCCH res. set 1.
  // - We need to force the multiplexing for UE under test; for this, we add a CSI grant. For this peculiar case, we
  // need the allocation to fail in the multiplexing process, and not because of lack of available PUCCH resource
  // indicators from PUCCH set 0.
  // - It is important that we only occupy the PUCCH resources PUCCH res. set 1 with a PUCCH resource indicator that
  // also exists in PUCCH res. set 0; if this is not the case, the PUCCH allocator fails because of lack of available
  // PUCCH resource indicators from PUCCH set 0.
  //
  // 1) At this point, we call the allocator to allocate common + dedicated resources for the UE under test. The
  // allocation should fail because all PUCCH resources PUCCH res. set 1 are occupied (failure during multiplexing).
  // 2) We release the PUCCH resource for one of the UE occupying PUCCH res. set 1. This frees one of the PUCCH res.
  // indicator from PUCCH res. set 1.
  // 3) We repeat the allocation of common + dedicated resources for the UE under test; this time we expect a success;
  // if the allocator didn't clean the HARQ grants, the allocation would fail because it finds a dedicated HARQ grant
  // for the UE (which is not allowed the allocation of common + dedicated resources).

  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();

  const unsigned csi_part1_bits = 4;
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                     t_bench.get_main_ue().crnti,
                                                     t_bench.get_main_ue().get_pcell().cfg(),
                                                     csi_part1_bits);

  std::optional<unsigned> pucch_res_ind =
      t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                        t_bench.get_main_ue().crnti,
                                                        t_bench.get_main_ue().get_pcell().cfg(),
                                                        t_bench.k0,
                                                        t_bench.k1,
                                                        t_bench.dci_info);

  ASSERT_FALSE(pucch_res_ind.has_value());

  t_bench.pucch_alloc.remove_ue_uci_from_pucch(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                               t_bench.get_ue(t_bench.last_allocated_ue_idx).crnti,
                                               t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg());

  pucch_res_ind = t_bench.pucch_alloc.alloc_common_and_ded_harq_res(t_bench.res_grid,
                                                                    t_bench.get_main_ue().crnti,
                                                                    t_bench.get_main_ue().get_pcell().cfg(),
                                                                    t_bench.k0,
                                                                    t_bench.k1,
                                                                    t_bench.dci_info);

  ASSERT_TRUE(pucch_res_ind.has_value());
}

///////  Test HARQ-ACK allocation on ded. resources - Format 2  - Multi UEs ///////

TEST_F(test_pucch_allocator_ded_resources, test_harq_f2_alloc_2_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_ue_with_format2_harq_grant();
  // 1 PDU expected, as many as the number of UEs.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  const std::optional<unsigned> test_pucch_res_indicator = add_format2_3_4_harq_grant();
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH res indicator is 1, as 0 was assigned to the first UE.
  ASSERT_EQ(1, test_pucch_res_indicator.value());
  // 2 PDU expected, as many as the number of UEs.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_harq_f2_alloc_multiple_ues)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate an HARQ-ACK grant with Format 2 for 6 UEs.
  add_ue_with_format2_harq_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_ue_with_format2_harq_grant();
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());

  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  add_ue_with_format2_harq_grant();
  // 5 PDU expected, as many as the number of UEs.
  ASSERT_EQ(5, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  const std::optional<unsigned> test_pucch_res_indicator = add_format2_3_4_harq_grant();
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator expected is 5 (the first 5 UEs got PUCCH res indicator 0,..,4).
  ASSERT_EQ(5, test_pucch_res_indicator.value());
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);
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
  const std::optional<unsigned> test_pucch_res_indicator = add_format2_3_4_harq_grant();
  // 7 PDU expected, as many as the number of UEs; of these, the first 6 are expected to be Format 2, the last one
  // is Format 1, as the allocation of the Format 2 for the last UE failed for lack of available PUCCH format 2
  // resources.
  ASSERT_EQ(7, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid.result.ul.pucchs.back().format());
  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

TEST_F(test_pucch_allocator_ded_resources, test_multiple_ues_and_csi_alloc)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Allocate a CSI grant for UE 0x4601.
  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().uci_bits.csi_part1_nof_bits);

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
  add_ue_with_sr_and_harq_f2_3_4();
  add_ue_with_sr_and_harq_f2_3_4();
  add_ue_with_sr_and_harq_f2_3_4();
  add_ue_with_sr_and_harq_f2_3_4();
  add_ue_with_sr_and_harq_f2_3_4();
  // 5 PDU expected, as many as the number of UEs.
  ASSERT_EQ(5, slot_grid.result.ul.pucchs.size());

  // Allocate an HARQ-ACK grant with Format 2 for UE 0x4601.
  add_ue_with_sr_and_harq_f2_3_4();
  // 6 PDU expected, as many as the number of UEs.
  ASSERT_EQ(6, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.back().format());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs.back().uci_bits.sr_bits);
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
  ASSERT_EQ(sr_nof_bits::one, removed_bits.sr_bits);
  ASSERT_EQ(0, removed_bits.csi_part1_nof_bits);
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
  ASSERT_EQ(4, removed_bits.csi_part1_nof_bits);
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
  ASSERT_EQ(0, removed_bits.csi_part1_nof_bits);
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
  ASSERT_EQ(0, removed_bits.csi_part1_nof_bits);
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
  ASSERT_EQ(4, removed_bits.csi_part1_nof_bits);
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
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_1.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid_1.result.ul.pucchs.back().format());

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 5.
  auto& slot_grid_2 = t_bench.res_grid[t_bench.k0 + --k1];

  // Allocate 1 HARQ at k1 = 4.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_EQ(1, slot_grid_2.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, slot_grid_2.result.ul.pucchs.back().format());

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
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_3.result.ul.pucchs.back().format());
  ASSERT_EQ(3, slot_grid_3.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);

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
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_4.result.ul.pucchs.back().format());
  ASSERT_EQ(4, slot_grid_4.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);

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
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid_5.result.ul.pucchs.back().format());
  ASSERT_EQ(5, slot_grid_5.result.ul.pucchs.back().uci_bits.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_ded_resources, test_for_private_fnc_retrieving_existing_grants)
{
  // All the allocation allocate a HARQ-ACK grant at slot 7.
  // t_bench.sl_tx = 0; k0 = 0; k1 = 7  =>  t_bench.sl_tx + k0 + k1 = 7.
  unsigned         k1         = 7;
  auto&            slot_grid  = t_bench.res_grid[t_bench.k0 + k1];
  const slot_point pucch_slot = slot_grid.slot;

  // Allocate 1 HARQ at k1 = 7.
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
  std::optional<unsigned> pucch_res_ind_ue1 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue1_idx).crnti, t_bench.get_ue(ue1_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue1.has_value());
  ASSERT_EQ(0, pucch_res_ind_ue1.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue1_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));

  std::optional<unsigned> pucch_res_ind_ue0 = t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_ind_ue0.has_value());
  // Note: The first common PUCCH resource is on the same resources as the first dedicated PUCCH resource.
  ASSERT_EQ(1, pucch_res_ind_ue0.value());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.pdu_context.is_common;
  }));

  std::optional<unsigned> pucch_res_ind_ue2 = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_ue(ue2_idx).crnti, t_bench.get_ue(ue2_idx).get_pcell().cfg(), t_bench.k0, k1);
  ASSERT_TRUE(pucch_res_ind_ue2.has_value());
  ASSERT_EQ(1, pucch_res_ind_ue2.value());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue2_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));
  // Test now that the previous allocations have not been messed up.
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue1_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.pdu_context.is_common;
  }));

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
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue1_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 2U;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.pdu_context.is_common;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue2_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));

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
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue1_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_2 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 3U;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U and
           pdu.pdu_context.is_common;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue2_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));

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

  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue1_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_2 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 3U;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_main_ue().crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 1U;
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [rnti = t_bench.get_ue(ue2_idx).crnti](const pucch_info& pdu) {
    return pdu.format() == pucch_format::FORMAT_1 and pdu.crnti == rnti and pdu.uci_bits.harq_ack_nof_bits == 2U;
  }));
}

///////   Test allocation of dedicated PUCCH resources with different code rates and max payloads   ///////

class test_pucch_allocator_small_code_rate : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_small_code_rate() :
    pucch_allocator_base_tester(test_bench_params{.max_c_rate = max_pucch_code_rate::dot_15})
  {
  }
};

TEST_F(test_pucch_allocator_small_code_rate, with_4_bits_payload_csi_plus_harq_mplex_not_allowed)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_csi_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.front().uci_bits.csi_part1_nof_bits);

  // HARQ-ACK grant is expected NOT to be multiplexed with CSI grants.
  add_harq_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.front().uci_bits.csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.front().uci_bits.harq_ack_nof_bits);
}

class test_pucch_allocator_16_bit_payload : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_16_bit_payload() :
    pucch_allocator_base_tester(test_bench_params{.pucch_f2_f3_more_prbs = true, .cfg_for_mimo_4x4 = true})
  {
    srsran_assert(t_bench.get_main_ue().get_pcell().cfg().csi_meas_cfg() != nullptr,
                  "CSI configuration needed for this test");
    auto csi_report_cfg = create_csi_report_configuration(*t_bench.get_main_ue().get_pcell().cfg().csi_meas_cfg());
    csi_report_size     = get_csi_report_pucch_size(csi_report_cfg).part1_size.value();
  }

protected:
  unsigned csi_report_size = 0;
};

TEST_F(test_pucch_allocator_16_bit_payload, with_16_bits_payload_csi_plus_harq_mplex_allowed_in_mimo_4x4)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  add_csi_grant(csi_report_size);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(11, slot_grid.result.ul.pucchs.front().uci_bits.csi_part1_nof_bits);

  // HARQ-ACK grants are expected to be multiplexed with CSI grants.
  add_harq_grant();
  add_harq_grant();
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(11, slot_grid.result.ul.pucchs.front().uci_bits.csi_part1_nof_bits);
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.front().uci_bits.harq_ack_nof_bits);
}
