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
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

enum class policy_type { time_rr };

class dummy_pdsch_allocator : public ue_pdsch_allocator
{
public:
  cell_slot_resource_allocator& slot_res_grid;
  std::vector<ue_pdsch_grant>   last_grants;

  dummy_pdsch_allocator(cell_slot_resource_allocator& slot_res_grid_) : slot_res_grid(slot_res_grid_) {}

  bool allocate_dl_grant(const ue_pdsch_grant& grant) override
  {
    last_grants.push_back(grant);
    return true;
  }
};

class dummy_pusch_allocator : public ue_pusch_allocator
{
public:
  cell_slot_resource_allocator& slot_res_grid;
  std::vector<ue_pusch_grant>   last_grants;

  dummy_pusch_allocator(cell_slot_resource_allocator& slot_res_grid_) : slot_res_grid(slot_res_grid_) {}

  bool allocate_ul_grant(const ue_pusch_grant& grant) override
  {
    last_grants.push_back(grant);
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

  void run_slot()
  {
    res_grid.slot_indication(next_slot);

    unsigned dl_idx = next_slot.to_uint() % 2;
    for (unsigned i = 0; i != 2; ++i) {
      if (dl_idx == i) {
        sched->dl_sched(pdsch_alloc, ue_res_grid, ues, true);
        sched->dl_sched(pdsch_alloc, ue_res_grid, ues, false);
      } else {
        sched->ul_sched(pusch_alloc, ue_res_grid, ues, true);
        sched->ul_sched(pusch_alloc, ue_res_grid, ues, false);
      }
    }

    next_slot++;
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        make_ue_create_req(ue_index, to_rnti(0x4601 + (unsigned)ue_index), lcids_to_activate, uint_to_lcg_id(0));
    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_req)
  {
    ues.emplace(ue_req.ue_index, expert_cfg, cell_cfg, ue_req);
    return ues[ue_req.ue_index];
  }

  sched_ue_creation_request_message make_ue_create_req(du_ue_index_t                        ue_index,
                                                       rnti_t                               rnti,
                                                       const std::initializer_list<lcid_t>& lcids_to_activate,
                                                       lcg_id_t                             lcg_id)
  {
    sched_ue_creation_request_message req = test_helpers::create_default_sched_ue_creation_request();
    req.ue_index                          = ue_index;
    req.crnti                             = rnti;
    auto default_lc_cfg                   = config_helpers::create_default_logical_channel_config(uint_to_lcid(0));
    default_lc_cfg.lc_group               = lcg_id;
    for (lcid_t lcid : lcids_to_activate) {
      default_lc_cfg.lcid = lcid;
      req.cfg.lc_config_list.push_back(default_lc_cfg);
    }
    return req;
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
  slot_point                        next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
};

TEST_P(scheduler_policy_test, when_coreset0_used_then_dl_grant_is_within_bounds_of_coreset0_rbs)
{
  auto ue_req = make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(0));
  ue_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces.clear();
  ue& u = add_ue(ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(4), 100000000);

  run_slot();

  ASSERT_FALSE(this->pdsch_alloc.last_grants.empty());
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].user->ue_index, u.ue_index);
  crb_interval crbs = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].crbs, crbs);
}

TEST_P(scheduler_policy_test, scheduler_favors_ss_with_higher_nof_candidates_for_aggr_lvl)
{
  sched_ue_creation_request_message ue_req =
      make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(1));
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

  const ue& u = add_ue(ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(4), 1053);
  notify_ul_bsr(u.ue_index, uint_to_lcg_id(1), 1053);

  run_slot();

  ASSERT_FALSE(this->pdsch_alloc.last_grants.empty());
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].user->ue_index, u.ue_index);
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].ss_id, to_search_space_id(2));

  ASSERT_FALSE(this->pusch_alloc.last_grants.empty());
  ASSERT_EQ(this->pusch_alloc.last_grants[0].user->ue_index, u.ue_index);
  ASSERT_EQ(this->pusch_alloc.last_grants[0].ss_id, to_search_space_id(2));
}

TEST_P(scheduler_policy_test, scheduler_favors_coreset_gt_0_when_ss_has_equal_nof_candidates_for_aggr_lvl)
{
  sched_ue_creation_request_message ue_req =
      make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, uint_to_lcg_id(2));

  const ue& u = add_ue(ue_req);

  push_dl_bs(u.ue_index, uint_to_lcid(5), 1053);
  notify_ul_bsr(u.ue_index, uint_to_lcg_id(2), 1053);

  run_slot();

  ASSERT_FALSE(this->pdsch_alloc.last_grants.empty());
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].user->ue_index, u.ue_index);
  // Scheduler choose SS#2 since nof. candidates in all SearchSpaces for Aggr. lvl 4 are equal.
  ASSERT_EQ(this->pdsch_alloc.last_grants[0].ss_id, to_search_space_id(2));

  ASSERT_FALSE(this->pusch_alloc.last_grants.empty());
  ASSERT_EQ(this->pusch_alloc.last_grants[0].user->ue_index, u.ue_index);
  // Scheduler choose SS#2 since nof. candidates in all SearchSpaces for Aggr. lvl 4 are equal.
  ASSERT_EQ(this->pusch_alloc.last_grants[0].ss_id, to_search_space_id(2));
}

TEST_P(scheduler_policy_test, scheduler_allocates_more_than_one_ue_in_case_their_bsr_is_low)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  notify_ul_bsr(u1.ue_index, lcg_id, 10);
  notify_ul_bsr(u2.ue_index, lcg_id, 10);

  run_slot();

  ASSERT_EQ(pusch_alloc.last_grants.size(), 2);
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_policy_test, testing::Values(policy_type::time_rr));
