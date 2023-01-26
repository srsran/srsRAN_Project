/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../test_utils/config_generators.h"
#include "lib/scheduler/policy/scheduler_time_rr.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/support/error_handling.h"
#include <gtest/gtest.h>

using namespace srsgnb;

enum class policy_type { time_rr };

class dummy_pdsch_allocator : public ue_pdsch_allocator
{
public:
  cell_slot_resource_allocator& slot_res_grid;
  optional<ue_pdsch_grant>      last_grant;

  dummy_pdsch_allocator(cell_slot_resource_allocator& slot_res_grid_) : slot_res_grid(slot_res_grid_) {}

  bool allocate_dl_grant(const ue_pdsch_grant& grant) override
  {
    last_grant = grant;
    return true;
  }
};

class dummy_pusch_allocator : public ue_pusch_allocator
{
public:
  cell_slot_resource_allocator& slot_res_grid;
  optional<ue_pusch_grant>      last_grant;

  dummy_pusch_allocator(cell_slot_resource_allocator& slot_res_grid_) : slot_res_grid(slot_res_grid_) {}

  bool allocate_ul_grant(const ue_pusch_grant& grant) override
  {
    last_grant = grant;
    return true;
  }
};

class scheduler_policy_test : public ::testing::TestWithParam<policy_type>
{
protected:
  scheduler_policy_test() : res_grid(cell_cfg), pdsch_alloc(res_grid[0]), pusch_alloc(res_grid[0])
  {
    ue_res_grid.add_cell(res_grid);

    switch (GetParam()) {
      case policy_type::time_rr:
        sched = std::make_unique<scheduler_time_rr>();
        break;
      default:
        report_fatal_error("Invalid policy");
    }
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.ue_index                          = ue_index;
    ue_creation_req.crnti                             = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list.push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ues.emplace(ue_index, expert_cfg, cell_cfg, ue_creation_req);

    return ues[ue_index];
  }

  ue& add_ue(du_ue_index_t                        ue_index,
             const std::initializer_list<lcid_t>& lcids_to_activate,
             lcg_id_t                             lcg_id,
             sched_ue_creation_request_message&   ue_req)
  {
    ue_req.ue_index         = ue_index;
    ue_req.crnti            = to_rnti(0x4601 + (unsigned)ue_index);
    auto default_lc_cfg     = config_helpers::create_default_logical_channel_config(uint_to_lcid(0));
    default_lc_cfg.lc_group = lcg_id;
    for (lcid_t lcid : lcids_to_activate) {
      default_lc_cfg.lcid = lcid;
      ue_req.cfg.lc_config_list.push_back(default_lc_cfg);
    }
    ues.emplace(ue_index, expert_cfg, cell_cfg, ue_req);

    return ues[ue_index];
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    dl_buffer_state_indication_message ind{};
    ind.ue_index = ue_index;
    ind.lcid     = lcid;
    ind.bs       = bytes;
    ues[ue_index].handle_dl_buffer_state_indication(ind);
  }

  void notify_ul_bsr(du_ue_index_t ue_index, lcg_id_t lcg_id, unsigned bytes)
  {
    // Assumptions:
    // - Only one LCG is assumed to have data to send.
    // - BSR is Short BSR.
    ul_bsr_indication_message msg{
        to_du_cell_index(0), ue_index, to_rnti(0x4601 + (unsigned)ue_index), bsr_format::SHORT_BSR, {}};
    msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, bytes});

    ues[ue_index].handle_bsr_indication(msg);
  }

  scheduler_ue_expert_config expert_cfg = config_helpers::make_default_scheduler_expert_config().ue;
  cell_configuration         cell_cfg{test_helpers::make_default_sched_cell_configuration_request()};

  cell_resource_allocator           res_grid;
  ue_resource_grid_view             ue_res_grid;
  dummy_pdsch_allocator             pdsch_alloc;
  dummy_pusch_allocator             pusch_alloc;
  ue_list                           ues;
  std::unique_ptr<scheduler_policy> sched;
};

TEST_P(scheduler_policy_test, when_coreset0_used_then_dl_grant_is_within_bounds_of_coreset0_rbs)
{
  ue& u = add_ue(to_du_ue_index(0), {uint_to_lcid(4)});
  push_dl_bs(u.ue_index, uint_to_lcid(4), 100000000);

  sched->dl_sched(pdsch_alloc, ue_res_grid, ues, false);

  ASSERT_TRUE(this->pdsch_alloc.last_grant.has_value());
  ASSERT_EQ(this->pdsch_alloc.last_grant->user->ue_index, u.ue_index);
  crb_interval crbs = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  ASSERT_EQ(this->pdsch_alloc.last_grant->crbs, crbs);
}

TEST_P(scheduler_policy_test, scheduler_favors_ss_with_higher_nof_candidates_for_aggr_lvl)
{
  sched_ue_creation_request_message ue_req = test_helpers::create_default_sched_ue_creation_request();
  // Default CORESET(s) and SearchSpace(s) configurations are as follows:
  // CS#0 - PRBs [0, 48)
  // CS#1 - PRBs [0, 36)
  // SS#0 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 0], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#0
  // SS#1 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 2], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#0
  // SS#2 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 2], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#1

  // NOTE: Policy scheduler currently uses fixed aggr. lvl of 4 so test is based on nof. PDCCH candidates for aggr.
  // lvl. 4.

  // Modify SS#2 to have more nof. candidates for aggregation level 4.
  auto& ss_list = ue_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().search_spaces;
  for (auto& ss : ss_list) {
    if (ss.id == to_search_space_id(2)) {
      ss.nof_candidates = {0, 0, 2, 0};
      break;
    }
  }

  const ue& u = add_ue(to_du_ue_index(0), {uint_to_lcid(4)}, uint_to_lcg_id(1), ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(4), 1053);

  sched->dl_sched(pdsch_alloc, ue_res_grid, ues, false);

  ASSERT_TRUE(this->pdsch_alloc.last_grant.has_value());
  ASSERT_EQ(this->pdsch_alloc.last_grant->user->ue_index, u.ue_index);
  ASSERT_EQ(this->pdsch_alloc.last_grant->ss_id, to_search_space_id(2));

  notify_ul_bsr(u.ue_index, uint_to_lcg_id(1), 1053);
  sched->ul_sched(pusch_alloc, ue_res_grid, ues, false);

  ASSERT_TRUE(this->pusch_alloc.last_grant.has_value());
  ASSERT_EQ(this->pusch_alloc.last_grant->user->ue_index, u.ue_index);
  ASSERT_EQ(this->pusch_alloc.last_grant->ss_id, to_search_space_id(2));
}

TEST_P(scheduler_policy_test, scheduler_favors_coreset_gt_0_when_ss_has_equal_nof_candidates_for_aggr_lvl)
{
  sched_ue_creation_request_message ue_req = test_helpers::create_default_sched_ue_creation_request();
  // Default CORESET(s) and SearchSpace(s) configurations are as follows:
  // CS#0 - PRBs [0, 48)
  // CS#1 - PRBs [0, 36)
  // SS#0 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 0], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#0
  // SS#1 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 2], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#0
  // SS#2 - Nof. Candidates at Aggregation level [ [0 -> 0], [2 -> 2], [4 -> 1], [8 -> 0], [16 -> 0] ] - CS#1

  // NOTE: Policy scheduler currently uses fixed aggr. lvl of 4 so test is based on nof. PDCCH candidates for aggr.
  // lvl. 4.

  const ue& u = add_ue(to_du_ue_index(0), {uint_to_lcid(5)}, uint_to_lcg_id(2), ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(5), 1053);

  sched->dl_sched(pdsch_alloc, ue_res_grid, ues, false);

  ASSERT_TRUE(this->pdsch_alloc.last_grant.has_value());
  ASSERT_EQ(this->pdsch_alloc.last_grant->user->ue_index, u.ue_index);
  // Scheduler choose SS#2 since nof. candidates in all SearchSpaces for Aggr. lvl 4 are equal.
  ASSERT_EQ(this->pdsch_alloc.last_grant->ss_id, to_search_space_id(2));

  notify_ul_bsr(u.ue_index, uint_to_lcg_id(2), 1053);
  sched->ul_sched(pusch_alloc, ue_res_grid, ues, false);

  ASSERT_TRUE(this->pusch_alloc.last_grant.has_value());
  ASSERT_EQ(this->pusch_alloc.last_grant->user->ue_index, u.ue_index);
  // Scheduler choose SS#2 since nof. candidates in all SearchSpaces for Aggr. lvl 4 are equal.
  ASSERT_EQ(this->pusch_alloc.last_grant->ss_id, to_search_space_id(2));
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_policy_test, testing::Values(policy_type::time_rr));
