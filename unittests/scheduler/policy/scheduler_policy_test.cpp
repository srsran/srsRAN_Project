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

class dummy_allocator : public ue_pdsch_allocator
{
public:
  cell_slot_resource_allocator& slot_res_grid;
  optional<ue_pdsch_grant>      last_grant;

  dummy_allocator(cell_slot_resource_allocator& slot_res_grid_) : slot_res_grid(slot_res_grid_) {}

  bool allocate_dl_grant(const ue_pdsch_grant& grant) override
  {
    last_grant = grant;
    return true;
  }

  const cell_slot_resource_grid& dl_resource_grid(du_cell_index_t cell_index, unsigned k0) const override
  {
    return slot_res_grid.dl_res_grid;
  }
};

class scheduler_policy_test : public ::testing::TestWithParam<policy_type>
{
protected:
  scheduler_policy_test() : slot_res_grid(cell_cfg), alloc(slot_res_grid)
  {
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

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    dl_buffer_state_indication_message ind{};
    ind.ue_index = ue_index;
    ind.rnti     = ues[ue_index].crnti;
    ind.lcid     = lcid;
    ind.bs       = bytes;
    ues[ue_index].handle_dl_buffer_state_indication(ind);
  }

  scheduler_ue_expert_config expert_cfg = config_helpers::make_default_scheduler_expert_config().ue;
  cell_configuration         cell_cfg{test_helpers::make_default_sched_cell_configuration_request()};

  cell_slot_resource_allocator      slot_res_grid;
  dummy_allocator                   alloc;
  ue_list                           ues;
  std::unique_ptr<scheduler_policy> sched;
};

TEST_P(scheduler_policy_test, when_coreset0_used_then_dl_grant_is_within_bounds_of_coreset0_rbs)
{
  ue& u = add_ue(to_du_ue_index(0), {uint_to_lcid(4)});
  push_dl_bs(u.ue_index, uint_to_lcid(4), 100000000);

  sched->dl_sched(alloc, ues, false);

  ASSERT_TRUE(this->alloc.last_grant.has_value());
  ASSERT_EQ(this->alloc.last_grant->user->ue_index, u.ue_index);
  crb_interval crbs = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  ASSERT_EQ(this->alloc.last_grant->crbs, crbs);
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_policy_test, testing::Values(policy_type::time_rr));
