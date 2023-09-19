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

///////   Test allocation of dedicated PUCCH resources    ///////

class pucch_allocator_fallback_mode_test : public ::testing::Test
{
public:
  pucch_allocator_fallback_mode_test(){};

protected:
  // Parameters that are passed by the routine to run the tests.
  test_bench     t_bench;
  const unsigned pucch_res_idx{0};

  void add_sr_grant(bool is_fallback = false)
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg(),
                                                      is_fallback);
  }

  pucch_harq_ack_grant add_format1_harq_grant()
  {
    return t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);
  }

  pucch_harq_ack_grant add_harq_grant_common()
  {
    return t_bench.pucch_alloc.alloc_common_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, t_bench.k1, t_bench.dci_info);
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

  void add_csi_grant(bool is_fallback = false, unsigned csi_part1_bits = 4)
  {
    t_bench.pucch_alloc.pucch_allocate_csi_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                       t_bench.get_main_ue().crnti,
                                                       t_bench.get_main_ue().get_pcell().cfg(),
                                                       csi_part1_bits,
                                                       is_fallback);
  }
};

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_common_csi)
{
  pucch_harq_ack_grant grant = add_harq_grant_common();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_ded_plus_csi)
{
  pucch_harq_ack_grant grant = add_format1_harq_grant();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_common_sr)
{
  pucch_harq_ack_grant grant = add_harq_grant_common();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_sr_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // For the SR, we expect both ACK and SR bits.
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.sr_bits);
}

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_common_csi_sr)
{
  pucch_harq_ack_grant grant = add_harq_grant_common();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
  // After allocating the SR, we expect SR and CSI on the same F2 grant, and the HARQ in a separate one.
  add_sr_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.sr_bits);
}

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_common_sr_csi)
{
  pucch_harq_ack_grant grant = add_harq_grant_common();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_sr_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_1.harq_ack_nof_bits);

  // For the SR, we expect both ACK and SR bits.
  ASSERT_EQ(pucch_format::FORMAT_1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(1, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_1.sr_bits);

  // After allocating the CSI, we expect SR and CSI on the same F2 grant, and the HARQ in a separate one.
  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
  ASSERT_EQ(sr_nof_bits::one, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.sr_bits);
}

TEST_F(pucch_allocator_fallback_mode_test, test_allocate_harq_f2_plus_csi)
{
  pucch_harq_ack_grant grant = add_format2_harq_grant();
  ASSERT_NE(nullptr, grant.pucch_pdu);

  add_csi_grant(/* is_fallback=*/true);
  ASSERT_EQ(2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs.size());
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format);
  ASSERT_EQ(3, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[0].format_2.harq_ack_nof_bits);
  // In the CSI grant, we only expect the CSI bits.
  ASSERT_EQ(pucch_format::FORMAT_2, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format);
  ASSERT_EQ(0, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.harq_ack_nof_bits);
  ASSERT_EQ(4, t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.pucchs[1].format_2.csi_part1_bits);
}
