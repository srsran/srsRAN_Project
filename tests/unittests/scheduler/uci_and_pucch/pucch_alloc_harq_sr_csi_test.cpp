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

class test_pucch_f2_alloc_several_prbs : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_f2_alloc_several_prbs() : pucch_allocator_base_tester(test_bench_params{.pucch_f2_f3_more_prbs = true})
  {
    // This PUCCH grant will be for 5 HARQ bits, which fit in 1 PRB.
    auto& format2_harq               = pucch_expected_harq_only.format_params.emplace<pucch_format_2>();
    pucch_expected_harq_only.crnti   = to_rnti(0x4601);
    pucch_expected_harq_only.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_harq_only.resources.prbs =
        prb_interval::start_and_len(test_helpers::common_pucch_res_guardband + 2, 1);
    pucch_expected_harq_only.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_harq_only.resources.symbols         = ofdm_symbol_range{0, 2};

    format2_harq.max_code_rate     = max_pucch_code_rate::dot_25;
    format2_harq.n_id_scambling    = t_bench.cell_cfg.pci;
    format2_harq.n_id_0_scrambling = t_bench.cell_cfg.pci;

    // This PUCCH grant will be for 5 HARQ bits + 4 CSI bits, which fit in 2 PRBs.
    pucch_expected_harq_csi = pucch_expected_harq_only;
    pucch_expected_harq_csi.resources.prbs =
        prb_interval::start_and_len(test_helpers::common_pucch_res_guardband + 2, 2);

    // This PUCCH grant will be for 4 CSI bits only, which are encoded in the maximum number of PRBs.
    auto& format2_csi               = pucch_expected_csi_only.format_params.emplace<pucch_format_2>();
    pucch_expected_csi_only.crnti   = to_rnti(0x4601);
    pucch_expected_csi_only.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi_only.resources.prbs =
        prb_interval::start_and_len(test_helpers::common_pucch_res_guardband + 2, 3);
    pucch_expected_csi_only.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi_only.resources.symbols         = ofdm_symbol_range{12, 14};

    format2_csi.max_code_rate     = max_pucch_code_rate::dot_25;
    format2_csi.n_id_scambling    = t_bench.cell_cfg.pci;
    format2_csi.n_id_0_scrambling = t_bench.cell_cfg.pci;
  }

protected:
  // Parameters that are passed by the routine to run the tests.
  pucch_info pucch_expected_harq_only;
  pucch_info pucch_expected_harq_csi;
  pucch_info pucch_expected_csi_only;
};

///////   Test PUCCH Format 2 PRB configuration.    ///////

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_csi_only)
{
  pucch_expected_csi_only.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi_only.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi_only.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi_only](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_csi_sr)
{
  pucch_expected_csi_only.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi_only.uci_bits.sr_bits            = sr_nof_bits::one;
  pucch_expected_csi_only.uci_bits.csi_part1_nof_bits = 4;

  add_sr_grant();
  add_csi_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_csi_only](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
  ASSERT_TRUE(slot_grid.result.ul.pucchs[0].csi_rep_cfg.has_value());
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_harq_only)
{
  pucch_expected_harq_only.uci_bits.harq_ack_nof_bits  = 5;
  pucch_expected_harq_only.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_harq_only.uci_bits.csi_part1_nof_bits = 0;

  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 PUCCH grant with CSI.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_TRUE(find_pucch_pdu(slot_grid.result.ul.pucchs, [&expected = pucch_expected_harq_only](const auto& pdu) {
    return pucch_info_match(expected, pdu);
  }));
}

TEST_F(test_pucch_f2_alloc_several_prbs, test_prb_allocation_harq_csi_only)
{
  // We don't know a-priori whether CSI and HARQ will be multilplexed within the same resource; we need to consider both
  // possibilities, (i) 2 separate PUCCH resources HARQ + CSI, and (ii) 1 PUCCH resource with both HARQ and CSI.
  pucch_expected_harq_only.uci_bits.harq_ack_nof_bits  = 5;
  pucch_expected_harq_only.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_harq_only.uci_bits.csi_part1_nof_bits = 0;

  pucch_expected_csi_only.uci_bits.harq_ack_nof_bits  = 0;
  pucch_expected_csi_only.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_csi_only.uci_bits.csi_part1_nof_bits = 4;

  pucch_expected_harq_csi.uci_bits.harq_ack_nof_bits  = 5;
  pucch_expected_harq_csi.uci_bits.sr_bits            = sr_nof_bits::no_sr;
  pucch_expected_harq_csi.uci_bits.csi_part1_nof_bits = 4;

  add_csi_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();
  add_harq_grant();

  const auto& pucch_pdus = t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs;
  ASSERT_TRUE(find_pucch_pdu(pucch_pdus,
                             [&expected = pucch_expected_harq_only](const auto& pdu) {
                               return pucch_info_match(expected, pdu);
                             }) or
              find_pucch_pdu(
                  pucch_pdus,
                  [&expected = pucch_expected_csi_only](const auto& pdu) { return pucch_info_match(expected, pdu); }) or
              find_pucch_pdu(pucch_pdus, [&expected = pucch_expected_harq_csi](const auto& pdu) {
                return pucch_info_match(expected, pdu);
              }));
}

///////   Test UL grants reached and PUCCH fails.    ///////

class test_pucch_allocator_ded_resources_reached_ul_grants : public ::testing::Test, public pucch_allocator_base_tester
{
public:
  test_pucch_allocator_ded_resources_reached_ul_grants() :
    pucch_allocator_base_tester(test_bench_params{.pucch_res_common = 11, .n_cces = 1}, 3U, 6U)
  {
  }
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
