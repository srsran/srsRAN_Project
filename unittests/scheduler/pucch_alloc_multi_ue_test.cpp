/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/uci_scheduling/uci_scheduler_impl.h"
#include "uci_test_utils.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>

using namespace srsgnb;

////////////    Test the PUCCH resource manager class     ////////////

class test_pucch_resource_manager : public ::testing::Test
{
public:
  test_pucch_resource_manager() :
    pucch_cfg{config_helpers::make_default_ue_uplink_config().init_ul_bwp.pucch_cfg.value()}, sl_tx(slot_point(0, 0))
  {
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 2,
                                                         .starting_prb           = 1,
                                                         .second_hop_prb         = 7,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 3,
                                                         .starting_prb           = 2,
                                                         .second_hop_prb         = 8,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 4,
                                                         .starting_prb           = 3,
                                                         .second_hop_prb         = 9,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 5,
                                                         .starting_prb           = 4,
                                                         .second_hop_prb         = 10,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 6,
                                                         .starting_prb           = 5,
                                                         .second_hop_prb         = 11,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    pucch_cfg.pucch_res_list.emplace_back(pucch_resource{.res_id                 = 7,
                                                         .starting_prb           = 6,
                                                         .second_hop_prb         = 12,
                                                         .intraslot_freq_hopping = true,
                                                         .format                 = pucch_format::FORMAT_1});
    res_manager.slot_indication(sl_tx);
  };

protected:
  pucch_config           pucch_cfg;
  pucch_resource_manager res_manager;
  slot_point             sl_tx;

  // Allocate PUCCH for a given number of UEs, in increasing order of RNTI.
  void allocate_ues(unsigned nof_ues_to_allocate)
  {
    for (size_t n = 0; n != nof_ues_to_allocate; ++n) {
      rnti_t rnti = to_rnti(0x4601 + n);
      res_manager.get_next_harq_res_available(sl_tx, rnti, pucch_cfg);
    }
  };
};

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for 1 UE only.
TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_1)
{
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for n UEs.
TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_2)
{
  allocate_ues(1);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  ASSERT_EQ(1, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[1], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_3)
{
  allocate_ues(2);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4603), pucch_cfg);

  ASSERT_EQ(2, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[2], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_4)
{
  allocate_ues(3);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4604), pucch_cfg);

  ASSERT_EQ(3, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[3], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_5)
{
  allocate_ues(4);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4605), pucch_cfg);

  ASSERT_EQ(4, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[4], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_6)
{
  allocate_ues(5);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4606), pucch_cfg);

  ASSERT_EQ(5, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[5], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_7)
{
  allocate_ues(6);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4607), pucch_cfg);

  ASSERT_EQ(6, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[6], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_8)
{
  allocate_ues(7);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4608), pucch_cfg);

  ASSERT_EQ(7, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[7], record.pucch_res);
}

TEST_F(test_pucch_resource_manager, get_next_harq_res_nof_ues_9)
{
  allocate_ues(8);
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4609), pucch_cfg);

  ASSERT_EQ(nullptr, record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for 1 UE only.
TEST_F(test_pucch_resource_manager, get_next_harq_different_slot)
{
  allocate_ues(1);
  ++sl_tx;
  const pucch_harq_resource_alloc_record record =
      res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4602), pucch_cfg);

  // Expect that pucch_res_indicator = 0 is returned, as the UE 0x4602 is allocated in a different slot to UE 0x4601.
  ASSERT_EQ(0, record.pucch_res_indicator);
  ASSERT_EQ(&pucch_cfg.pucch_res_list[0], record.pucch_res);
}

// Tests whether PUCCH HARQ grant is allocated with correct PUCCH RESOURCE Indicator; for 1 UE only.
TEST_F(test_pucch_resource_manager, slot_indication)
{
  res_manager.get_next_harq_res_available(sl_tx, to_rnti(0x4601), pucch_cfg);

  // Increment slot point and invoke slot_indication(), which should reset the previous UE's resource allocation.
  ++sl_tx;
  res_manager.slot_indication(sl_tx);

  // Slot point pointing at the last slot, that has been cleared by that slot_indication().
  slot_point old_slot{0, sl_tx.to_uint() - 1};
  int        res_id = res_manager.get_pucch_res_indicator(old_slot, to_rnti(0x4601));

  // Expect that pucch_res_indicator = -1 is returned (due to the slot_indication() resetting the resource records for
  // old slots).
  ASSERT_EQ(-1, res_id);
}

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_params {
  pucch_format      format;
  prb_interval      prbs;
  prb_interval      second_hop_prbs;
  ofdm_symbol_range symbols;
  bool              intra_slot_freq_hop;
  uint8_t           initial_cyclic_shift;
  sr_nof_bits       sr_bits;
  unsigned          harq_ack_nof_bits;
  uint8_t           time_domain_occ;
};

// Parameters for PUCCH scheduler initialization.
struct pucch_params {
  // Input parameters.
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_test_parameters {
  unsigned               dci_pucch_res_indicator;
  expected_output_params output_params;
  pucch_params           pucch_input_params;
};

expected_output_params make_expected_output_params_sr_only()
{
  return expected_output_params{.format               = pucch_format::FORMAT_1,
                                .prbs                 = prb_interval{0, 1},
                                .second_hop_prbs      = prb_interval{51, 52},
                                .symbols              = ofdm_symbol_range{0, 14},
                                .intra_slot_freq_hop  = true,
                                .initial_cyclic_shift = 0,
                                .sr_bits              = sr_nof_bits::one,
                                .harq_ack_nof_bits    = 0,
                                .time_domain_occ      = 0};
}

// Make expected results parameters (default values are for SR only).
expected_output_params make_expected_output_params_sr_harq()
{
  expected_output_params out = make_expected_output_params_sr_only();
  out.harq_ack_nof_bits      = 1;
  return out;
}

static expected_output_params make_expected_output_params_harq_ded_resources()
{
  return expected_output_params{.format               = pucch_format::FORMAT_1,
                                .prbs                 = prb_interval{51, 52},
                                .second_hop_prbs      = prb_interval{0, 1},
                                .symbols              = ofdm_symbol_range{0, 14},
                                .intra_slot_freq_hop  = true,
                                .initial_cyclic_shift = 0,
                                .sr_bits              = sr_nof_bits::no_sr,
                                .harq_ack_nof_bits    = 1,
                                .time_domain_occ      = 0};
}

class test_pucch_harq_allocator_ded_resources : public ::testing::Test
{
public:
  test_pucch_harq_allocator_ded_resources() :
    harq_expected_params{make_expected_output_params_harq_ded_resources()},
    pucch_expected{build_pucch_info(&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                                    static_cast<unsigned>(t_bench.cell_cfg.pci),
                                    harq_expected_params.format,
                                    harq_expected_params.prbs,
                                    harq_expected_params.second_hop_prbs,
                                    harq_expected_params.symbols,
                                    harq_expected_params.initial_cyclic_shift,
                                    harq_expected_params.sr_bits,
                                    harq_expected_params.harq_ack_nof_bits,
                                    harq_expected_params.time_domain_occ)} {};

protected:
  // Parameters that are passed by the routing to run the tests.
  expected_output_params harq_expected_params;
  test_bench             t_bench;
  pucch_info             pucch_expected;
  const unsigned         pucch_res_idx{0};

  void add_sr_grant()
  {
    t_bench.pucch_alloc.pucch_allocate_sr_opportunity(t_bench.res_grid[t_bench.k0 + t_bench.k1],
                                                      t_bench.get_main_ue().crnti,
                                                      t_bench.get_main_ue().get_pcell().cfg());
  }

  void add_harq_grant(unsigned nof_harq_ack_bits = 1)
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

  void add_ue_with_harq_grant()
  {
    t_bench.add_ue();
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(t_bench.res_grid,
                                                    t_bench.last_allocated_rnti,
                                                    t_bench.get_ue(t_bench.last_allocated_ue_idx).get_pcell().cfg(),
                                                    t_bench.k0,
                                                    t_bench.k1);
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
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs.back()));
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
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[1]));
  // Verify that the SR grants gets updated.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs[0].format_1.harq_ack_nof_bits);
}

// Tests whether existing PUCCH HARQ grant gets updated.
TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq)
{
  add_harq_grant();
  pucch_expected.format_1.harq_ack_nof_bits = 2;
  pucch_harq_ack_grant test_pdu             = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ.
  ASSERT_EQ(1, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[0]));
}

// Tests whether existing PUCCH HARQ grant gets updated with an existing SR.
TEST_F(test_pucch_harq_allocator_ded_resources, test_update_harq_and_sr)
{
  add_harq_grant();
  pucch_expected.format_1.harq_ack_nof_bits = 2;
  add_sr_grant();
  pucch_harq_ack_grant test_pdu = t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ and 1 SR.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_EQ(test_pdu.pucch_pdu, &slot_grid.result.ul.pucchs[0]);
  ASSERT_EQ(pucch_res_idx, test_pdu.pucch_res_indicator);
  // The function add_harq_grant() has set the expected harq_ack_nof_bits = 2.
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, slot_grid.result.ul.pucchs[0]));
  // Verify that the SR grants gets updated.
  const unsigned EXPECTED_HARQ_BITS_IN_SR = 2;
  ASSERT_EQ(EXPECTED_HARQ_BITS_IN_SR, slot_grid.result.ul.pucchs[1].format_1.harq_ack_nof_bits);
  ASSERT_EQ(sr_nof_bits::one, slot_grid.result.ul.pucchs[1].format_1.sr_bits);
}

// Tests whether allocator skips PUCCH HARQ grant due to max HARQ bits reached.
TEST_F(test_pucch_harq_allocator_ded_resources, test_skip_alloc_with_2_harq_bits)
{
  // > Allocate HARQ grant with 2 HARQ-ACK bits.
  add_harq_grant(2);
  // > Attempt to allocate a second HARQ grant (which will be skipped due to an existing one with 2 HARQ bits).
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto& slot_grid = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  // Expect 1 HARQ grant, the one allocated at the beginning with 2 HARQ bits.
  const unsigned EXPECTED_HARQ_GRANTS = 1;
  ASSERT_EQ(EXPECTED_HARQ_GRANTS, slot_grid.result.ul.pucchs.size());
}

// Tests whether allocator skips PUCCH HARQ grant due to existing PUSCH grant.
TEST_F(test_pucch_harq_allocator_ded_resources, test_skip_pucch_with_pusch)
{
  add_pusch_alloc();
  t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, t_bench.k1);

  auto&          slot_grid            = t_bench.res_grid[t_bench.k0 + t_bench.k1];
  const unsigned EXPECTED_HARQ_GRANTS = 0;
  ASSERT_EQ(EXPECTED_HARQ_GRANTS, slot_grid.result.ul.pucchs.size());
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
  // Verify that the UE 0x4601 does not get allocated any HARQ grant.
  ASSERT_EQ(nullptr, test_pdu.pucch_pdu);
  // The 2 HARQ grants should belong to UE different from 0x4601.
  ASSERT_EQ(2, slot_grid.result.ul.pucchs.size());
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[0].crnti);
  ASSERT_NE(t_bench.get_main_ue().crnti, slot_grid.result.ul.pucchs[1].crnti);
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
