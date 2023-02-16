
#include "lib/scheduler/ue_scheduling/ue.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsgnb/support/srsgnb_test.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(ue_configuration, configuration_valid_on_creation)
{
  sched_cell_configuration_request_message msg = test_helpers::make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};

  auto ue_create_msg = test_helpers::create_default_sched_ue_creation_request();

  ue_cell_configuration ue_cfg{cell_cfg, ue_create_msg.cfg.cells[0].serv_cell_cfg};

  // Test Common Config.
  TESTASSERT(ue_cfg.find_dl_bwp_common(to_bwp_id(0)) != nullptr);
  TESTASSERT(ue_cfg.dl_bwp_common(to_bwp_id(0)).generic_params == cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  TESTASSERT(ue_cfg.coreset(to_coreset_id(0)).id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->id);
  TESTASSERT_EQ(0, ue_cfg.search_space(to_search_space_id(0)).id);
  TESTASSERT(ue_cfg.search_space(to_search_space_id(0)) ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0]);
  TESTASSERT_EQ(1, ue_cfg.search_space(to_search_space_id(1)).id);
  TESTASSERT(ue_cfg.search_space(to_search_space_id(1)) ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1]);

  // Test Dedicated Config.
  TESTASSERT(ue_cfg.find_coreset(to_coreset_id(2)) == nullptr);
  TESTASSERT_EQ(2, ue_cfg.search_space(to_search_space_id(2)).id);
  TESTASSERT(ue_cfg.search_space(to_search_space_id(2)) ==
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
  ue                                       u{expert_cfg.ue, cell_cfg, ue_create_msg};

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
