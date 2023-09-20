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

class test_uci_allocator_fallback : public ::testing::Test
{
public:
  test_uci_allocator_fallback(const test_bench_params& params = {}) : t_bench(params), k2{t_bench.k0 + t_bench.k1} {}

protected:
  test_bench t_bench;
  unsigned   k2;

  void add_sr_grant(bool is_fallback = false)
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg(),
                                                      is_fallback);
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

  void add_csi_grant(bool is_fallback = false)
  {
    const unsigned csi_part1_bits = 4;
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits,
                                                       is_fallback);
  }

  void add_pusch_alloc(unsigned pusch_slot_number)
  {
    auto& puschs = t_bench.res_grid[pusch_slot_number].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }
};

///////   UCI allocation on PUCCH common resources ///////

TEST_F(test_uci_allocator_fallback, uci_alloc_on_common_pucch_resources)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates,
                                      &t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.front().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.front().format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.
}

TEST_F(test_uci_allocator_fallback, uci_alloc_on_ded_pucch_resources)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates,
                                      nullptr);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.front().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.front().format_1.sr_bits);
  // Note: no need to check other PUCCH grant values, as this is part of pucch_allocator test.

  // Test again without passing any argument for the dci_info.
  t_bench.slot_indication(++t_bench.sl_tx);
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid_1 = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid_1.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid_1.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid_1.result.ul.pucchs.front().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid_1.result.ul.pucchs.front().format_1.sr_bits);
}

TEST_F(test_uci_allocator_fallback, uci_alloc_common_pucch_resources_with_pusch_fails)
{
  add_pusch_alloc(t_bench.k0 + k2);
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates,
                                      &t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[k2];

  // No PUCCH grant expected.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  //  1 grant expected on PUSCH.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  // Expect no UCI allocated, as calling alloc_uci_harq_ue() for common resources with existing PUSCH grant should just
  // exit.
  ASSERT_FALSE(slot_grid.result.ul.puschs.back().uci.has_value());
}

TEST_F(test_uci_allocator_fallback, uci_alloc_common_pucch_resources_with_ded_harq_pucch_fails)
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
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.front().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.front().format_1.sr_bits);

  // Allocate a UCI using common PUCCH resources and expect that to fail.
  const uci_allocation uci_pucch_common = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                                              t_bench.get_main_ue().crnti,
                                                                              t_bench.get_main_ue().get_pcell().cfg(),
                                                                              t_bench.k0,
                                                                              k1_candidates,
                                                                              &t_bench.dci_info);

  // Still 1 PUCCH grant expected.
  ASSERT_FALSE(uci_pucch_common.alloc_successful);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_uci_allocator_fallback, uci_alloc_2_common_pucch_resources_same_slot_fails)
{
  // Allocate first a UCI on PUCCH common resource and verify it's a success.
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates,
                                      &t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  //  No grants expected on PUSCH.
  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.front().format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::no_sr, slot_grid.result.ul.pucchs.front().format_1.sr_bits);

  // Allocate a second UCI using common PUCCH resources and expect that to fail.
  const uci_allocation uci_pucch_common = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                                              t_bench.get_main_ue().crnti,
                                                                              t_bench.get_main_ue().get_pcell().cfg(),
                                                                              t_bench.k0,
                                                                              k1_candidates,
                                                                              &t_bench.dci_info);

  // Still 1 PUCCH grant expected.
  ASSERT_FALSE(uci_pucch_common.alloc_successful);
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_common_csi_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates,
                                      &t_bench.dci_info);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_ded_and_common_plus_csi_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  uci_allocation uci_alloc = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                                 t_bench.get_main_ue().crnti,
                                                                 t_bench.get_main_ue().get_pcell().cfg(),
                                                                 t_bench.k0,
                                                                 k1_candidates,
                                                                 &t_bench.dci_info);

  ASSERT_FALSE(uci_alloc.alloc_successful);

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_ded_and_common_plus_sr_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                      t_bench.get_main_ue().crnti,
                                      t_bench.get_main_ue().get_pcell().cfg(),
                                      t_bench.k0,
                                      k1_candidates);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  uci_allocation uci_alloc = t_bench.uci_alloc.alloc_uci_harq_ue(t_bench.res_grid,
                                                                 t_bench.get_main_ue().crnti,
                                                                 t_bench.get_main_ue().get_pcell().cfg(),
                                                                 t_bench.k0,
                                                                 k1_candidates,
                                                                 &t_bench.dci_info);

  ASSERT_FALSE(uci_alloc.alloc_successful);

  add_sr_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.sr_bits);
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_ded_f2_plus_csi_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  add_format2_grant_on_pucch();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(3, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.csi_part1_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_ded_f2_plus_sr_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  add_format2_grant_on_pucch();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_sr_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(3, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.csi_part1_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.sr_bits);
}

TEST_F(test_uci_allocator_fallback, test_allocate_harq_ded_f2_plus_sr_csi_fallback)
{
  const std::vector<uint8_t> k1_candidates = {static_cast<uint8_t>(t_bench.k1)};
  add_format2_grant_on_pucch();

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];

  ASSERT_EQ(0, slot_grid.result.ul.puschs.size());
  // 1 PUCCH grant expected.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());

  add_sr_grant(/* is_fallback=*/true);
  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(3, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.harq_ack_nof_bits);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.csi_part1_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.sr_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}
