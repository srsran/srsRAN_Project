/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "unittests/scheduler/test_utils/uci_test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_cfg_params {
  alpha_scaling_opt alpha{alpha_scaling_opt::f1};
  uint8_t           beta_offset_harq_ack{9};
};

class test_uci_allocator : public ::testing::Test
{
public:
  test_uci_allocator() : k2{t_bench.k0 + t_bench.k1} {}

protected:
  test_bench                 t_bench;
  unsigned                   k2;
  expected_output_cfg_params output_params{};

  bool check_pusch_out_param(const ul_sched_info& pusch_pdu) const
  {
    srsgnb_assert(pusch_pdu.uci.has_value(), "Expected UCI on PUSCH PDU not found.");
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

  void add_pusch_alloc()
  {
    auto& puschs = t_bench.res_grid[t_bench.k0 + t_bench.k1].result.ul.puschs;
    puschs.emplace_back(ul_sched_info{});
    puschs.back().pusch_cfg.rnti = t_bench.get_main_ue().crnti;
  }
};

///////   UCI allocation on PUCCH    ///////

TEST_F(test_uci_allocator, uci_alloc_with_empty_grants)
{
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

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
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

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
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

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
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

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

///////   UCI allocation on PUSCH    ///////

TEST_F(test_uci_allocator, uci_alloc_on_existing_pusch)
{
  add_pusch_alloc();
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

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

TEST_F(test_uci_allocator, uci_alloc_on_existing_pusch_and_pucch)
{
  add_pusch_alloc();
  add_harq_grant_on_pucch();
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[k2];

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_EQ(2, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

TEST_F(test_uci_allocator, uci_alloc_on_existing_pusch_and_pucch_plus_sr)
{
  add_pusch_alloc();
  add_sr_grant();
  add_harq_grant_on_pucch();
  t_bench.uci_alloc.alloc_uci_harq_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[k2];

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
  // 1 expected PUSCH grant.
  ASSERT_EQ(1, slot_grid.result.ul.puschs.size());
  ASSERT_EQ(2, slot_grid.result.ul.puschs.back().uci.value().harq_ack_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part1_nof_bits);
  ASSERT_EQ(0, slot_grid.result.ul.puschs.back().uci.value().csi_part2_nof_bits);
  ASSERT_TRUE(check_pusch_out_param(slot_grid.result.ul.puschs.back()));
}

///////   UCI multiplexing on PUSCH    ///////

TEST_F(test_uci_allocator, uci_no_multiplexing_on_pusch)
{
  add_pusch_alloc();
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
  add_pusch_alloc();
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

TEST_F(test_uci_allocator, uci_multiplexing_harq_sr_on_pusch)
{
  add_pusch_alloc();
  add_sr_grant();
  add_harq_grant_on_pucch();
  auto& slot_grid = t_bench.res_grid[k2];

  // 1 PUSCH grant (without UCI) and 2 PUCCH grants expected before multiplexing.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
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
  add_pusch_alloc();
  auto& slot_grid = t_bench.res_grid[k2];
  t_bench.uci_alloc.uci_allocate_sr_opportunity(
      slot_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg());

  // No grants expected on PUCCH.
  ASSERT_EQ(0, slot_grid.result.ul.pucchs.size());
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
