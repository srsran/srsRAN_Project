/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "unittests/scheduler/test_utils/config_generators.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsgnb/ran/duplex_mode.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

// Parameters to be passed to test.
struct srb0_test_params {
  uint8_t k0;
  uint8_t k1;
};

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  scheduler_ue_expert_config    expert_cfg;
  cell_configuration            cell_cfg;
  cell_resource_allocator       res_grid;
  pdcch_resource_allocator_impl pdcch_sch;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  ue_list                       ue_db;
  ue_cell_grid_allocator        ue_alloc;
  ue_srb0_scheduler             srb0_sched;

  explicit test_bench(const scheduler_ue_expert_config&               expert_cfg_,
                      const sched_cell_configuration_request_message& cell_req =
                          test_helpers::make_default_sched_cell_configuration_request()) :
    expert_cfg{expert_cfg_},
    cell_cfg{cell_req},
    res_grid{cell_cfg},
    pdcch_sch{cell_cfg},
    pucch_alloc{cell_cfg},
    uci_alloc{pucch_alloc},
    ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED", true)),
    srb0_sched(expert_cfg, cell_cfg, pdcch_sch, pucch_alloc, ue_db)
  {
  }
};

class srb0_scheduler_tester : public ::testing::TestWithParam<srb0_test_params>
{
protected:
  slot_point            next_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<test_bench>  bench;
  srb0_test_params      params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  srb0_scheduler_tester() : params{GetParam()} {};

  ~srb0_scheduler_tester() override
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_ue_expert_config&               expert_cfg,
                   const sched_cell_configuration_request_message& msg =
                       test_helpers::make_default_sched_cell_configuration_request())
  {
    bench.emplace(expert_cfg, msg);

    const auto& dl_lst = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }
  }

  void run_slot()
  {
    mac_logger.set_context(next_slot.sfn(), next_slot.slot_index());
    test_logger.set_context(next_slot.sfn(), next_slot.slot_index());

    bench->res_grid.slot_indication(next_slot);
    bench->pdcch_sch.slot_indication(next_slot);
    next_slot++;

    bench->srb0_sched.run_slot(bench->res_grid);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  scheduler_ue_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index) const
  {
    return scheduler_ue_expert_config{10, 10, 4, max_msg4_mcs_index};
  }

  sched_cell_configuration_request_message create_random_cell_config_request(duplex_mode mode) const
  {
    sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = params.k0;

    if (mode == duplex_mode::TDD) {
      msg.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common();
    }
    return msg;
  }

  const pdsch_time_domain_resource_allocation& get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
  {
    return bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
  }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() { return bench->res_grid[0].result.dl.dl_pdcchs; }

  static bool ue_is_allocated_pdcch(span<const pdcch_dl_information> dl_pdcchs, const ue& u)
  {
    if (dl_pdcchs.empty()) {
      return false;
    }
    return std::any_of(
        dl_pdcchs.begin(), dl_pdcchs.end(), [&u](const auto& pdcch) { return pdcch.ctx.rnti == u.crnti; });
  }

  bool ue_is_allocated_pdsch(const ue& u)
  {
    // Search valid PDSCH time domain resource.
    const auto& bwp_cfg_common = bench->cell_cfg.dl_cfg_common.init_dl_bwp;
    for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);

      // Fetch PDSCH resource grid allocators.
      cell_slot_resource_allocator& pdsch_alloc = bench->res_grid[pdsch_td_cfg.k0];
      // Search for PDSCH UE grant.
      for (const auto& grant : pdsch_alloc.result.dl.ue_grants) {
        if (grant.pdsch_cfg.rnti == u.crnti) {
          return true;
        }
      }
    }
    return false;
  }

  bool tbs_scheduled_bytes_matches_given_size(const ue& u, unsigned exp_size)
  {
    // Search valid PDSCH time domain resource.
    const auto& bwp_cfg_common         = bench->cell_cfg.dl_cfg_common.init_dl_bwp;
    unsigned    total_cw_tb_size_bytes = 0;
    for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);

      // Fetch PDSCH resource grid allocators.
      cell_slot_resource_allocator& pdsch_alloc = bench->res_grid[pdsch_td_cfg.k0];
      // Search for PDSCH UE grant.
      for (const auto& grant : pdsch_alloc.result.dl.ue_grants) {
        if (grant.pdsch_cfg.rnti != u.crnti) {
          continue;
        }
        for (const auto& cw : grant.pdsch_cfg.codewords) {
          total_cw_tb_size_bytes += cw.tb_size_bytes;
        }
      }
    }
    return total_cw_tb_size_bytes >= exp_size;
  }

  bool ue_is_allocated_pucch(const ue& u)
  {
    // Search valid PDSCH time domain resource.
    const auto& bwp_cfg_common = bench->cell_cfg.dl_cfg_common.init_dl_bwp;
    for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);

      // Fetch PUCCH resource grid allocators.
      cell_slot_resource_allocator& pucch_alloc = bench->res_grid[pdsch_td_cfg.k0 + params.k1];
      // Search for PUCCH grant.
      for (const auto& grant : pucch_alloc.result.ul.pucchs) {
        if (grant.crnti == u.crnti) {
          return true;
        }
      }
    }
    return false;
  }

  bool add_ue(rnti_t tc_rnti, du_ue_index_t ue_index)
  {
    // Add cell to UE cell grid allocator.
    bench->ue_alloc.add_cell(bench->cell_cfg.cell_index, bench->pdcch_sch, bench->uci_alloc, bench->res_grid);
    auto ue_create_req     = test_helpers::create_default_sched_ue_creation_request();
    ue_create_req.crnti    = tc_rnti;
    ue_create_req.ue_index = ue_index;
    // Add UE to UE DB.
    auto u = std::make_unique<ue>(bench->expert_cfg, bench->cell_cfg, ue_create_req);
    if (bench->ue_db.contains(ue_index)) {
      // UE already exists.
      return false;
    }
    bench->ue_db.insert(ue_index, std::move(u));
    return true;
  }

  void push_buffer_state_to_dl_ue(du_ue_index_t ue_idx, unsigned buffer_size)
  {
    // Notification from upper layers of DL buffer state.
    dl_buffer_state_indication_message msg{ue_idx, bench->ue_db[ue_idx].crnti, LCID_SRB0, buffer_size};
    bench->ue_db[ue_idx].handle_dl_buffer_state_indication(msg);

    // Notify scheduler of DL buffer state.
    bench->srb0_sched.handle_dl_buffer_state_indication(ue_idx);
  }

  unsigned get_pending_bytes(du_ue_index_t ue_idx) { return bench->ue_db[ue_idx].pending_dl_srb0_newtx_bytes(); }

  const ue& get_ue(du_ue_index_t ue_idx) { return bench->ue_db[ue_idx]; }
};

TEST_P(srb0_scheduler_tester, successfully_allocated_resources)
{
  setup_sched(create_expert_config(1),
              create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  unsigned mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  unsigned exp_size = get_pending_bytes(to_du_ue_index(0));

  run_slot();

  // Test the following:
  // 1. Check for DCI_1_0 allocation for SRB0 on PDCCH.
  // 2. Check for PDSCH allocation.
  // 3. Check whether CW TB bytes matches with pending bytes to be sent.
  ASSERT_TRUE(ue_is_allocated_pdcch(scheduled_dl_pdcchs(), get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(ue_is_allocated_pdsch(get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(get_ue(to_du_ue_index(0)), exp_size));
}

TEST_P(srb0_scheduler_tester, failed_allocating_resources)
{
  setup_sched(create_expert_config(2),
              create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  // Note: This step is for filling resource grid so that no other UE's SRB0 message can be scheduled.
  unsigned ue1_mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), ue1_mac_srb0_sdu_size);

  // Add UE 2.
  add_ue(to_rnti(0x4602), to_du_ue_index(1));
  // Notify about SRB0 message in DL of size 101 bytes.
  unsigned ue2_mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(1), ue2_mac_srb0_sdu_size);

  run_slot();

  // Allocation for UE2 should fail.
  ASSERT_FALSE(ue_is_allocated_pdcch(scheduled_dl_pdcchs(), get_ue(to_du_ue_index(1))));
  ASSERT_FALSE(ue_is_allocated_pdsch(get_ue(to_du_ue_index(1))));
}

TEST_P(srb0_scheduler_tester, test_large_srb0_buffer_size)
{
  setup_sched(create_expert_config(27),
              create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 458 bytes.
  unsigned mac_srb0_sdu_size = 458;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  unsigned exp_size = get_pending_bytes(to_du_ue_index(0));

  run_slot();

  ASSERT_TRUE(ue_is_allocated_pdcch(scheduled_dl_pdcchs(), get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(ue_is_allocated_pdsch(get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(get_ue(to_du_ue_index(0)), exp_size));
}

TEST_P(srb0_scheduler_tester, test_srb0_buffer_size_exceeding_max_msg4_mcs_index)
{
  setup_sched(create_expert_config(3),
              create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 360 bytes which requires MCS index > 3.
  unsigned mac_srb0_sdu_size = 360;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  run_slot();

  // Allocation for UE should fail.
  ASSERT_FALSE(ue_is_allocated_pdcch(scheduled_dl_pdcchs(), get_ue(to_du_ue_index(0))));
  ASSERT_FALSE(ue_is_allocated_pdsch(get_ue(to_du_ue_index(0))));
}

TEST_P(srb0_scheduler_tester, sanity_check_with_random_max_mcs_and_payload_size)
{
  const sch_mcs_index max_msg4_mcs = get_random_uint(0, 27);
  setup_sched(create_expert_config(max_msg4_mcs),
              create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Random payload size.
  unsigned mac_srb0_sdu_size = get_random_uint(1, 458);
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  srslog::basic_logger& logger(srslog::fetch_basic_logger("TEST"));
  logger.info("SRB0 scheduler sanity test params PDU size ({}), max msg4 mcs ({}).", mac_srb0_sdu_size, max_msg4_mcs);

  run_slot();
}

TEST_P(srb0_scheduler_tester, test_msg4_successful_allocation_with_custom_cell_config)
{
  auto cell_cfg = create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD);

  // Modify cell configuration (reproducing msg4 crash scenario in du_example)
  cell_cfg.pci                          = 69;
  cell_cfg.dl_carrier.arfcn             = 536020;
  cell_cfg.ssb_config.offset_to_point_A = 40;
  cell_cfg.ssb_config.k_ssb             = 6;
  cell_cfg.coreset0                     = 9;

  setup_sched(create_expert_config(0), cell_cfg);

  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 129 bytes.
  unsigned mac_srb0_sdu_size = 129;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  unsigned exp_size = get_pending_bytes(to_du_ue_index(0));

  run_slot();

  // Test the following:
  // 1. Check for DCI_1_0 allocation for SRB0 on PDCCH.
  // 2. Check for PDSCH allocation.
  // 3. Check whether CW TB bytes matches with pending bytes to be sent.
  ASSERT_TRUE(ue_is_allocated_pdcch(scheduled_dl_pdcchs(), get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(ue_is_allocated_pdsch(get_ue(to_du_ue_index(0))));
  ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(get_ue(to_du_ue_index(0)), exp_size));
}

INSTANTIATE_TEST_SUITE_P(srb0_scheduler,
                         srb0_scheduler_tester,
                         testing::Values(srb0_test_params{.k0 = 1, .k1 = 4}, srb0_test_params{.k0 = 2, .k1 = 4}));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  (void)(::testing::GTEST_FLAG(death_test_style) = "fast");

  return RUN_ALL_TESTS();
}
