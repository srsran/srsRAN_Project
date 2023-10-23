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

// Expected results parameters.
struct expected_output_cfg_params {
  alpha_scaling_opt alpha{alpha_scaling_opt::f1};
  uint8_t           beta_offset_harq_ack{9};
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
    srsran_assert(pusch_pdu.uci.has_value(), "Expected UCI on PUSCH PDU not found.");
    return pusch_pdu.uci.value().alpha == output_params.alpha &&
           pusch_pdu.uci.value().beta_offset_harq_ack == output_params.beta_offset_harq_ack;
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
    pdu.format_2.harq_ack_nof_bits = nof_harq_ack_bits;
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

///////   UCI allocation on PUCCH    ///////

TEST_F(test_uci_allocator, uci_alloc_with_empty_grants)
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

TEST_F(test_uci_allocator, uci_alloc_on_existing_pucch_harq)
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

TEST_F(test_uci_allocator, uci_alloc_on_existing_sr)
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

TEST_F(test_uci_allocator, test_uci_alloc_on_existing_pucch_plus_sr)
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

TEST_F(test_uci_allocator, uci_alloc_on_existing_harq_2_bits)
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

TEST_F(test_uci_allocator, uci_alloc_on_existing_pusch)
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
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

///////   UCI multiplexing on PUSCH    ///////

TEST_F(test_uci_allocator, uci_no_multiplexing_on_pusch)
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

TEST_F(test_uci_allocator, uci_multiplexing_harq_on_pusch)
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
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_multiplexing_3_bit_harq_on_pusch)
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
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_multiplexing_harq_sr_on_pusch)
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
  ASSERT_EQ(1, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
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
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_multiplexing_3_bit_harq_sr_csi_on_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  add_format2_grant_on_pucch(3, srsran::sr_nof_bits::one);
  add_csi_grant();
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
  ASSERT_EQ(3, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(4, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

///////   UCI allocate SR    ///////

TEST_F(test_uci_allocator, sr_with_empty_pucch)
{
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs.back().format_1.sr_bits);
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.back().format_1.harq_ack_nof_bits);
}

TEST_F(test_uci_allocator, sr_with_existing_pucch)
{
  auto& slot_grid = t_bench.res_grid[k2];

  add_harq_grant_on_pucch();
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs[0].format_1.sr_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, sr_with_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

///////   UCI allocate CSI    ///////

TEST_F(test_uci_allocator, csi_with_empty_pucch)
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

TEST_F(test_uci_allocator, csi_with_existing_pucch)
{
  // We assume k2 = k0 + k1;
  auto& slot_grid = t_bench.res_grid[k2];

  add_harq_grant_on_pucch();
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(4, slot_grid.result.ul.pucchs.back().format_2.csi_part1_bits);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.back().format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.back().format_2.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator, csi_with_existing_pusch)
{
  add_pusch_alloc(t_bench.k0 + k2);
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(4, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
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
    uci_allocation alloc = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                               t_bench.get_main_ue().crnti,
                                                               t_bench.get_main_ue().get_pcell().cfg(),
                                                               t_bench.k0,
                                                               k1_candidates);

    if (alloc.alloc_successful) {
      ASSERT_EQ(alloc.harq_bit_idx, i);
    }
  }
}

// Allocate multiple HARQ-ACK grants over the same target slot.
TEST_F(test_tdd_uci_allocator, test_tdd_harq_ack_multiplexing)
{
  const unsigned pusch_slot_number = 7;
  // Add a PUSCH at slot 7.
  add_pusch_alloc(pusch_slot_number);

  // All the allocation allocate a HARQ-ACK grant at slot 7.
  // t_bench.sl_tx = 0; k0 = 0; k1 = 7  =>  t_bench.sl_tx + k0 + k1 = 7.
  std::vector<uint8_t> k1_candidates = {7};
  auto&                slot_grid_1   = t_bench.res_grid[t_bench.k0 + k1_candidates.back()];

  // Allocate 1 HARQ at k1 = 7.
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);
  ASSERT_EQ(0, slot_grid_1.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid_1.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid_1.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(1, slot_grid_1.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid_1.result.ul.puschs.back().uci.value().csi_part1_nof_bits);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 1; k0 = 0; k1 = 6  =>  t_bench.sl_tx + k0 + k1 = 6.
  k1_candidates     = {6};
  auto& slot_grid_2 = t_bench.res_grid[t_bench.k0 + k1_candidates.back()];

  // Allocate 1 HARQ at k1 = 6.
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);
  ASSERT_EQ(0, slot_grid_2.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid_2.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid_2.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(2, slot_grid_2.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid_2.result.ul.puschs.back().uci.value().csi_part1_nof_bits);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 2; k0 = 0; k1 = 5  =>  t_bench.sl_tx + k0 + k1 = 5.
  k1_candidates     = {5};
  auto& slot_grid_3 = t_bench.res_grid[t_bench.k0 + k1_candidates.back()];

  // Allocate 1 HARQ at k1 = 5. This will convert the PUCCH grant to Format 2.
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);
  ASSERT_EQ(0, slot_grid_3.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid_3.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid_3.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(3, slot_grid_3.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid_3.result.ul.puschs.back().uci.value().csi_part1_nof_bits);

  // Advance by 1 slot.
  t_bench.slot_indication(++t_bench.sl_tx);
  // t_bench.sl_tx = 3; k0 = 0; k1 = 4  =>  t_bench.sl_tx + k0 + k1 = 4.
  k1_candidates     = {4};
  auto& slot_grid_4 = t_bench.res_grid[t_bench.k0 + k1_candidates.back()];

  // Allocate 1 HARQ at k1 = 4.
  t_bench.uci_alloc.uci_allocate_csi_opportunity(
      slot_grid_4, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());
  ASSERT_EQ(0, slot_grid_4.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid_4.result.ul.puschs.size());
  ASSERT_TRUE(slot_grid_4.result.ul.puschs.back().uci.has_value());
  ASSERT_EQ(3, slot_grid_4.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(4, slot_grid_4.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
}
