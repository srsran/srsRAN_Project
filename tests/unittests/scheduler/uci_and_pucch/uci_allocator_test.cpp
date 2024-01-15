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

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_cfg_params {
  alpha_scaling_opt alpha{alpha_scaling_opt::f1};
  uint8_t           beta_offset_harq_ack{9};
  uint8_t           beta_offset_csi_part1{9};
  uint8_t           beta_offset_csi_part2{6};
};

class test_uci_allocator : public ::testing::Test
{
public:
  test_uci_allocator(const test_bench_params& params = {}) : t_bench(params), k2{t_bench.k0 + t_bench.k1} {}

protected:
  test_bench                 t_bench;
  unsigned                   k2;
  expected_output_cfg_params output_params{};

  bool check_pusch_out_param(const ul_sched_info& pusch_pdu) const
  {
    if (not pusch_pdu.uci.has_value()) {
      return false;
    }
    const bool harq_beta_offset_ok =
        pusch_pdu.uci.value().harq.has_value()
            ? pusch_pdu.uci.value().harq.value().beta_offset_harq_ack == output_params.beta_offset_harq_ack
            : true;
    const bool csi_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_1 == output_params.beta_offset_csi_part1
            : true;

    return pusch_pdu.uci.value().alpha == output_params.alpha and harq_beta_offset_ok and csi_beta_offset_ok;
  }

  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_harq_grant_on_pucch(unsigned nof_harq_ack_bits = 1)
  {
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    for (auto& pucch : t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs) {
      if (pucch.crnti == t_bench.get_main_ue().crnti) {
        pucch.format_1.harq_ack_nof_bits = nof_harq_ack_bits;
      }
    }
  }

  void add_format2_grant_on_pucch(unsigned nof_harq_ack_bits = 3, sr_nof_bits sr_bits = srsran::sr_nof_bits::no_sr)
  {
    srsran_assert(nof_harq_ack_bits > 2, "At least 3 HARQ bits are required to trigger a Format 2 PUCCH grant.");
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
    auto& pdu                      = t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.front();
    pdu.format_2.harq_ack_nof_bits = nof_harq_ack_bits;
    pdu.format_2.sr_bits           = sr_bits;
  }

  void add_csi_grant(unsigned csi_part1_bits = 4)
  {
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits);
  }

  void add_pusch_alloc(unsigned pusch_slot_number)
  {
    auto& puschs = t_bench.res_grid[pusch_slot_number].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }
};

///////   UCI allocate SR    ///////

TEST_F(test_uci_allocator, alloc_sr_only)
{
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs.back().format_1.sr_bits);
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.back().format_1.harq_ack_nof_bits);
}

TEST_F(test_uci_allocator, alloc_sr_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

///////   UCI allocate CSI    ///////

TEST_F(test_uci_allocator, alloc_csi_only)
{
  // We assume k2 = k0 + k1;
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.back().format_2.sr_bits);
}

TEST_F(test_uci_allocator, alloc_csi_over_sr)
{
  // We assume k2 = k0 + k1;
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs.back().format_2.sr_bits);
}

TEST_F(test_uci_allocator, csi_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(4, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
}

///////   UCI allocation on PUCCH    ///////

TEST_F(test_uci_allocator, uci_harq_alloc_with_no_pusch_grants)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.back().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.back().format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, uci_harq_alloc_over_existing_pucch_harq)
{
  add_harq_grant_on_pucch();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.back().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.back().format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, uci_harq_alloc_over_existing_sr)
{
  add_sr_grant();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 2 PUCCH grants expected.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[0].format_1.sr_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, uci_harq_alloc_on_existing_pucch_harq_plus_sr)
{
  add_sr_grant();
  add_harq_grant_on_pucch();
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 2 PUCCH grants expected.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[0].format_1.sr_bits);
  ASSERT_EQ(2, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, uci_harq_alloc_on_existing_harq_2_bits)
{
  add_harq_grant_on_pucch(/* Nof. HARQ bits */ 2);
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, slot_grid.result.ul.pucchs.front().format);
  ASSERT_EQ(3, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.back().format_2.sr_bits);
}

///////   UCI allocation on PUSCH    ///////

TEST_F(test_uci_allocator, uci_harq_alloc_on_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[k2];

  // No PUCCH grant expected.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  //  1 grant expected on PUSCH.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  // ... but without any UCI.
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());
}

///////   UCI multiplexing on PUSCH    ///////

TEST_F(test_uci_allocator, uci_mplexing_on_pusch_with_no_pucch_grants)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());
}

TEST_F(test_uci_allocator, uci_mplexing_harq_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_harq_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) and 1 PUCCH grant expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_mplexing_3_bit_harq_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_format2_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH (without UCI) and 1 PUCCH grant expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_mplexing_harq_sr_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_sr_grant();
  add_harq_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_multiplexing_3_bit_harq_sr_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_format2_grant_on_pucch(3, srsran::sr_nof_bits::one);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_multiplexing_3_bit_harq_sr_csi_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_csi_grant();
  add_format2_grant_on_pucch(3, srsran::sr_nof_bits::one);
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq.value().harq_ack_nof_bits);
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(4, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

///////   UCI allocation in TDD ///////

class test_tdd_uci_allocator : public test_uci_allocator
{
protected:
  constexpr static size_t DAI_MOD = 4;

  test_tdd_uci_allocator() : test_uci_allocator(test_bench_params{.is_tdd = true}) {}
};

TEST_F(test_tdd_uci_allocator, when_tdd_cfg_then_harq_bit_index_increases_with_number_of_allocs)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  for (unsigned i = 0; i != DAI_MOD * 2; ++i) {
    optional<uci_allocation> alloc = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                                         t_bench.get_main_ue().crnti,
                                                                         t_bench.get_main_ue().get_pcell().cfg(),
                                                                         t_bench.k0,
                                                                         k1_candidates);

    if (alloc.has_value()) {
      ASSERT_EQ(alloc.value().harq_bit_idx, i);
    }
  }
}

class test_uci_allocator_mimo_4x4 : public ::testing::Test
{
public:
  test_uci_allocator_mimo_4x4() : t_bench(test_bench_params{.cfg_for_mimo_4x4 = true}), k2{t_bench.k0 + t_bench.k1} {}

protected:
  test_bench                 t_bench;
  unsigned                   k2;
  expected_output_cfg_params output_params{};

  bool check_pusch_out_param(const ul_sched_info& pusch_pdu) const
  {
    if (not pusch_pdu.uci.has_value()) {
      return false;
    }
    const bool harq_beta_offset_ok =
        pusch_pdu.uci.value().harq.has_value()
            ? pusch_pdu.uci.value().harq.value().beta_offset_harq_ack == output_params.beta_offset_harq_ack
            : true;
    const bool csi_p1_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_1 == output_params.beta_offset_csi_part1
            : true;
    const bool csi_p2_beta_offset_ok =
        pusch_pdu.uci.value().csi.has_value() and pusch_pdu.uci.value().csi.value().beta_offset_csi_2.has_value()
            ? pusch_pdu.uci.value().csi.value().beta_offset_csi_2.value() == output_params.beta_offset_csi_part2
            : true;

    return pusch_pdu.uci.value().alpha == output_params.alpha and harq_beta_offset_ok and csi_p1_beta_offset_ok and
           csi_p2_beta_offset_ok;
  }

  void add_pusch_alloc(unsigned pusch_slot_number)
  {
    auto& puschs = t_bench.res_grid[pusch_slot_number].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }
};

TEST_F(test_uci_allocator_mimo_4x4, uci_alloc_csi_part2_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(11, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
}

TEST_F(test_uci_allocator_mimo_4x4, uci_mplex_csi_part2_over_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];

  // Add manually the PUCCH grant and force the number of CSI bits to 11.
  auto& pucch_csi                   = slot_grid.result.ul.pucchs.emplace_back();
  pucch_csi.crnti                   = t_bench.get_main_ue().crnti;
  pucch_csi.format                  = srsran::pucch_format::FORMAT_2;
  pucch_csi.format_2.csi_part1_bits = 11;

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());

  t_bench.uci_alloc.multiplex_uci_on_pusch(slot_grid.result.ul.puschs.back(),
                                           slot_grid,
                                           t_bench.get_main_ue().get_pcell().cfg(),
                                           t_bench.get_main_ue().crnti);

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().harq.has_value());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.value().csi.has_value());
  ASSERT_EQ(11, slot_grid.result.ul.puschs.back().uci.value().csi.value().csi_part1_nof_bits);
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.value().csi.value().beta_offset_csi_2.has_value());
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}
