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

#include "lib/scheduler/ue_context/ue.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"
#include "srsran/support/srsran_test.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_configuration_test : public ::testing::Test
{
protected:
  scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message msg = sched_config_helper::make_default_sched_cell_configuration_request();
  sched_ue_creation_request_message ue_create_msg = sched_config_helper::create_default_sched_ue_creation_request();

  cell_common_configuration_list cell_cfg_db;
  du_cell_group_config_pool      cfg_pool;
  cell_harq_manager cell_harqs{1, MAX_NOF_HARQS, std::make_unique<scheduler_harq_timeout_dummy_notifier>()};
};

TEST_F(ue_configuration_test, configuration_valid_on_creation)
{
  cell_configuration cell_cfg{sched_cfg, msg};
  cfg_pool.add_cell(msg);
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[to_du_cell_index(0)]};

  // Test Common Config.
  ASSERT_TRUE(ue_cfg.find_bwp(to_bwp_id(0)) != nullptr);
  ASSERT_TRUE(ue_cfg.bwp(to_bwp_id(0)).dl_common->value().generic_params ==
              cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  ASSERT_TRUE(ue_cfg.coreset(to_coreset_id(0)).id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->id);
  ASSERT_EQ(0, fmt::underlying(ue_cfg.search_space(to_search_space_id(0)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(0)).cfg ==
              cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0]);
  ASSERT_EQ(1, fmt::underlying(ue_cfg.search_space(to_search_space_id(1)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(1)).cfg ==
              cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1]);

  // Test Dedicated Config.
  ASSERT_TRUE(ue_cfg.find_coreset(to_coreset_id(2)) == nullptr);
  ASSERT_EQ(2, fmt::underlying(ue_cfg.search_space(to_search_space_id(2)).cfg->get_id()));
  ASSERT_TRUE(*ue_cfg.search_space(to_search_space_id(2)).cfg ==
              (*ue_create_msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]);
  ASSERT_TRUE(ue_cfg.find_search_space(to_search_space_id(3)) == nullptr);
}

TEST_F(ue_configuration_test, configuration_valid_on_reconfiguration)
{
  cell_configuration cell_cfg{sched_cfg, msg};
  cfg_pool.add_cell(msg);
  ue_cell_configuration ue_cfg{to_rnti(0x4601), cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[to_du_cell_index(0)]};

  sched_ue_reconfiguration_message recfg_req;
  recfg_req.ue_index = ue_create_msg.ue_index;
  recfg_req.crnti    = ue_create_msg.crnti;
  recfg_req.cfg.cells.emplace();
  recfg_req.cfg.cells.value().resize(1);
  cell_config_dedicated& ue_cell_reconf = recfg_req.cfg.cells.value()[0];
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg.emplace();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.emplace_back();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back() = config_helpers::make_default_coreset_config();
  ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back().id = to_coreset_id(2);
  ue_cfg.reconfigure(cfg_pool.reconf_ue(recfg_req).cells[to_du_cell_index(0)]);

  ASSERT_TRUE(ue_cfg.find_coreset(to_coreset_id(2)) != nullptr);
  ASSERT_EQ(2, fmt::underlying(ue_cfg.coreset(to_coreset_id(2)).id));
  ASSERT_TRUE(ue_cfg.coreset(to_coreset_id(2)) == ue_cell_reconf.serv_cell_cfg.init_dl_bwp.pdcch_cfg->coresets.back());
}

TEST_F(ue_configuration_test, when_reconfiguration_is_received_then_ue_updates_logical_channel_states)
{
  // Test Preamble.
  cell_cfg_db.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, msg));
  cfg_pool.add_cell(msg);
  ue_configuration ue_ded_cfg{ue_create_msg.ue_index, ue_create_msg.crnti, cell_cfg_db, cfg_pool.add_ue(ue_create_msg)};
  ue               u{ue_creation_command{ue_ded_cfg, ue_create_msg.starts_in_fallback, cell_harqs}};

  // Pass Reconfiguration to UE with an new Logical Channel.
  sched_ue_reconfiguration_message recfg{};
  recfg.ue_index = ue_create_msg.ue_index;
  recfg.crnti    = ue_create_msg.crnti;
  recfg.cfg      = ue_create_msg.cfg;
  recfg.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(uint_to_lcid(4)));
  ue_configuration ue_ded_cfg2{ue_ded_cfg};
  ue_ded_cfg2.update(cell_cfg_db, cfg_pool.reconf_ue(recfg));
  u.handle_reconfiguration_request(ue_reconf_command{ue_ded_cfg2}, false);

  // Confirm that the UE is in fallback.
  ASSERT_TRUE(u.get_pcell().is_in_fallback_mode());
  ASSERT_TRUE(u.get_pcell().get_pcell_state().reconf_ongoing);
  ASSERT_FALSE(u.get_pcell().get_pcell_state().reestablished);

  // While in fallback, DL buffer status that are not for SRB0/SRB1, do not get represented in pending bytes.
  ASSERT_FALSE(u.dl_logical_channels().has_pending_bytes());
  for (const auto& lc : *recfg.cfg.lc_config_list) {
    u.handle_dl_buffer_state_indication(lc.lcid, 10);
    if (lc.lcid <= LCID_SRB1) {
      ASSERT_TRUE(u.dl_logical_channels().has_pending_bytes());
    } else {
      ASSERT_FALSE(u.dl_logical_channels().has_pending_bytes());
    }
    u.handle_dl_buffer_state_indication(lc.lcid, 0);
  }

  // Confirm that UE config applied config.
  u.handle_config_applied();

  // Verify that DL buffer state indications affect newly active logical channels.
  for (const auto& lc : *recfg.cfg.lc_config_list) {
    if (lc.lcid == uint_to_lcid(0)) {
      // LCID0 is a special case.
      continue;
    }
    u.handle_dl_buffer_state_indication(lc.lcid, 10);
    ASSERT_TRUE(u.dl_logical_channels().has_pending_bytes());
    u.handle_dl_buffer_state_indication(lc.lcid, 0);
    ASSERT_FALSE(u.dl_logical_channels().has_pending_bytes());
  }

  // Verify that inactive logical channels do not affect pending bytes.
  u.handle_dl_buffer_state_indication(uint_to_lcid(6), 10);
  ASSERT_FALSE(u.dl_logical_channels().has_pending_bytes());
}

TEST_F(ue_configuration_test, search_spaces_pdcch_candidate_lists_does_not_surpass_limit)
{
  cell_config_builder_params params{};
  params.scs_common     = subcarrier_spacing::kHz30;
  params.dl_f_ref_arfcn = 520002;
  params.band           = nr_band::n41;
  params.channel_bw_mhz = bs_channel_bandwidth::MHz50;
  msg                   = sched_config_helper::make_default_sched_cell_configuration_request(params);
  ue_create_msg         = sched_config_helper::create_default_sched_ue_creation_request(params);

  auto&                        pdcch_cfg = *(*ue_create_msg.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg;
  const coreset_configuration& cset_cfg  = pdcch_cfg.coresets[0];
  search_space_configuration&  ss_cfg    = pdcch_cfg.search_spaces[0];
  ss_cfg.set_non_ss0_nof_candidates({config_helpers::compute_max_nof_candidates(aggregation_level::n1, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n2, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n4, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n8, cset_cfg),
                                     config_helpers::compute_max_nof_candidates(aggregation_level::n16, cset_cfg)});

  cell_configuration cell_cfg{sched_cfg, msg};
  cfg_pool.add_cell(msg);
  rnti_t crnti = to_rnti(test_rgen::uniform_int<uint16_t>(to_value(rnti_t::MIN_CRNTI), to_value(rnti_t::MAX_CRNTI)));
  ue_cell_configuration ue_cfg{crnti, cell_cfg, cfg_pool.add_ue(ue_create_msg).cells[to_du_cell_index(0)]};

  const bwp_config& bwp            = ue_cfg.bwp(to_bwp_id(0));
  const unsigned    max_candidates = max_nof_monitored_pdcch_candidates(bwp.dl_common->value().generic_params.scs);

  unsigned       sfn = test_rgen::uniform_int<unsigned>(0, 1023);
  const unsigned slots_to_test =
      msg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].get_monitoring_slot_periodicity();
  slot_point start_slot{params.scs_common, sfn, 0}, end_slot = start_slot + slots_to_test;
  for (slot_point pdcch_slot = start_slot; pdcch_slot != end_slot; ++pdcch_slot) {
    unsigned pdcch_candidates_count = 0;
    for (unsigned l = 0; l != NOF_AGGREGATION_LEVELS; ++l) {
      const aggregation_level aggr_lvl = aggregation_index_to_level(l);

      for (const search_space_configuration& ss : bwp.search_spaces) {
        ASSERT_GE(ss.get_nof_candidates()[l],
                  ue_cfg.search_space(ss.get_id()).get_pdcch_candidates(aggr_lvl, pdcch_slot).size())
            << "The generated candidates cannot exceed the number of candidates passed in the SearchSpace config";

        pdcch_candidates_count += ue_cfg.search_space(ss.get_id()).get_pdcch_candidates(aggr_lvl, pdcch_slot).size();
      }
    }

    // The number of PDCCH candidates in each SearchSpace must not exceed the max number of PDCCH candidates for the
    // given numerology as per TS 38.213 Table 10.1-2.
    ASSERT_LE(pdcch_candidates_count, max_candidates);
  }
}
