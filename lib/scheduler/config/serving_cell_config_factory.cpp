/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/config/serving_cell_config_factory.h"
#include "srsgnb/du/du_cell_config_helpers.h"

using namespace srsgnb;

serving_cell_config srsgnb::make_default_initial_ue_serving_cell_config()
{
  serving_cell_config serv_cell;

  // > PDCCH-Config.
  serv_cell.init_dl_bwp.pdcch_cfg.emplace();
  pdcch_config& pdcch_cfg = serv_cell.init_dl_bwp.pdcch_cfg.value();
  // >> Add CORESET#2.
  pdcch_cfg.coreset_to_addmod_list.push_back(du_config_helpers::make_default_coreset_config());
  pdcch_cfg.coreset_to_addmod_list[0].id = to_coreset_id(1);
  // >> Add SearchSpace#2.
  pdcch_cfg.ss_to_addmod_list.push_back(du_config_helpers::make_default_ue_search_space_config());

  // > PDSCH-Config.
  serv_cell.init_dl_bwp.pdsch_cfg.emplace();
  pdsch_config& pdsch_cfg            = serv_cell.init_dl_bwp.pdsch_cfg.value();
  pdsch_cfg.data_scrambling_id_pdsch = 0;

  return serv_cell;
}
