/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../test_utils/dummy_test_components.h"
#include "../test_utils/sched_random_utils.h"
#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/ue_scheduling/ue_pdsch_alloc_param_candidate_searcher.h"
#include "lib/scheduler/ue_scheduling/ue_pusch_alloc_param_candidate_searcher.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_pxsch_alloc_param_candidate_searcher_test : public ::testing::Test
{
protected:
  ue_pxsch_alloc_param_candidate_searcher_test() :
    cell_cfg(*[this]() {
      auto cell_req = sched_config_helper::make_default_sched_cell_configuration_request();
      cfg_pool.add_cell(cell_req);
      cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, cell_req));
      return cell_cfg_list[to_du_cell_index(0)].get();
    }()),
    logger(srslog::fetch_basic_logger("SCHED", true)),
    next_slot(test_helper::generate_random_slot_point(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs))
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    sched_ue_creation_request_message ue_creation_req = sched_config_helper::create_default_sched_ue_creation_request();
    ue_creation_req.ue_index                          = to_du_ue_index(0);
    ue_creation_req.crnti                             = to_rnti(0x4601 + (unsigned)ue_creation_req.ue_index);
    for (lcid_t lcid : std::array<lcid_t, 3>{uint_to_lcid(1), uint_to_lcid(2), uint_to_lcid(4)}) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ue_ded_cfg.emplace(
        ue_creation_req.ue_index, ue_creation_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_creation_req));
    ue_ptr = std::make_unique<ue>(ue_creation_command{*ue_ded_cfg, ue_creation_req.starts_in_fallback, cell_harqs});
    ue_cc  = &ue_ptr->get_cell(to_ue_cell_index(0));
  }

  slot_point get_next_ul_slot(slot_point start_slot)
  {
    slot_point next_ul_slot = start_slot + sched_cfg.ue.min_k1;
    while (not cell_cfg.is_fully_ul_enabled(next_ul_slot)) {
      ++next_ul_slot;
    }
    return next_ul_slot;
  }

  void run_slot() { next_slot++; }

  const scheduler_expert_config     sched_cfg = config_helpers::make_default_scheduler_expert_config();
  const scheduler_ue_expert_config& expert_cfg{sched_cfg.ue};
  du_cell_group_config_pool         cfg_pool;
  cell_common_configuration_list    cell_cfg_list;
  const cell_configuration&         cell_cfg;
  cell_harq_manager cell_harqs{1, MAX_NOF_HARQS, std::make_unique<scheduler_harq_timeout_dummy_notifier>()};

  std::optional<ue_configuration> ue_ded_cfg;

  srslog::basic_logger& logger;

  std::unique_ptr<ue> ue_ptr;
  ue_cell*            ue_cc = nullptr;

  slot_point next_slot;
};

TEST_F(ue_pxsch_alloc_param_candidate_searcher_test, only_searchspaces_in_ue_dedicated_cfg_is_considered)
{
  const harq_id_t  h_id = to_harq_id(0);
  const slot_point pdcch_slot{0, 0};
  const auto&      ss_list = ue_cc->cfg().bwp(to_bwp_id(0)).search_spaces;

  ue_pdsch_alloc_param_candidate_searcher dl_searcher(
      *ue_ptr, to_du_cell_index(0), ue_cc->harqs.dl_harq(h_id), pdcch_slot, pdcch_slot);
  ASSERT_TRUE(not dl_searcher.is_empty());
  for (const auto& candidate : dl_searcher) {
    ASSERT_TRUE(ss_list.contains(candidate.ss().cfg->get_id()));
  }
  ue_pusch_alloc_param_candidate_searcher ul_searcher(
      *ue_ptr, to_du_cell_index(0), ue_cc->harqs.ul_harq(h_id), pdcch_slot, get_next_ul_slot(pdcch_slot));
  ASSERT_TRUE(not ul_searcher.is_empty());
  for (const auto& candidate : ul_searcher) {
    bool ss_present_in_ue_ded_cfg =
        std::find_if(ss_list.begin(), ss_list.end(), [&candidate](const search_space_configuration& ss_cfg) {
          return ss_cfg.get_id() == candidate.ss().cfg->get_id();
        }) != ss_list.end();
    ASSERT_TRUE(ss_present_in_ue_ded_cfg);
  }
}

TEST_F(ue_pxsch_alloc_param_candidate_searcher_test, only_candidates_for_given_pusch_slot_is_returned)
{
  const harq_id_t  h_id = to_harq_id(0);
  const slot_point pdcch_slot{0, 0};
  const slot_point pusch_slot = get_next_ul_slot(pdcch_slot);

  ue_pusch_alloc_param_candidate_searcher ul_searcher(
      *ue_ptr, to_du_cell_index(0), ue_cc->harqs.ul_harq(h_id), pdcch_slot, pusch_slot);
  ASSERT_TRUE(not ul_searcher.is_empty());
  for (const auto& candidate : ul_searcher) {
    ASSERT_EQ(pdcch_slot + candidate.pusch_td_res().k2, pusch_slot) << "Candidate is not for the given PUSCH slot";
  }
}
