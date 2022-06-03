
#include "ue_configuration.h"
#include "../../asn1/asn1_diff_utils.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_cell_configuration::ue_cell_configuration(const cell_configuration&                    cell_cfg_common_,
                                             const serving_cell_ue_configuration_request& cell_cfg_ded_req) :
  cell_cfg_common(cell_cfg_common_)
{
  // Apply Common Config.
  dl_bwps_cfg.emplace(0);
  dl_bwps_cfg[0].bwp_id        = to_bwp_id(0);
  dl_bwps_cfg[0].bwp_dl_common = cell_cfg_common.dl_cfg_common.init_dl_bwp;
  dl_bwps_cfg[0].bwp_dl_ded    = *cell_cfg_ded_req.init_dl_bwp;
  dl_bwps[0]                   = &cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params;
  const auto& pdcch_cfg        = cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common;
  dl_coresets[0]               = &(*pdcch_cfg.coreset0);
  if (pdcch_cfg.common_coreset.has_value()) {
    dl_coresets[pdcch_cfg.common_coreset->id] = &pdcch_cfg.common_coreset.value();
  }
  for (const search_space_configuration& ss_cfg :
       cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    dl_search_spaces[ss_cfg.id] = &ss_cfg;
  }

  // Apply UE-dedicated Config.
  reconfigure(cell_cfg_ded_req);
}

void ue_cell_configuration::reconfigure(const serving_cell_ue_configuration_request& cell_cfg_ded_req)
{
  // Update DL BWPs.
  if (cell_cfg_ded_req.init_dl_bwp.has_value()) {
    addmod_bwp_ded_cfg(to_bwp_id(0), *cell_cfg_ded_req.init_dl_bwp);
  }
  for (bwp_id_t bwpid : cell_cfg_ded_req.dl_bwps_to_rel_list) {
    rel_bwp_ded_cfg(bwpid);
  }
  for (const bwp_downlink& dl_bwp : cell_cfg_ded_req.dl_bwps_to_addmod_list) {
    addmod_bwp_ded_cfg(dl_bwp.bwp_id, *cell_cfg_ded_req.init_dl_bwp);
  }
}

void ue_cell_configuration::addmod_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded)
{
  if (not dl_bwps_cfg.contains(bwpid)) {
    dl_bwps_cfg.emplace(bwpid);
  }
  bwp_downlink_dedicated& self_bwp_ded = dl_bwps_cfg[bwpid].bwp_dl_ded;
  if (not self_bwp_ded.pdcch_cfg.has_value()) {
    self_bwp_ded.pdcch_cfg.emplace();
  }
  pdcch_config& self_pdcch = *self_bwp_ded.pdcch_cfg;

  // Apply CORESET diff.
  apply_addmodremlist_diff(self_pdcch.coreset_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->coreset_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->coreset_to_rel_list,
                           [](const coreset_configuration& cs) { return cs.id; });
  for (coreset_id cs_id : bwp_dl_ded.pdcch_cfg->coreset_to_rel_list) {
    if (cs_id == cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value() and
        cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset->id) {
      // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
      // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
      // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
      // servingCellConfigCommon.
      dl_coresets[cs_id] = &cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();
    } else {
      dl_coresets[cs_id] = nullptr;
    }
  }
  for (const coreset_configuration& cs : bwp_dl_ded.pdcch_cfg->coreset_to_addmod_list) {
    dl_coresets[cs.id] = &cs;
  }

  // Apply SearchSpace diff.
  apply_addmodremlist_diff(self_pdcch.ss_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->ss_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->ss_to_rel_list,
                           [](const search_space_configuration& ss) { return ss.id; });
  for (search_space_id ss_id : bwp_dl_ded.pdcch_cfg->ss_to_rel_list) {
    dl_search_spaces[ss_id] = nullptr;
  }
  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->ss_to_addmod_list) {
    dl_search_spaces[ss.id] = &ss;
  }

  // Set BWP Config.
  dl_bwps[bwpid] = &dl_bwps_cfg[bwpid].bwp_dl_common.generic_params;
}

void ue_cell_configuration::rel_bwp_ded_cfg(bwp_id_t bwpid)
{
  srsran_sanity_check(dl_bwps_cfg.contains(bwpid), "BWP-id={} does not exist", bwpid);
  bwp_downlink_dedicated& self_bwp_ded = dl_bwps_cfg[bwpid].bwp_dl_ded;
  for (const coreset_configuration& cs_cfg : self_bwp_ded.pdcch_cfg->coreset_to_addmod_list) {
    dl_coresets[cs_cfg.id] = nullptr;
  }
  for (const search_space_configuration& ss_cfg : self_bwp_ded.pdcch_cfg->ss_to_addmod_list) {
    dl_search_spaces[ss_cfg.id] = &ss_cfg;
  }
  dl_bwps_cfg.erase(bwpid);
}
