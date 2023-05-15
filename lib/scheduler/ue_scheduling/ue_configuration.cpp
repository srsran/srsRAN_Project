
#include "ue_configuration.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pusch/pusch_default_time_allocation.h"

using namespace srsran;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_cell_configuration::ue_cell_configuration(const cell_configuration&  cell_cfg_common_,
                                             const serving_cell_config& serv_cell_cfg_) :
  cell_cfg_common(cell_cfg_common_)
{
  // Apply UE-dedicated Config.
  reconfigure(serv_cell_cfg_);
}

void ue_cell_configuration::reconfigure(const serving_cell_config& cell_cfg_ded_req)
{
  cell_cfg_ded = cell_cfg_ded_req;

  // Clear previous lookup tables.
  bwp_table     = {};
  coresets      = {};
  search_spaces = {};

  // Recompute DL param lookup tables.
  configure_bwp_common_cfg(to_bwp_id(0), cell_cfg_common.dl_cfg_common.init_dl_bwp);
  configure_bwp_ded_cfg(to_bwp_id(0), cell_cfg_ded.init_dl_bwp);
  for (const bwp_downlink& bwp : cell_cfg_ded.dl_bwps) {
    if (bwp.bwp_dl_common.has_value()) {
      configure_bwp_common_cfg(bwp.bwp_id, *bwp.bwp_dl_common);
    }
    if (bwp.bwp_dl_ded.has_value()) {
      configure_bwp_ded_cfg(bwp.bwp_id, *bwp.bwp_dl_ded);
    }
  }

  // Recompute UL param lookup tables.
  configure_bwp_common_cfg(to_bwp_id(0), cell_cfg_common.ul_cfg_common.init_ul_bwp);
  if (cell_cfg_ded.ul_config.has_value()) {
    configure_bwp_ded_cfg(to_bwp_id(0), cell_cfg_ded.ul_config->init_ul_bwp);
  }
}

void ue_cell_configuration::configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl_common)
{
  // Compute DL BWP-Id lookup table.
  bwp_table[bwpid].bwp_id    = bwpid;
  bwp_table[bwpid].dl_common = &bwp_dl_common;

  // Compute CORESET-Id lookup table.
  if (bwp_dl_common.pdcch_common.coreset0.has_value()) {
    coresets[0]             = &*bwp_dl_common.pdcch_common.coreset0;
    coreset_id_to_bwp_id[0] = bwpid;
  }
  if (bwp_dl_common.pdcch_common.common_coreset.has_value()) {
    coresets[bwp_dl_common.pdcch_common.common_coreset->id]             = &*bwp_dl_common.pdcch_common.common_coreset;
    coreset_id_to_bwp_id[bwp_dl_common.pdcch_common.common_coreset->id] = bwpid;
  }

  // Compute SearchSpace-Id lookup tables.
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    search_spaces.emplace(ss.id);
    search_spaces[ss.id].cfg                    = &ss;
    search_spaces[ss.id].coreset                = &*coresets[ss.cs_id];
    search_spaces[ss.id].bwp                    = &bwp_table[bwpid];
    search_spaces[ss.id].pdsch_time_domain_list = get_c_rnti_pdsch_time_domain_list(
        ss, *search_spaces[ss.id].bwp->dl_common, nullptr, cell_cfg_common.dmrs_typeA_pos);
  }
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    bwp_table[bwpid].search_spaces.emplace(ss.id, &search_spaces[ss.id]);
  }
}

void ue_cell_configuration::configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_common = &bwp_ul_common;

  for (const search_space_configuration& ss : bwp_table[bwpid].dl_common->pdcch_common.search_spaces) {
    search_spaces[ss.id].pusch_time_domain_list =
        get_c_rnti_pusch_time_domain_list(ss, *bwp_table[bwpid].ul_common, nullptr);
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded)
{
  // Compute DL BWP-Id lookup table.
  bwp_table[bwpid].dl_ded = &bwp_dl_ded;
  if (not bwp_dl_ded.pdcch_cfg.has_value()) {
    return;
  }

  // Compute CORESET-Id lookup table.
  for (const coreset_configuration& cs : bwp_dl_ded.pdcch_cfg->coresets) {
    // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
    // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
    // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
    // servingCellConfigCommon.
    coresets[cs.id]             = &cs;
    coreset_id_to_bwp_id[cs.id] = bwpid;
  }

  // Compute SearchSpace-Id lookup tables.
  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    search_spaces.emplace(ss.id);
    search_spaces[ss.id].cfg                    = &ss;
    search_spaces[ss.id].coreset                = coresets[ss.cs_id];
    search_spaces[ss.id].bwp                    = &bwp_table[bwpid];
    search_spaces[ss.id].pdsch_time_domain_list = get_c_rnti_pdsch_time_domain_list(
        ss, *bwp_table[bwpid].dl_common, bwp_table[bwpid].dl_ded, cell_cfg_common.dmrs_typeA_pos);
  }
  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    bwp_table[bwpid].search_spaces.emplace(ss.id, &search_spaces[ss.id]);
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_ded = &bwp_ul_ded;

  for (const search_space_configuration& ss : bwp_table[bwpid].dl_ded->pdcch_cfg->search_spaces) {
    search_spaces[ss.id].pusch_time_domain_list =
        get_c_rnti_pusch_time_domain_list(ss, *bwp_table[bwpid].ul_common, bwp_table[bwpid].ul_ded);
  }
}

dci_ul_rnti_config_type ue_cell_configuration::get_ul_rnti_config_type(search_space_id ss_id) const
{
  dci_ul_format dci_fmt = search_space(ss_id).get_ul_dci_format();
  return dci_fmt == dci_ul_format::f0_0 ? dci_ul_rnti_config_type::c_rnti_f0_0 : dci_ul_rnti_config_type::c_rnti_f0_1;
}

dci_dl_rnti_config_type ue_cell_configuration::get_dl_rnti_config_type(search_space_id ss_id) const
{
  dci_dl_format dci_fmt = search_space(ss_id).get_dl_dci_format();
  return dci_fmt == dci_dl_format::f1_0 ? dci_dl_rnti_config_type::c_rnti_f1_0 : dci_dl_rnti_config_type::c_rnti_f1_1;
}
