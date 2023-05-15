
#include "ue_configuration.h"
#include "../../asn1/asn1_diff_utils.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pusch/pusch_default_time_allocation.h"
#include "srsran/support/error_handling.h"

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
  ss_list       = {};

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
  bwp_table[bwpid].dl_common = &bwp_dl_common;
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    bwp_table[bwpid].search_spaces.push_back(&ss);
  }

  // Compute CORESET-Id lookup table.
  if (bwp_dl_common.pdcch_common.coreset0.has_value()) {
    coresets[0]             = &*bwp_dl_common.pdcch_common.coreset0;
    coreset_id_to_bwp_id[0] = bwpid;
  }
  if (bwp_dl_common.pdcch_common.common_coreset.has_value()) {
    coresets[bwp_dl_common.pdcch_common.common_coreset->id] = &*bwp_dl_common.pdcch_common.common_coreset;

    coreset_id_to_bwp_id[bwp_dl_common.pdcch_common.common_coreset->id] = bwpid;
  }

  // Compute SearchSpace-Id lookup table.
  for (const search_space_configuration& ss : bwp_dl_common.pdcch_common.search_spaces) {
    search_spaces[ss.id] = &ss;
    ss_list.emplace(ss.id);
    ss_list[ss.id].cfg     = &ss;
    ss_list[ss.id].coreset = &*coresets[ss.cs_id];
    ss_list[ss.id].bwp     = &bwp_table[bwpid];
    ss_list[ss.id].pdsch_time_domain_list =
        get_c_rnti_pdsch_time_domain_list(ss, *ss_list[ss.id].bwp->dl_common, nullptr, cell_cfg_common.dmrs_typeA_pos);
  }
}

void ue_cell_configuration::configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_common = &bwp_ul_common;

  for (const search_space_configuration& ss : bwp_table[bwpid].dl_common->pdcch_common.search_spaces) {
    ss_list[ss.id].pusch_time_domain_list = get_c_rnti_pusch_time_domain_list(ss, *bwp_table[bwpid].ul_common, nullptr);
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded)
{
  // Compute DL BWP-Id lookup table.
  bwp_table[bwpid].dl_ded = &bwp_dl_ded;
  if (not bwp_dl_ded.pdcch_cfg.has_value()) {
    return;
  }

  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    bwp_table[bwpid].search_spaces.push_back(&ss);
  }

  // Compute CORESET-Id lookup table.
  for (const coreset_configuration& cs : bwp_dl_ded.pdcch_cfg->coresets) {
    // TS 38.331, "PDCCH-Config" - In case network reconfigures control resource set with the same
    // ControlResourceSetId as used for commonControlResourceSet configured via PDCCH-ConfigCommon,
    // the configuration from PDCCH-Config always takes precedence and should not be updated by the UE based on
    // servingCellConfigCommon.
    coresets[cs.id] = &cs;

    coreset_id_to_bwp_id[cs.id] = bwpid;
  }

  // Compute SearchSpace-Id lookup table.
  for (const search_space_configuration& ss : bwp_dl_ded.pdcch_cfg->search_spaces) {
    search_spaces[ss.id] = &ss;
    ss_list.emplace(ss.id);
    ss_list[ss.id].cfg                    = &ss;
    ss_list[ss.id].coreset                = coresets[ss.cs_id];
    ss_list[ss.id].bwp                    = &bwp_table[bwpid];
    ss_list[ss.id].pdsch_time_domain_list = get_c_rnti_pdsch_time_domain_list(
        ss, *bwp_table[bwpid].dl_common, bwp_table[bwpid].dl_ded, cell_cfg_common.dmrs_typeA_pos);
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_ded = &bwp_ul_ded;

  for (const search_space_configuration& ss : bwp_table[bwpid].dl_ded->pdcch_cfg->search_spaces) {
    ss_list[ss.id].pusch_time_domain_list =
        get_c_rnti_pusch_time_domain_list(ss, *bwp_table[bwpid].ul_common, bwp_table[bwpid].ul_ded);
  }
}

dci_ul_rnti_config_type ue_cell_configuration::get_ul_rnti_config_type(search_space_id ss_id) const
{
  srsran_assert(search_spaces[ss_id] != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
  const search_space_configuration& ss_cfg = *search_spaces[ss_id];

  if (ss_cfg.type == search_space_configuration::type_t::common) {
    if (ss_cfg.common.f0_0_and_f1_0) {
      return dci_ul_rnti_config_type::c_rnti_f0_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported UL DCI format");
  }
  switch (ss_cfg.ue_specific) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_ul_rnti_config_type::c_rnti_f0_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_ul_rnti_config_type::c_rnti_f0_1;
  }
  report_fatal_error("Unsupported UL DCI RNTI type");
}

dci_dl_rnti_config_type ue_cell_configuration::get_dl_rnti_config_type(search_space_id ss_id) const
{
  srsran_assert(search_spaces[ss_id] != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
  const search_space_configuration& ss_cfg = *search_spaces[ss_id];

  if (ss_cfg.type == search_space_configuration::type_t::common) {
    if (ss_cfg.common.f0_0_and_f1_0) {
      return dci_dl_rnti_config_type::c_rnti_f1_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported UL DCI format");
  }
  switch (ss_cfg.ue_specific) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_dl_rnti_config_type::c_rnti_f1_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_dl_rnti_config_type::c_rnti_f1_1;
  }
  report_fatal_error("Unsupported DL DCI RNTI type");
}

dci_ul_format ue_cell_configuration::get_ul_dci_format(search_space_id ss_id) const
{
  srsran_assert(search_spaces[ss_id] != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
  const search_space_configuration& ss_cfg = *search_spaces[ss_id];

  if (ss_cfg.type == search_space_configuration::type_t::common) {
    if (ss_cfg.common.f0_0_and_f1_0) {
      return dci_ul_format::f0_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported UL DCI format");
  }
  switch (ss_cfg.ue_specific) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_ul_format::f0_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_ul_format::f0_1;
  }
  report_fatal_error("Unsupported UL DCI format");
}

dci_dl_format ue_cell_configuration::get_dl_dci_format(search_space_id ss_id) const
{
  srsran_assert(search_spaces[ss_id] != nullptr, "Inexistent SearchSpace-Id={}", ss_id);
  const search_space_configuration& ss_cfg = *search_spaces[ss_id];

  if (ss_cfg.type == search_space_configuration::type_t::common) {
    if (ss_cfg.common.f0_0_and_f1_0) {
      return dci_dl_format::f1_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported DL DCI format");
  }
  switch (ss_cfg.ue_specific) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_dl_format::f1_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_dl_format::f1_1;
  }
  report_fatal_error("Unsupported DL DCI format");
}
