
#include "../../lib/scheduler/ue_scheduling/ue_configuration.h"
#include "srsgnb/mac/mac_configuration_helpers.h"
#include "srsgnb/support/srsgnb_test.h"
#include "unittests/scheduler/utils/config_generators.h"

using namespace srsgnb;

void test_ue_cfg_creation()
{
  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};

  auto ue_create_msg = make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());

  ue_cell_configuration ue_cfg{cell_cfg, *ue_create_msg.cells[0].serv_cell_cfg};

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
             ue_create_msg.cells[0].serv_cell_cfg->init_dl_bwp.pdcch_cfg->search_spaces[0]);
  TESTASSERT(ue_cfg.find_search_space(to_search_space_id(3)) == nullptr);
}

void test_ue_cfg_reconfig()
{
  sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
  cell_configuration                       cell_cfg{msg};
  auto ue_create_msg = make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());
  ue_cell_configuration ue_cfg{cell_cfg, *ue_create_msg.cells[0].serv_cell_cfg};

  serving_cell_ue_configuration_request ue_cell_reconf{};
  ue_cell_reconf.serv_cell_cfg.emplace();
  ue_cell_reconf.serv_cell_cfg->init_dl_bwp.pdcch_cfg.emplace();
  ue_cell_reconf.serv_cell_cfg->init_dl_bwp.pdcch_cfg->coresets.emplace_back();
  ue_cell_reconf.serv_cell_cfg->init_dl_bwp.pdcch_cfg->coresets.back() = config_helpers::make_default_coreset_config();
  ue_cell_reconf.serv_cell_cfg->init_dl_bwp.pdcch_cfg->coresets.back().id = to_coreset_id(2);

  ue_cfg.reconfigure(*ue_cell_reconf.serv_cell_cfg);

  TESTASSERT(ue_cfg.find_coreset(to_coreset_id(2)) != nullptr);
  TESTASSERT_EQ(2, ue_cfg.coreset(to_coreset_id(2)).id);
  TESTASSERT(ue_cfg.coreset(to_coreset_id(2)) == ue_cell_reconf.serv_cell_cfg->init_dl_bwp.pdcch_cfg->coresets.back());
}

int main()
{
  test_ue_cfg_creation();
  test_ue_cfg_reconfig();
}
