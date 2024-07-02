/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../test_utils/config_generators.h"
#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/ue_scheduling/ue_pdsch_alloc_param_candidate_searcher.h"
#include "lib/scheduler/ue_scheduling/ue_pusch_alloc_param_candidate_searcher.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_pxsch_alloc_param_candidate_searcher_test : public ::testing::Test
{
protected:
  ue_pxsch_alloc_param_candidate_searcher_test() :
    cell_cfg(*[this]() {
      cell_cfg_list.emplace(to_du_cell_index(0),
                            std::make_unique<cell_configuration>(
                                sched_cfg, test_helpers::make_default_sched_cell_configuration_request()));
      return cell_cfg_list[to_du_cell_index(0)].get();
    }()),
    logger(srslog::fetch_basic_logger("SCHED", true)),
    next_slot(test_helpers::generate_random_slot_point(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs))
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.ue_index                          = to_du_ue_index(0);
    ue_creation_req.crnti                             = to_rnti(0x4601 + (unsigned)ue_creation_req.ue_index);
    for (lcid_t lcid : std::array<lcid_t, 3>{uint_to_lcid(1), uint_to_lcid(2), uint_to_lcid(4)}) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ue_ded_cfg.emplace(ue_creation_req.ue_index, ue_creation_req.crnti, cell_cfg_list, ue_creation_req.cfg);
    ue_ptr = std::make_unique<ue>(
        ue_creation_command{*ue_ded_cfg, ue_creation_req.starts_in_fallback, harq_timeout_handler});
    ue_cc = &ue_ptr->get_cell(to_ue_cell_index(0));
  }

  void run_slot() { next_slot++; }

  const scheduler_expert_config        sched_cfg = config_helpers::make_default_scheduler_expert_config();
  const scheduler_ue_expert_config&    expert_cfg{sched_cfg.ue};
  cell_common_configuration_list       cell_cfg_list;
  const cell_configuration&            cell_cfg;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;
  std::optional<ue_configuration>      ue_ded_cfg;

  srslog::basic_logger& logger;

  std::unique_ptr<ue> ue_ptr;
  ue_cell*            ue_cc = nullptr;

  slot_point next_slot;
};

TEST_F(ue_pxsch_alloc_param_candidate_searcher_test, only_searchspaces_in_ue_dedicated_cfg_is_considered)
{
  const harq_id_t                        h_id    = to_harq_id(0);
  span<const search_space_configuration> ss_list = ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces;

  ue_pdsch_alloc_param_candidate_searcher dl_searcher(
      *ue_ptr, to_du_cell_index(0), ue_cc->harqs.dl_harq(h_id), slot_point{0, 0}, {});
  ASSERT_TRUE(not dl_searcher.is_empty());
  for (const auto& candidate : dl_searcher) {
    bool ss_present_in_ue_ded_cfg =
        std::find_if(ss_list.begin(), ss_list.end(), [&candidate](const search_space_configuration& ss_cfg) {
          return ss_cfg.get_id() == candidate.ss().cfg->get_id();
        }) != ss_list.end();
    ASSERT_TRUE(ss_present_in_ue_ded_cfg);
  }
  ue_pusch_alloc_param_candidate_searcher ul_searcher(
      *ue_ptr, to_du_cell_index(0), ue_cc->harqs.ul_harq(h_id), slot_point{0, 0}, {});
  ASSERT_TRUE(not dl_searcher.is_empty());
  for (const auto& candidate : ul_searcher) {
    bool ss_present_in_ue_ded_cfg =
        std::find_if(ss_list.begin(), ss_list.end(), [&candidate](const search_space_configuration& ss_cfg) {
          return ss_cfg.get_id() == candidate.ss().cfg->get_id();
        }) != ss_list.end();
    ASSERT_TRUE(ss_present_in_ue_ded_cfg);
  }
}
