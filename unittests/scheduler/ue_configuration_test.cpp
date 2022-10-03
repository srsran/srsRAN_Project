
#include "../../lib/scheduler/ue_scheduling/ue_configuration.h"
#include "config_generators.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

void test_ue_cfg_creation()
{
  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};

  auto ue_create_msg = make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());

  ue_cell_configuration ue_cfg{cell_cfg, ue_create_msg.serv_cell_cfg};

  // Test Common Config.
  TESTASSERT(ue_cfg.dl_bwps[0] != nullptr);
  TESTASSERT(ue_cfg.dl_bwps[0]->bwp_dl_common->generic_params == cell_cfg.dl_cfg_common.init_dl_bwp.generic_params);
  TESTASSERT(ue_cfg.dl_coresets[0]->id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->id);
  TESTASSERT_EQ(0, ue_cfg.dl_search_spaces[0]->id);
  TESTASSERT(ue_cfg.dl_search_spaces[0]->id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].id);
  TESTASSERT(ue_cfg.dl_search_spaces[0]->cs_id ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[0].cs_id);
  TESTASSERT_EQ(1, ue_cfg.dl_search_spaces[1]->id);
  TESTASSERT(ue_cfg.dl_search_spaces[1]->id == cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].id);
  TESTASSERT(ue_cfg.dl_search_spaces[1]->cs_id ==
             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[1].cs_id);

  // Test Dedicated Config.
  TESTASSERT(ue_cfg.dl_coresets[1]->id ==
             ue_create_msg.serv_cell_cfg.init_dl_bwp->pdcch_cfg->coreset_to_addmod_list[0].id);
  TESTASSERT(ue_cfg.dl_coresets[2] == nullptr);
  TESTASSERT_EQ(2, ue_cfg.dl_search_spaces[2]->id);
  TESTASSERT_EQ(ue_cfg.dl_search_spaces[2]->cs_id,
                ue_create_msg.serv_cell_cfg.init_dl_bwp->pdcch_cfg->ss_to_addmod_list[0].cs_id);
  TESTASSERT(ue_cfg.dl_search_spaces[3] == nullptr);
}

void test_ue_cfg_reconfig()
{
  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};
  auto ue_create_msg = make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());
  ue_cell_configuration ue_cfg{cell_cfg, ue_create_msg.serv_cell_cfg};

  serving_cell_ue_configuration_request ue_cell_reconf{};
  ue_cell_reconf.init_dl_bwp.emplace();
  ue_cell_reconf.init_dl_bwp->pdcch_cfg.emplace();
  ue_cell_reconf.init_dl_bwp->pdcch_cfg->coreset_to_addmod_list.emplace_back();
  ue_cell_reconf.init_dl_bwp->pdcch_cfg->coreset_to_addmod_list.back() = config_helpers::make_default_coreset_config();
  ue_cell_reconf.init_dl_bwp->pdcch_cfg->coreset_to_addmod_list.back().id = to_coreset_id(2);
  ue_cell_reconf.init_dl_bwp->pdcch_cfg->ss_to_rel_list.emplace_back();
  ue_cell_reconf.init_dl_bwp->pdcch_cfg->ss_to_rel_list[0] = to_search_space_id(2);

  ue_cfg.reconfigure(ue_cell_reconf);

  TESTASSERT(ue_cfg.dl_coresets[2] != nullptr);
  TESTASSERT_EQ(2, ue_cfg.dl_coresets[2]->id);
  TESTASSERT(ue_cfg.dl_search_spaces[2] == nullptr);
}

int main()
{
  test_ue_cfg_creation();
  test_ue_cfg_reconfig();
}
