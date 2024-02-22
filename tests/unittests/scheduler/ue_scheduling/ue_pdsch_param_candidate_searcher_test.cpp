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
#include "lib/scheduler/ue_scheduling/ue_pdsch_param_candidate_searcher.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_pdsch_param_candidate_searcher_test : public ::testing::Test
{
protected:
  ue_pdsch_param_candidate_searcher_test() :
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

    // Create UE.
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

  void handle_harq_newtx(harq_id_t harq_id, unsigned k1 = 4)
  {
    const search_space_info& ss = ue_cc->cfg().search_space(to_search_space_id(2));

    pdsch_information pdsch{ue_ptr->crnti,
                            &ss.bwp->dl_common->generic_params,
                            ss.coreset,
                            vrb_alloc{vrb_interval{0, 5}},
                            ss.pdsch_time_domain_list[0].symbols,
                            {},
                            {},
                            0,
                            1,
                            false,
                            search_space_set_type::ue_specific,
                            dci_dl_format::f1_1,
                            harq_id,
                            nullopt};

    ue_cc->harqs.dl_harq(harq_id).new_tx(next_slot, k1, 4, 0, 15, 1);
    ue_cc->harqs.dl_harq(harq_id).save_alloc_params(srsran::dci_dl_rnti_config_type::c_rnti_f1_1, pdsch);
  }

  const scheduler_expert_config        sched_cfg = config_helpers::make_default_scheduler_expert_config();
  const scheduler_ue_expert_config&    expert_cfg{sched_cfg.ue};
  cell_common_configuration_list       cell_cfg_list;
  const cell_configuration&            cell_cfg;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;
  optional<ue_configuration>           ue_ded_cfg;

  srslog::basic_logger& logger;

  std::unique_ptr<ue> ue_ptr;
  ue_cell*            ue_cc = nullptr;

  slot_point next_slot;
};

TEST_F(ue_pdsch_param_candidate_searcher_test, when_no_pending_newtx_bytes_then_zero_candidates)
{
  ue_pdsch_param_candidate_searcher searcher(*ue_ptr, to_ue_cell_index(0), false, slot_point{0, 0}, logger);
  EXPECT_EQ(searcher.begin(), searcher.end());
}

TEST_F(ue_pdsch_param_candidate_searcher_test, when_no_pending_retxs_then_zero_candidates)
{
  ue_pdsch_param_candidate_searcher searcher2(*ue_ptr, to_ue_cell_index(0), true, slot_point{0, 0}, logger);
  EXPECT_EQ(searcher2.begin(), searcher2.end());
}

TEST_F(ue_pdsch_param_candidate_searcher_test, when_pending_newtx_bytes_then_there_are_newtx_candidates)
{
  ue_ptr->handle_dl_buffer_state_indication(dl_buffer_state_indication_message{ue_ptr->ue_index, uint_to_lcid(4), 100});

  ue_pdsch_param_candidate_searcher searcher(*ue_ptr, to_ue_cell_index(0), false, next_slot, logger);
  EXPECT_NE(searcher.begin(), searcher.end());

  std::set<harq_id_t>                                    harqs_visited;
  std::set<ue_pdsch_param_candidate_searcher::candidate> candidates_visited;
  for (const ue_pdsch_param_candidate_searcher::candidate& candidate : searcher) {
    harqs_visited.insert(candidate.harq().id);
    EXPECT_TRUE(candidate.harq().empty());
    ASSERT_EQ(&ue_cc->harqs.dl_harq(candidate.harq().id), &candidate.harq());
    EXPECT_EQ(&ue_cc->cfg().search_space(candidate.ss().cfg->get_id()), &candidate.ss());
    EXPECT_LT(candidate.pdsch_td_res_index(), candidate.ss().pdsch_time_domain_list.size());
    EXPECT_TRUE(candidates_visited.insert(candidate).second) << "Iterating through the same candidate twice";
  }
  ASSERT_EQ(harqs_visited.size(), 1) << "In case of newTx, we should not iterate through multiple HARQs";
}

TEST_F(ue_pdsch_param_candidate_searcher_test, when_harqs_with_pending_retx_exist_then_there_are_retx_candidates)
{
  unsigned              nof_rexts = test_rgen::uniform_int<unsigned>(1, 16);
  std::vector<unsigned> harq_ids(nof_rexts);
  std::iota(harq_ids.begin(), harq_ids.end(), 0);
  std::shuffle(harq_ids.begin(), harq_ids.end(), test_rgen::get());

  for (unsigned hid : harq_ids) {
    handle_harq_newtx(to_harq_id(hid));
  }
  dl_harq_process& first_h = ue_cc->harqs.dl_harq(to_harq_id(harq_ids[0]));

  while (first_h.slot_ack() != next_slot) {
    // Status: There shouldn't be candidates for reTx.
    ue_pdsch_param_candidate_searcher searcher(*ue_ptr, to_ue_cell_index(0), true, next_slot, logger);
    EXPECT_EQ(searcher.begin(), searcher.end());

    run_slot();
  }

  // Action: NACK the HARQs.
  for (unsigned hid : harq_ids) {
    ue_cc->harqs.dl_harq(to_harq_id(hid)).ack_info(0, srsran::mac_harq_ack_report_status::nack, nullopt);
    EXPECT_TRUE(ue_cc->harqs.dl_harq(to_harq_id(hid)).has_pending_retx());
  }

  // Status: There should be candidates for reTx.
  ue_pdsch_param_candidate_searcher searcher(*ue_ptr, to_ue_cell_index(0), true, next_slot, logger);
  EXPECT_NE(searcher.begin(), searcher.end());

  std::set<harq_id_t>                                    harqs_visited;
  std::set<ue_pdsch_param_candidate_searcher::candidate> candidates_visited;
  for (const ue_pdsch_param_candidate_searcher::candidate& candidate : searcher) {
    harqs_visited.insert(candidate.harq().id);
    EXPECT_TRUE(candidate.harq().has_pending_retx());
    ASSERT_EQ(&ue_cc->harqs.dl_harq(candidate.harq().id), &candidate.harq());
    EXPECT_EQ(&ue_cc->cfg().search_space(candidate.ss().cfg->get_id()), &candidate.ss());
    EXPECT_LT(candidate.pdsch_td_res_index(), candidate.ss().pdsch_time_domain_list.size());
    EXPECT_TRUE(candidates_visited.insert(candidate).second) << "Iterating through the same candidate twice";
  }
  ASSERT_EQ(harqs_visited.size(), nof_rexts) << "HARQ candidates should match the number of pending reTxs";
}
