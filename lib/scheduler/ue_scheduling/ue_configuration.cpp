
#include "ue_configuration.h"
#include "../../asn1/asn1_diff_utils.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_cell_configuration::ue_cell_configuration(const cell_configuration&                    cell_cfg_common_,
                                             const serving_cell_ue_configuration_request& cell_cfg_ded_req) :
  cell_cfg_common(cell_cfg_common_)
{
  // Apply Common Config.
  addmod_bwp_common_cfg(to_bwp_id(0), cell_cfg_common.dl_cfg_common.init_dl_bwp);

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
    addmod_bwp_cfg(dl_bwp.bwp_id, dl_bwp);
  }

  update_config_maps();
}

void ue_cell_configuration::addmod_bwp_cfg(bwp_id_t bwpid, const bwp_downlink& bwp_dl)
{
  if (not dl_bwps_cfg.contains(bwpid)) {
    dl_bwps_cfg.emplace(bwpid);
  }
  if (bwp_dl.bwp_dl_ded.has_value()) {
    addmod_bwp_ded_cfg(bwpid, *bwp_dl.bwp_dl_ded);
  }
}

void ue_cell_configuration::addmod_bwp_common_cfg(srsgnb::bwp_id_t bwpid, const srsgnb::bwp_downlink_common& bwp_dl)
{
  if (not dl_bwps_cfg.contains(bwpid)) {
    dl_bwps_cfg.emplace(bwpid);
  }

  // Note: Common config does not apply diffs.
  dl_bwps_cfg[bwpid].bwp_dl_common = bwp_dl;
}

void ue_cell_configuration::update_config_maps()
{
  // Update DL BWPs.
  dl_bwps = {};
  for (const bwp_downlink& bwp_dl : dl_bwps_cfg) {
    dl_bwps[bwp_dl.bwp_id] = &bwp_dl;
  }

  // Update DL CORESETs.
  dl_coresets = {};
  for (const bwp_downlink& bwp_dl : dl_bwps_cfg) {
    if (bwp_dl.bwp_dl_common.has_value()) {
      if (bwp_dl.bwp_dl_common->pdcch_common.coreset0.has_value()) {
        dl_coresets[0] = &*bwp_dl.bwp_dl_common->pdcch_common.coreset0;
      }
      if (bwp_dl.bwp_dl_common->pdcch_common.common_coreset.has_value()) {
        dl_coresets[bwp_dl.bwp_dl_common->pdcch_common.common_coreset->id] =
            &*bwp_dl.bwp_dl_common->pdcch_common.common_coreset;
      }
    }
    if (bwp_dl.bwp_dl_ded.has_value()) {
      // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
      // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
      // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
      // servingCellConfigCommon.
      for (const coreset_configuration& cs_cfg : bwp_dl.bwp_dl_ded->pdcch_cfg->coreset_to_addmod_list) {
        dl_coresets[cs_cfg.id] = &cs_cfg;
      }
    }
  }

  // Update DL SearchSpaces.
  dl_search_spaces = {};
  for (const bwp_downlink& bwp_dl : dl_bwps_cfg) {
    if (bwp_dl.bwp_dl_common.has_value()) {
      for (const search_space_configuration& ss_cfg : bwp_dl.bwp_dl_common->pdcch_common.search_spaces) {
        dl_search_spaces[ss_cfg.id] = &ss_cfg;
      }
    }
    if (bwp_dl.bwp_dl_ded.has_value()) {
      for (const search_space_configuration& ss_cfg : bwp_dl.bwp_dl_ded->pdcch_cfg->ss_to_addmod_list) {
        dl_search_spaces[ss_cfg.id] = &ss_cfg;
      }
    }
  }
}

void ue_cell_configuration::addmod_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded)
{
  if (not dl_bwps_cfg.contains(bwpid)) {
    dl_bwps_cfg.emplace(bwpid);
  }
  if (not dl_bwps_cfg[bwpid].bwp_dl_ded.has_value()) {
    dl_bwps_cfg[bwpid].bwp_dl_ded.emplace();
  }
  bwp_downlink_dedicated& self_bwp_ded = *dl_bwps_cfg[bwpid].bwp_dl_ded;
  if (not self_bwp_ded.pdcch_cfg.has_value()) {
    self_bwp_ded.pdcch_cfg.emplace();
  }
  pdcch_config& self_pdcch = *self_bwp_ded.pdcch_cfg;

  // Apply CORESET diff.
  apply_addmodremlist_diff(self_pdcch.coreset_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->coreset_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->coreset_to_rel_list,
                           [](const coreset_configuration& cs) { return cs.id; });

  // Apply SearchSpace diff.
  apply_addmodremlist_diff(self_pdcch.ss_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->ss_to_addmod_list,
                           bwp_dl_ded.pdcch_cfg->ss_to_rel_list,
                           [](const search_space_configuration& ss) { return ss.id; });
}

void ue_cell_configuration::rel_bwp_ded_cfg(bwp_id_t bwpid)
{
  srsgnb_assert(dl_bwps_cfg.contains(bwpid), "BWP-id={} does not exist", bwpid);
  dl_bwps_cfg.erase(bwpid);
}
