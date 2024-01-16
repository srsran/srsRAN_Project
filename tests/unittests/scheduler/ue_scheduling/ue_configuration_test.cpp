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

#include "lib/scheduler/ue_scheduling/ue.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_configuration_test : public ::testing::Test
{
protected:
  scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message msg       = test_helpers::make_default_sched_cell_configuration_request();
  sched_ue_creation_request_message        ue_create_msg = test_helpers::create_default_sched_ue_creation_request();
  scheduler_harq_timeout_dummy_handler     harq_timeout_handler;

  cell_common_configuration_list cell_cfg_db;
};

TEST_F(ue_configuration_test, configuration_valid_on_creation)
{
  cell_configuration    cell_cfg{sched_cfg, msg};
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, (*ue_create_msg.cfg.cells)[0].serv_cell_cfg};

  // Test Common Config.
  TESTASSERT(ue_cfg.find_bwp(to_bwp_id(0)) != nullptr);
  TESTASSERT(ue_cfg.bwp(to_bwp_id(0)).dl_common->generic_params == cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  TESTASSERT(ue_cfg.coreset(to_coreset_id(0)).id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->id);
  TESTASSERT_EQ(0, ue_cfg.search_space(to_search_space_id(0)).cfg->get_id());
  TESTASSERT(*ue_cfg.search_space(to_search_space_id(0)).cfg ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0]);
  TESTASSERT_EQ(1, ue_cfg.search_space(to_search_space_id(1)).cfg->get_id());
  TESTASSERT(*ue_cfg.search_space(to_search_space_id(1)).cfg ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1]);

  // Test Dedicated Config.
  TESTASSERT(ue_cfg.find_coreset(to_coreset_id(2)) == nullptr);
  TESTASSERT_EQ(2, ue_cfg.search_space(to_search_space_id(2)).cfg->get_id());
  TESTASSERT(*ue_cfg.search_space(to_search_space_id(2)).cfg ==
             (*ue_create_msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]);
  TESTASSERT(ue_cfg.find_search_space(to_search_space_id(3)) == nullptr);
}

TEST_F(ue_configuration_test, configuration_valid_on_reconfiguration)
{
  cell_configuration    cell_cfg{sched_cfg, msg};
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, (*ue_create_msg.cfg.cells)[0].serv_cell_cfg};

  cell_config_dedicated ue_cell_reconf{};
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg.emplace();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.emplace_back();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back() = config_helpers::make_default_coreset_config();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back().id = to_coreset_id(2);

  ue_cfg.reconfigure(ue_cell_reconf.serv_cell_cfg);

  TESTASSERT(ue_cfg.find_coreset(to_coreset_id(2)) != nullptr);
  TESTASSERT_EQ(2, ue_cfg.coreset(to_coreset_id(2)).id);
  TESTASSERT(ue_cfg.coreset(to_coreset_id(2)) == ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back());
}

TEST_F(ue_configuration_test, when_reconfiguration_is_received_then_ue_updates_logical_channel_states)
{
  // Test Preamble.
  cell_cfg_db.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, msg));
  ue_configuration ue_ded_cfg{ue_create_msg.ue_index, ue_create_msg.crnti, cell_cfg_db, ue_create_msg.cfg};
  ue               u{ue_creation_command{ue_ded_cfg, ue_create_msg.starts_in_fallback, harq_timeout_handler}};

  // Pass Reconfiguration to UE with an new Logical Channel.
  sched_ue_reconfiguration_message recfg{};
  recfg.ue_index = ue_create_msg.ue_index;
  recfg.crnti    = ue_create_msg.crnti;
  recfg.cfg      = ue_create_msg.cfg;
  recfg.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(uint_to_lcid(4)));
  ue_configuration ue_ded_cfg2{ue_ded_cfg};
  ue_ded_cfg2.update(cell_cfg_db, recfg.cfg);
  u.handle_reconfiguration_request(ue_reconf_command{ue_ded_cfg2});
  dl_buffer_state_indication_message ind{recfg.ue_index, uint_to_lcid(4), 0};

  // Verify that DL buffer state indications affect newly active logical channels.
  for (const auto& lc : *recfg.cfg.lc_config_list) {
    if (lc.lcid == uint_to_lcid(0)) {
      // LCID0 is a special case.
      continue;
    }
    ind.lcid = lc.lcid;
    ind.bs   = 10;
    u.handle_dl_buffer_state_indication(ind);
    ASSERT_TRUE(u.pending_dl_newtx_bytes());
    ind.bs = 0;
    u.handle_dl_buffer_state_indication(ind);
    ASSERT_FALSE(u.pending_dl_newtx_bytes());
  }

  // Verify that inactive logical channels do not affect pending bytes.
  ind.lcid = uint_to_lcid(6);
  ind.bs   = 10;
  u.handle_dl_buffer_state_indication(ind);
  ASSERT_FALSE(u.pending_dl_newtx_bytes());
}

TEST_F(ue_configuration_test, search_spaces_pdcch_candidate_lists_does_not_surpass_limit)
{
  cell_config_builder_params params{};
  params.scs_common     = subcarrier_spacing::kHz30;
  params.dl_arfcn       = 520002;
  params.band           = nr_band::n41;
  params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz50;
  msg                   = test_helpers::make_default_sched_cell_configuration_request(params);
  ue_create_msg         = test_helpers::create_default_sched_ue_creation_request(params);

  auto&                        pdcch_cfg = *(*ue_create_msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg;
  const coreset_configuration& cset_cfg  = pdcch_cfg.coresets[0];
  search_space_configuration&  ss_cfg    = pdcch_cfg.search_spaces[0];
  ss_cfg.set_non_ss0_nof_candidates({config_helpers::compute_max_nof_candidates(aggregation_level::n1, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n8, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n16, cset_cfg)});

  cell_configuration cell_cfg{sched_cfg, msg};
  rnti_t crnti = to_rnti(test_rgen::uniform_int<uint16_t>(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI)));
  ue_cell_configuration ue_cfg{crnti, cell_cfg, (*ue_create_msg.cfg.cells)[0].serv_cell_cfg};

  const bwp_info& bwp            = ue_cfg.bwp(to_bwp_id(0));
  const unsigned  max_candidates = max_nof_monitored_pdcch_candidates(bwp.dl_common->generic_params.scs);

  unsigned       sfn = test_rgen::uniform_int<unsigned>(0, 1023);
  const unsigned slots_to_test =
      msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].get_monitoring_slot_periodicity();
  slot_point start_slot{params.scs_common, sfn, 0}, end_slot = start_slot + slots_to_test;
  for (slot_point pdcch_slot = start_slot; pdcch_slot != end_slot; ++pdcch_slot) {
    unsigned pdcch_candidates_count = 0;
    for (unsigned l = 0; l != NOF_AGGREGATION_LEVELS; ++l) {
      const aggregation_level aggr_lvl = aggregation_index_to_level(l);

      for (const search_space_info* ss : bwp.search_spaces) {
        ASSERT_GE(ss->cfg->get_nof_candidates()[l], ss->get_pdcch_candidates(aggr_lvl, pdcch_slot).size())
            << "The generated candidates cannot exceed the number of candidates passed in the SearchSpace config";

        pdcch_candidates_count += ss->get_pdcch_candidates(aggr_lvl, pdcch_slot).size();
      }
    }

    // The number of PDCCH candidates in each SearchSpace must not exceed the max number of PDCCH candidates for the
    // given numerology as per TS 38.213 Table 10.1-2.
    ASSERT_LE(pdcch_candidates_count, max_candidates);
  }
}
