
#include "ue_configuration.h"

using namespace srsgnb;

ue_cell_configuration::ue_cell_configuration(const cell_configuration&                    cell_cfg_common_,
                                             const serving_cell_ue_configuration_request& cell_cfg_ded_req) :
  cell_cfg_common(cell_cfg_common_)
{
  // Apply Common Config.
  dl_bwps.emplace(0, cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params);
  for (const coreset_configuration& cs_cfg : cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.coresets) {
    dl_coresets.emplace(cs_cfg.id, cs_cfg);
  }
  for (const search_space_configuration& ss_cfg :
       cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    dl_search_spaces.emplace(ss_cfg.id, ss_cfg);
  }

  // Apply UE-dedicated Config.
  reconfigure(cell_cfg_ded_req);
}

void ue_cell_configuration::reconfigure(const serving_cell_ue_configuration_request& cell_cfg_ded_req)
{
  const bwp_downlink_dedicated& dl_bwp_cfg = cell_cfg_ded_req.dl_bwps[0];
  for (coreset_id cs_id : dl_bwp_cfg.pdcch_cfg->coreset_to_rel_list) {
    dl_coresets.erase(cs_id);
  }
  for (const coreset_configuration& cs_cfg : dl_bwp_cfg.pdcch_cfg->coreset_to_addmod_list) {
    dl_coresets.insert(cs_cfg.id, cs_cfg);
  }
  for (search_space_id ss_id : dl_bwp_cfg.pdcch_cfg->ss_to_rel_list) {
    dl_search_spaces.erase(ss_id);
  }
  for (const search_space_configuration& ss_cfg : dl_bwp_cfg.pdcch_cfg->ss_to_addmod_list) {
    dl_search_spaces.insert(ss_cfg.id, ss_cfg);
  }
}
