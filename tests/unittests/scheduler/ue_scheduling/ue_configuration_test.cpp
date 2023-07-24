/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

TEST(ue_configuration, configuration_valid_on_creation)
{
  sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};

  auto ue_create_msg = test_helpers::create_default_sched_ue_creation_request();

  ue_cell_configuration ue_cfg{cell_cfg, ue_create_msg.cfg.cells[0].serv_cell_cfg};

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
             ue_create_msg.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]);
  TESTASSERT(ue_cfg.find_search_space(to_search_space_id(3)) == nullptr);
}

TEST(ue_configuration, configuration_valid_on_reconfiguration)
{
  sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};
  auto                                     ue_create_msg = test_helpers::create_default_sched_ue_creation_request();
  ue_cell_configuration                    ue_cfg{cell_cfg, ue_create_msg.cfg.cells[0].serv_cell_cfg};

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

TEST(ue_configuration, when_reconfiguration_is_received_then_ue_updates_logical_channel_states)
{
  // Test Preamble.
  scheduler_expert_config                  expert_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message msg        = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};
  sched_ue_creation_request_message        ue_create_msg = test_helpers::create_default_sched_ue_creation_request();
  scheduler_harq_timeout_dummy_handler     harq_timeout_handler;
  ue                                       u{expert_cfg.ue, cell_cfg, ue_create_msg, harq_timeout_handler};

  // Pass Reconfiguration to UE with an new Logical Channel.
  sched_ue_reconfiguration_message recfg{};
  recfg.ue_index = ue_create_msg.ue_index;
  recfg.crnti    = ue_create_msg.crnti;
  recfg.cfg      = ue_create_msg.cfg;
  recfg.cfg.lc_config_list.push_back(config_helpers::create_default_logical_channel_config(uint_to_lcid(4)));
  u.handle_reconfiguration_request(recfg);
  dl_buffer_state_indication_message ind{recfg.ue_index, uint_to_lcid(4), 0};

  // Verify that DL buffer state indications affect newly active logical channels.
  for (const auto& lc : recfg.cfg.lc_config_list) {
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
