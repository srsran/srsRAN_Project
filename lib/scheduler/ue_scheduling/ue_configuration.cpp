
#include "ue_configuration.h"
#include "../../asn1/asn1_diff_utils.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"

using namespace srsgnb;

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
  bwp_table        = {};
  dl_coresets      = {};
  dl_search_spaces = {};

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
  bwp_table[bwpid].dl_bwp_common = &bwp_dl_common;
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    bwp_table[bwpid].dl_search_spaces.push_back(&ss);
  }

  // Compute CORESET-Id lookup table.
  if (bwp_dl_common.pdcch_common.coreset0.has_value()) {
    dl_coresets[0]          = &*bwp_dl_common.pdcch_common.coreset0;
    coreset_id_to_bwp_id[0] = bwpid;
  }
  if (bwp_dl_common.pdcch_common.common_coreset.has_value()) {
    dl_coresets[bwp_dl_common.pdcch_common.common_coreset->id] = &*bwp_dl_common.pdcch_common.common_coreset;

    coreset_id_to_bwp_id[bwp_dl_common.pdcch_common.common_coreset->id] = bwpid;
  }

  // Compute SearchSpace-Id lookup table.
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    dl_search_spaces[ss.id] = &ss;
  }
}

void ue_cell_configuration::configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_bwp_common = &bwp_ul_common;
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded)
{
  // Compute DL BWP-Id lookup table.
  bwp_table[bwpid].dl_bwp_ded = &bwp_dl_ded;
  if (not bwp_dl_ded.pdcch_cfg.has_value()) {
    return;
  }

  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    bwp_table[bwpid].dl_search_spaces.push_back(&ss);
  }

  // Compute CORESET-Id lookup table.
  for (const coreset_configuration& cs : bwp_dl_ded.pdcch_cfg->coresets) {
    // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
    // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
    // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
    // servingCellConfigCommon.
    dl_coresets[cs.id] = &cs;

    coreset_id_to_bwp_id[cs.id] = bwpid;
  }

  // Compute SearchSpace-Id lookup table.
  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    dl_search_spaces[ss.id] = &ss;
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_bwp_ded = &bwp_ul_ded;
}

span<const pdsch_time_domain_resource_allocation>
ue_cell_configuration::get_pdsch_time_domain_list(search_space_id ss_id) const
{
  srsgnb_assert(dl_search_spaces[ss_id] != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
  const search_space_configuration& ss_cfg  = *dl_search_spaces[ss_id];
  const bwp_params&                 bwp_row = bwp_table[coreset_id_to_bwp_id[ss_cfg.cs_id]];

  if (ss_cfg.type != search_space_configuration::type::common or ss_cfg.cs_id != to_coreset_id(0)) {
    if (not bwp_row.dl_bwp_ded->pdsch_cfg->pdsch_td_alloc_list.empty()) {
      // UE dedicated pdsch-TimeDomain list.
      return bwp_row.dl_bwp_ded->pdsch_cfg->pdsch_td_alloc_list;
    }
  }

  if (not bwp_row.dl_bwp_common->pdsch_common.pdsch_td_alloc_list.empty()) {
    // common pdsch-TimeDomain list.
    return bwp_row.dl_bwp_common->pdsch_common.pdsch_td_alloc_list;
  }

  // default A table case.
  return pdsch_default_time_allocations_default_A_table(
      bwp_row.dl_bwp_common->generic_params.cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL,
      cell_cfg_common.dmrs_typeA_pos);
}
