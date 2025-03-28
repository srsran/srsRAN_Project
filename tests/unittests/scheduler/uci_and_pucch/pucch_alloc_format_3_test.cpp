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

///////   Test PUCCH Format 3.    ///////

class test_pucch_allocator_format_3 : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_format_3() :
    pucch_allocator_base_tester(test_bench_params{.pucch_res_common = 0, .set1_format = pucch_format::FORMAT_3})
  {
    // Set the expected grants. The values are those generated by the `pucch_builder.add_build_new_ue_pucch_cfg`.
    // Set expected grant for PUCCH Format 1 SR.
    // The expected resource for SR corresponds to the second to last of the FORMAT 1 resources in the resource
    // list.
    auto& format1_sr             = pucch_expected_f1_sr.format_params.emplace<pucch_format_1>();
    pucch_expected_f1_sr.crnti   = to_rnti(0x4601);
    pucch_expected_f1_sr.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_sr.resources.prbs             = prb_interval{3, 4};
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
    // The expected resource for HARQ corresponds to the first resource in the resource list.
    auto& format1_harq             = pucch_expected_f1_harq.format_params.emplace<pucch_format_1>();
    pucch_expected_f1_harq.crnti   = to_rnti(0x4601);
    pucch_expected_f1_harq.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected_f1_harq.resources.prbs             = prb_interval{0, 1};
    pucch_expected_f1_harq.resources.second_hop_prbs  = prb_interval{0, 0};
    pucch_expected_f1_harq.resources.symbols          = ofdm_symbol_range{0, 14};
    pucch_expected_f1_harq.uci_bits.sr_bits           = sr_nof_bits::no_sr;
    pucch_expected_f1_harq.uci_bits.harq_ack_nof_bits = 1;

    format1_harq.initial_cyclic_shift = 0;
    format1_harq.time_domain_occ      = 0;

    format1_harq.group_hopping   = pucch_group_hopping::NEITHER;
    format1_harq.n_id_hopping    = t_bench.cell_cfg.pci;
    format1_harq.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    // Set expected grant for PUCCH Format 3 HARQ.
    // The expected resource for HARQ corresponds to the first of the FORMAT 3 resources in the resource list.
    auto& format3                               = pucch_expected_f3.format_params.emplace<pucch_format_3>();
    pucch_expected_f3.crnti                     = to_rnti(0x4601);
    pucch_expected_f3.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f3.resources.prbs            = prb_interval{4, 5};
    pucch_expected_f3.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f3.resources.symbols         = ofdm_symbol_range{0, 4};

    format3.max_code_rate     = max_pucch_code_rate::dot_25;
    format3.group_hopping     = pucch_group_hopping::NEITHER;
    format3.slot_repetition   = pucch_repetition_tx_slot::no_multi_slot;
    format3.n_id_hopping      = t_bench.cell_cfg.pci;
    format3.n_id_scrambling   = t_bench.cell_cfg.pci;
    format3.n_id_0_scrambling = t_bench.cell_cfg.pci;
    format3.pi_2_bpsk         = false;
    format3.additional_dmrs   = false;

    // Set expected grant for PUCCH Format 3 CSI.
    // The expected resource for CSI corresponds to the last resource in the resource list.
    auto& format3_csi                            = pucch_expected_csi.format_params.emplace<pucch_format_3>();
    pucch_expected_csi.crnti                     = to_rnti(0x4601);
    pucch_expected_csi.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi.resources.prbs            = prb_interval{5, 6};
    pucch_expected_csi.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi.resources.symbols         = ofdm_symbol_range{0, 4};

    format3_csi.max_code_rate     = max_pucch_code_rate::dot_25;
    format3_csi.group_hopping     = pucch_group_hopping::NEITHER;
    format3_csi.slot_repetition   = pucch_repetition_tx_slot::no_multi_slot;
    format3_csi.n_id_hopping      = t_bench.cell_cfg.pci;
    format3_csi.n_id_scrambling   = t_bench.cell_cfg.pci;
    format3_csi.n_id_0_scrambling = t_bench.cell_cfg.pci;
    format3_csi.pi_2_bpsk         = false;
    format3_csi.additional_dmrs   = false;
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info pucch_expected_f1_sr;
  pucch_info pucch_expected_f1_harq;
  pucch_info pucch_expected_f3;
  pucch_info pucch_expected_csi;
};

/////////////// Tests PUCCH allocator for SR.

TEST_F(test_pucch_allocator_format_3, test_sr_allocation_only)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  t_bench.pucch_alloc.pucch_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f1_sr](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

/////////////// Tests PUCCH allocator for CSI.

TEST_F(test_pucch_allocator_format_3, test_csi_alloc_only)
{
  static constexpr unsigned csi_part1_bits       = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_format_3, test_csi_alloc_over_sr)
{
  static constexpr unsigned csi_part1_bits       = 4;
  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  add_sr_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  t_bench.pucch_alloc.pucch_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), csi_part1_bits);

  // Expect 1 PUCCH PDU.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_allocator_format_3, test_csi_alloc_when_no_free_csi_resources)
{
  static constexpr unsigned csi_part1_bits       = 4;
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

TEST_F(test_pucch_allocator_format_3, test_csi_alloc_over_common_harq_grant)
{
  static constexpr unsigned csi_part1_bits       = 4;
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

///////  Test HARQ-ACK allocation on ded. resources - Format 3   ///////

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_3bits)
{
  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // By allocating the HARQ-ACK 3 times, the PUCCH is forced to convert the Format 1 into format 3, as Format 1 can
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

  // Convert to Format 3 and with 3 bits HARQ.
  test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_3, slot_grid.result.ul.pucchs[0].format());
  ASSERT_EQ(3, slot_grid.result.ul.pucchs[0].uci_bits.harq_ack_nof_bits);
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_3bits_over_sr)
{
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 0;

  add_sr_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 resource with HARQ+SR.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f3](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_1bit_over_csi)
{
  // With 1 HARQ-ACK bit, the F1 HARQ-ACK resource overlaps with the F3 resource for CSI, thus the 2 resources will be
  // multiplexed into 1, which is the PUCCH HARQ resource from set 1 (Format 3).
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = 1;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Expect 1 resource with HARQ+CSI.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const auto& pdu) { return pdu.csi_rep_cfg.has_value(); }));

  ASSERT_TRUE(find_pucch_pdu(
      pucch_pdus, [&expected = pucch_expected_f3](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_2bits_over_csi)
{
  // With 2 HARQ-ACK bits, the F1 HARQ-ACK resource overlaps with the F3 resource for CSI, thus the 2 resources will be
  // multiplexed into 1, which is the PUCCH HARQ resource from set 1 (Format 3).
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = 2;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  // Expect 1 resource with HARQ+CSI.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  const auto& pucch_pdus = slot_grid.result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const auto& pdu) { return pdu.csi_rep_cfg.has_value(); }));

  ASSERT_TRUE(find_pucch_pdu(
      pucch_pdus, [&expected = pucch_expected_f3](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_3bits_over_csi)
{
  // We don't know a-priori whether CSI and HARQ will be multiplexed within the same resource; we need to consider both
  // possibilities, (i) 2 separate PUCCH resources HARQ + CSI, and (ii) 1 PUCCH resource with both HARQ and CSI.
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 0;

  pucch_expected_csi.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi.uci_bits.csi_part1_nof_bits = 4;

  pucch_info pucch_f3_harq_csi_mplexed                  = pucch_expected_f3;
  pucch_f3_harq_csi_mplexed.uci_bits.harq_ack_nof_bits  = 3;
  pucch_f3_harq_csi_mplexed.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_f3_harq_csi_mplexed.uci_bits.csi_part1_nof_bits = 4;

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
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [](const auto& pdu) { return pdu.csi_rep_cfg.has_value(); }));
  if (slot_grid.result.ul.pucchs.size() == 2) {
    // Separate resources.
    ASSERT_TRUE(find_pucch_pdu(
        pucch_pdus, [&expected = pucch_expected_f3](const auto& pdu) { return pucch_info_match(expected, pdu); }));
    ASSERT_TRUE(find_pucch_pdu(
        pucch_pdus, [&expected = pucch_expected_csi](const auto& pdu) { return pucch_info_match(expected, pdu); }));
  } else {
    // Multiplexed.
    ASSERT_TRUE(find_pucch_pdu(pucch_pdus, [&expected = pucch_f3_harq_csi_mplexed](const auto& pdu) {
      return pucch_info_match(expected, pdu);
    }));
  }
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_3bits_over_sr_and_csi)
{
  // With SR and with PUCCH Format 1 it is guaranteed that the resources will be multiplexed, as PUCCH Format 1 for SR
  // spans over the 14 symbols.
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = 3;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 4;

  add_sr_grant();
  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 resource with HARQ+SR+CSI.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f3](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_7bits_over_sr_and_csi)
{
  // 1 SR + 4 CSI + 7 HARQ = 12 UCI bits (= PUCCH F3 capacity).
  constexpr unsigned nof_harq_bits = 7;
  // With SR and with PUCCH Format 1 it is guaranteed that the resources will be multiplexed, as PUCCH Format 1 for SR
  // spans over the 14 symbols.
  pucch_expected_f3.uci_bits.harq_ack_nof_bits  = nof_harq_bits;
  pucch_expected_f3.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_f3.uci_bits.csi_part1_nof_bits = 4;

  add_sr_grant();
  add_csi_grant();
  for (int i = 0; i != nof_harq_bits - 1; ++i) {
    add_harq_grant();
  }
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 resource with HARQ+SR+CSI.
  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  ASSERT_EQ(pucch_res_idx, test_pucch_res_indicator.value());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_f3](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [](const auto& pdu) { return pdu.csi_rep_cfg.has_value(); }));
}

TEST_F(test_pucch_allocator_format_3, test_harq_alloc_8bits_over_sr_and_csi_fails)
{
  // 1 SR + 4 CSI + 8 HARQ = 13 UCI bits (> PUCCH F3 capacity).
  constexpr unsigned nof_harq_bits = 8;
  add_sr_grant();
  add_csi_grant();
  for (int i = 0; i != nof_harq_bits - 1; ++i) {
    add_harq_grant();
  }

  // This should fail, as the PUCCH F3 payload for this configuration allows max 12 UCI bits.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_FALSE(test_pucch_res_indicator.has_value());
}

TEST_F(test_pucch_allocator_format_3, when_converting_harq_f1_to_f3_during_mplexing_csi_preserve_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_csi_grant();

  // At the end of the PUCCH allocation with Format 3, we expect the same PUCCH as for PUCCH format 1.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator 0 is used by the first UE that got allocated.
  ASSERT_EQ(1U, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_format_3, when_converting_harq_f1_to_f3_during_mplexing_sr_csi_preserve_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_sr_grant();
  add_csi_grant();

  // At the end of the PUCCH allocation with Format 3, we expect the same PUCCH as for PUCCH format 1.
  std::optional<unsigned> test_pucch_res_indicator = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  ASSERT_TRUE(test_pucch_res_indicator.has_value());
  // PUCCH resource indicator 0 is used by the first UE that got allocated.
  ASSERT_EQ(1U, test_pucch_res_indicator.value());
}

TEST_F(test_pucch_allocator_format_3, with_f3_res_1_harq_bit_adding_adding_extra_bit_doesnt_change_res_indicator)
{
  // This makes PUCCH resource indicator 0 busy for PUCCH resource set 0.
  add_ue_with_harq_grant();
  add_csi_grant();

  // After the second PUCCH allocation with Format 3, we expect the same PUCCH res indicator as for the first
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
                   [rnti = t_bench.get_main_ue().crnti](const auto& pdu) { return pdu.crnti == rnti; });
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
                   [rnti = t_bench.get_main_ue().crnti](const auto& pdu) { return pdu.crnti == rnti; });
  ASSERT_TRUE(second_alloc != slot_grid.result.ul.pucchs.end());
  ASSERT_EQ(first_alloc->resources.prbs, second_alloc->resources.prbs);
  ASSERT_EQ(first_alloc->resources.symbols, second_alloc->resources.symbols);
  ASSERT_EQ(first_alloc->resources.second_hop_prbs, second_alloc->resources.second_hop_prbs);
}
