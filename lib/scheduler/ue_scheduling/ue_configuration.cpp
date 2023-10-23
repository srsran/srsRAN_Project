/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ue_configuration.h"
#include "../support/pdcch/pdcch_mapping.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/pusch/pusch_default_time_allocation.h"
#include "../support/pusch/pusch_resource_allocation.h"
#include "srsran/support/math/gcd.h"

using namespace srsran;

span<const uint8_t> search_space_info::get_k1_candidates() const
{
  // TS38.213, clause 9.2.3 - For DCI format 1_0, the PDSCH-to-HARQ-timing-indicator field values map to
  // {1, 2, 3, 4, 5, 6, 7, 8}. For DCI format 1_1, if present, the PDSCH-to-HARQ-timing-indicator field values map to
  // values for a set of number of slots provided by dl-DataToUL-ACK as defined in Table 9.2.3-1.
  // Note: Tested UEs do not support k1 < 4.
  static const std::array<uint8_t, 5> f1_0_list = {4, 5, 6, 7, 8};
  if (get_dl_dci_format() == srsran::dci_dl_format::f1_0) {
    return f1_0_list;
  }
  return bwp->ul_ded->pucch_cfg->dl_data_to_ul_ack;
}

void search_space_info::update_pdcch_candidates(
    const std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>>& candidates,
    pci_t                                                                        pci)
{
  srsran_assert(candidates.size() > 0, "The SearchSpace doesn't have any candidates");
  ss_pdcch_candidates = candidates;

  crbs_of_candidates.resize(ss_pdcch_candidates.size());
  for (unsigned sl = 0; sl < ss_pdcch_candidates.size(); sl++) {
    for (unsigned lidx = 0; lidx < ss_pdcch_candidates[sl].size(); lidx++) {
      const aggregation_level aggr_lvl = aggregation_index_to_level(lidx);
      crbs_of_candidates[sl][lidx].resize(ss_pdcch_candidates[sl][lidx].size());
      for (unsigned candidate_idx = 0; candidate_idx != ss_pdcch_candidates[sl][lidx].size(); ++candidate_idx) {
        uint8_t ncce     = ss_pdcch_candidates[sl][lidx][candidate_idx];
        auto&   crb_list = crbs_of_candidates[sl][lidx][candidate_idx];

        // Get PRBs for each candidate.
        crb_list = pdcch_helper::cce_to_prb_mapping(bwp->dl_common->generic_params, *coreset, pci, aggr_lvl, ncce);

        // Convert PRBs to CRBs.
        for (uint16_t& prb_idx : crb_list) {
          prb_idx = prb_to_crb(bwp->dl_common->generic_params.crbs, prb_idx);
        }
      }
    }
  }
}

/// \brief Determines whether the given DCI format is monitored in UE specific SS or not.
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] active_bwp_id Active BWP Id.
/// \param[in] check_for_fallback_dci_formats Boolean denoting a fallback or non-fallback DCI format.
/// \return Returns whether the given DCI format are monitored in UE specific SS or not.
static bool is_dci_format_monitored_in_ue_ss(const ue_cell_configuration& ue_cell_cfg,
                                             bwp_id_t                     active_bwp_id,
                                             bool                         check_for_fallback_dci_formats)
{
  const auto& bwp_search_spaces = ue_cell_cfg.bwp(active_bwp_id).search_spaces;
  const auto  dci_format        = check_for_fallback_dci_formats
                                      ? search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0
                                      : search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;
  return std::any_of(bwp_search_spaces.begin(), bwp_search_spaces.end(), [dci_format](const search_space_info* ss) {
    return (not ss->cfg->is_common_search_space()) and (variant_get<search_space_configuration::ue_specific_dci_format>(
                                                            ss->cfg->get_monitored_dci_formats()) == dci_format);
  });
}

/// \brief Fetches DCI size configurations required to compute DCI size.
/// \param[in] ue_cell_cfg UE cell configuration.
/// \param[in] is_ue_configured_multiple_serving_cells Flag indicating whether UE is configured with more than 1 serving
/// cell.
/// \param[in] ss_id SearchSpace Id.
/// \return Return the DCI size configuration.
static dci_size_config get_dci_size_config(const ue_cell_configuration& ue_cell_cfg,
                                           bool                         is_ue_configured_multiple_serving_cells,
                                           search_space_id              ss_id)
{
  const search_space_info&            ss_info                    = ue_cell_cfg.search_space(ss_id);
  const bwp_info&                     init_bwp                   = ue_cell_cfg.bwp(to_bwp_id(0));
  const bwp_info&                     active_bwp                 = *ss_info.bwp;
  const bwp_downlink_common&          init_dl_bwp                = *init_bwp.dl_common;
  const bwp_downlink_common&          active_dl_bwp_cmn          = *active_bwp.dl_common;
  const bwp_configuration&            active_dl_bwp              = active_dl_bwp_cmn.generic_params;
  const bwp_uplink_common&            init_ul_bwp                = *init_bwp.ul_common;
  const bwp_configuration&            active_ul_bwp              = active_bwp.ul_common->generic_params;
  const optional<rach_config_common>& opt_rach_cfg               = active_bwp.ul_common->rach_cfg_common;
  const optional<csi_meas_config>&    opt_csi_meas_cfg           = ue_cell_cfg.cfg_dedicated().csi_meas_cfg;
  const optional<uplink_config>&      opt_ul_cfg                 = ue_cell_cfg.cfg_dedicated().ul_config;
  const auto&                         opt_pdsch_cfg              = ue_cell_cfg.cfg_dedicated().init_dl_bwp.pdsch_cfg;
  const auto&                         opt_pdsch_serving_cell_cfg = ue_cell_cfg.cfg_dedicated().pdsch_serv_cell_cfg;

  dci_size_config dci_sz_cfg = dci_size_config{
      is_dci_format_monitored_in_ue_ss(ue_cell_cfg, active_bwp.bwp_id, true),
      is_dci_format_monitored_in_ue_ss(ue_cell_cfg, active_bwp.bwp_id, false),
      init_dl_bwp.generic_params.crbs.length(),
      active_dl_bwp.crbs.length(),
      init_ul_bwp.generic_params.crbs.length(),
      active_ul_bwp.crbs.length(),
      init_dl_bwp.pdcch_common.coreset0.has_value() ? init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length() : 0};
  dci_sz_cfg.cross_carrier_configured = false;
  dci_sz_cfg.sul_configured           = false;
  // TODO: Need to fetch from physical_cell_group_config.
  dci_sz_cfg.pdsch_harq_ack_cb = pdsch_harq_ack_codebook::dynamic;

  // Fill out parameters for Format 0_1.
  // TODO: Once additional UL BWPs are added to uplinkConfig, populate below field from configs.
  dci_sz_cfg.nof_ul_bwp_rrc         = 0;
  dci_sz_cfg.nof_ul_time_domain_res = ue_cell_cfg.search_space(ss_id).pusch_time_domain_list.size();
  dci_sz_cfg.report_trigger_size    = 0;
  if (opt_csi_meas_cfg.has_value() and opt_csi_meas_cfg.value().report_trigger_size.has_value()) {
    dci_sz_cfg.report_trigger_size = opt_csi_meas_cfg.value().report_trigger_size.value();
  }
  dci_sz_cfg.frequency_hopping_configured = false;
  dci_sz_cfg.tx_config_non_codebook       = false;
  dci_sz_cfg.ptrs_uplink_configured       = false;
  dci_sz_cfg.dynamic_beta_offsets         = false;
  dci_sz_cfg.transform_precoding_enabled  = false;
  dci_sz_cfg.pusch_res_allocation_type    = resource_allocation::resource_allocation_type_1;
  if (opt_ul_cfg.has_value()) {
    const optional<pusch_config>&              opt_pusch_cfg    = opt_ul_cfg.value().init_ul_bwp.pusch_cfg;
    const optional<srs_config>&                opt_srs_cfg      = opt_ul_cfg.value().init_ul_bwp.srs_cfg;
    const optional<pusch_serving_cell_config>& opt_pusch_sc_cfg = opt_ul_cfg.value().pusch_scell_cfg;
    if (opt_pusch_cfg.has_value()) {
      dci_sz_cfg.tx_config_non_codebook = opt_pusch_cfg.value().tx_cfg != pusch_config::tx_config::not_set and
                                          opt_pusch_cfg.value().tx_cfg == pusch_config::tx_config::non_codebook;
      if (opt_pusch_cfg.value().trans_precoder != pusch_config::transform_precoder::not_set) {
        dci_sz_cfg.transform_precoding_enabled =
            opt_pusch_cfg.value().trans_precoder == pusch_config::transform_precoder::enabled;
      } else if (opt_rach_cfg.has_value()) {
        dci_sz_cfg.transform_precoding_enabled = opt_rach_cfg.value().msg3_transform_precoder;
      }
      if ((opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.has_value() and
           opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().ptrs.has_value()) or
          (opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.has_value() and
           opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().ptrs.has_value())) {
        dci_sz_cfg.ptrs_uplink_configured = true;
      }
      if (opt_pusch_cfg.value().uci_cfg.has_value() and
          opt_pusch_cfg.value().uci_cfg.value().beta_offsets_cfg.has_value() and
          variant_holds_alternative<uci_on_pusch::beta_offsets_dynamic>(
              opt_pusch_cfg.value().uci_cfg.value().beta_offsets_cfg.value())) {
        dci_sz_cfg.dynamic_beta_offsets = true;
      }
      switch (opt_pusch_cfg.value().res_alloc) {
        case pusch_config::resource_allocation::resource_allocation_type_0: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::resource_allocation_type_0;
          // TODO: Once rbg-Size is declared in PUSCH-Config, use the value from config.
          dci_sz_cfg.nof_ul_rb_groups = static_cast<unsigned>(get_nominal_rbg_size(active_ul_bwp.crbs.length(), true));
          break;
        }
        case pusch_config::resource_allocation::resource_allocation_type_1: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::resource_allocation_type_1;
          break;
        }
        case pusch_config::resource_allocation::dynamic_switch: {
          dci_sz_cfg.pusch_res_allocation_type = resource_allocation::dynamic_switch;
          // TODO: Once rbg-Size is defined in PUSCH-Config, use the value from config.
          dci_sz_cfg.nof_ul_rb_groups = static_cast<unsigned>(get_nominal_rbg_size(active_ul_bwp.crbs.length(), true));
          break;
        }
      }
      if (not dci_sz_cfg.tx_config_non_codebook and opt_pusch_cfg.value().max_rank.has_value()) {
        dci_sz_cfg.max_rank = opt_pusch_cfg.value().max_rank.value();
      }
      if (opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.has_value()) {
        dci_sz_cfg.pusch_dmrs_A_type    = opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().is_dmrs_type2
                                              ? dmrs_config_type::type2
                                              : dmrs_config_type::type1;
        dci_sz_cfg.pusch_dmrs_A_max_len = opt_pusch_cfg.value().pusch_mapping_type_a_dmrs.value().is_max_length_len2
                                              ? dmrs_max_length::len2
                                              : dmrs_max_length::len1;
      }
      if (opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.has_value()) {
        dci_sz_cfg.pusch_dmrs_B_type    = opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().is_dmrs_type2
                                              ? dmrs_config_type::type2
                                              : dmrs_config_type::type1;
        dci_sz_cfg.pusch_dmrs_B_max_len = opt_pusch_cfg.value().pusch_mapping_type_b_dmrs.value().is_max_length_len2
                                              ? dmrs_max_length::len2
                                              : dmrs_max_length::len1;
      }
    }
    if (opt_srs_cfg.has_value()) {
      const srs_config::srs_resource_set::usage usage = dci_sz_cfg.tx_config_non_codebook
                                                            ? srs_config::srs_resource_set::usage::non_codebook
                                                            : srs_config::srs_resource_set::usage::codebook;
      // See TS 38.214, clause 6.1.1.1 and 6.1.1.2.
      const auto* srs_res_set = std::find_if(
          opt_srs_cfg.value().srs_res_set.begin(),
          opt_srs_cfg.value().srs_res_set.end(),
          [usage](const srs_config::srs_resource_set& res_set) { return res_set.srs_res_set_usage == usage; });
      srsran_assert(srs_res_set != opt_srs_cfg.value().srs_res_set.end(), "No valid SRS resource set found");
      srsran_assert(not srs_res_set->srs_res_id_list.empty(), "No SRS resource configured in SRS resource set");
      // As per TS 38.214, clause 6.1.1.1, When multiple SRS resources are configured by SRS-ResourceSet with usage set
      // to 'codebook', the UE shall expect that higher layer parameters nrofSRS-Ports in SRS-Resource in
      // SRS-ResourceSet shall be configured with the same value for all these SRS resources.
      const auto  srs_resource_id = srs_res_set->srs_res_id_list.front();
      const auto* srs_res =
          std::find_if(opt_srs_cfg.value().srs_res.begin(),
                       opt_srs_cfg.value().srs_res.end(),
                       [srs_resource_id](const srs_config::srs_resource& res) { return res.id == srs_resource_id; });
      srsran_assert(srs_res != opt_srs_cfg.value().srs_res.end(), "No valid SRS resource found");
      if (not dci_sz_cfg.tx_config_non_codebook) {
        dci_sz_cfg.nof_srs_ports = static_cast<unsigned>(srs_res->nof_ports);
      }
      dci_sz_cfg.nof_srs_resources = srs_res_set->srs_res_id_list.size();
    }
    if (not dci_sz_cfg.tx_config_non_codebook and dci_sz_cfg.nof_srs_ports.has_value() and
        dci_sz_cfg.nof_srs_ports.value() > 1) {
      switch (opt_pusch_cfg.value().cb_subset) {
        case pusch_config::codebook_subset::fully_and_partial_and_non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::fully_and_partial_and_non_coherent;
          break;
        case pusch_config::codebook_subset::partial_and_non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::partial_and_non_coherent;
          break;
        case pusch_config::codebook_subset::non_coherent:
          dci_sz_cfg.cb_subset = tx_scheme_codebook_subset::non_coherent;
          break;
        default:
          break;
      }
    }
    if (opt_pusch_sc_cfg.has_value() and opt_pusch_sc_cfg.value().cbg_tx.has_value()) {
      dci_sz_cfg.max_cbg_tb_pusch = static_cast<unsigned>(opt_pusch_sc_cfg.value().cbg_tx.value().max_cgb_per_tb);
    }
    if (dci_sz_cfg.tx_config_non_codebook) {
      // TODO: Set value based on maxMIMO-Layers config in PUSCH-ServingCellConfig or UE capability.
      dci_sz_cfg.pusch_max_layers = 1;
    }
  }
  if (dci_sz_cfg.pdsch_harq_ack_cb == pdsch_harq_ack_codebook::dynamic) {
    // TODO: Set value based on config.
    dci_sz_cfg.dynamic_dual_harq_ack_cb = false;
  }

  // Fill out parameters for Format 1_1.
  dci_sz_cfg.nof_dl_bwp_rrc            = ue_cell_cfg.cfg_dedicated().dl_bwps.size();
  dci_sz_cfg.nof_dl_time_domain_res    = ue_cell_cfg.search_space(ss_id).pdsch_time_domain_list.size();
  dci_sz_cfg.nof_aperiodic_zp_csi      = 0;
  dci_sz_cfg.nof_pdsch_ack_timings     = ss_info.get_k1_candidates().size();
  dci_sz_cfg.dynamic_prb_bundling      = false;
  dci_sz_cfg.rm_pattern_group1         = false;
  dci_sz_cfg.rm_pattern_group2         = false;
  dci_sz_cfg.pdsch_two_codewords       = false;
  dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::resource_allocation_type_1;
  if (opt_pdsch_cfg.has_value()) {
    dci_sz_cfg.dynamic_prb_bundling =
        variant_holds_alternative<prb_bundling::dynamic_bundling>(opt_pdsch_cfg.value().prb_bndlg.bundling);
    dci_sz_cfg.pdsch_two_codewords = opt_pdsch_cfg.value().is_max_cw_sched_by_dci_is_two;
    switch (opt_pdsch_cfg.value().res_alloc) {
      case pdsch_config::resource_allocation::resource_allocation_type_0: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::resource_allocation_type_0;
        dci_sz_cfg.nof_dl_rb_groups          = static_cast<unsigned>(
            get_nominal_rbg_size(active_dl_bwp.crbs.length(), opt_pdsch_cfg.value().rbg_sz == rbg_size::config1));
        break;
      }
      case pdsch_config::resource_allocation::resource_allocation_type_1: {
        dci_sz_cfg.pdsch_res_allocation_type   = resource_allocation::resource_allocation_type_1;
        dci_sz_cfg.interleaved_vrb_prb_mapping = false;
        if (opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value()) {
          dci_sz_cfg.interleaved_vrb_prb_mapping = opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value();
        }
        break;
      }
      case pdsch_config::resource_allocation::dynamic_switch: {
        dci_sz_cfg.pdsch_res_allocation_type = resource_allocation::dynamic_switch;
        dci_sz_cfg.nof_dl_rb_groups          = static_cast<unsigned>(
            get_nominal_rbg_size(active_dl_bwp.crbs.length(), opt_pdsch_cfg.value().rbg_sz == rbg_size::config1));
        dci_sz_cfg.interleaved_vrb_prb_mapping = false;
        if (opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value()) {
          dci_sz_cfg.interleaved_vrb_prb_mapping = opt_pdsch_cfg.value().vrb_to_prb_itlvr.has_value();
        }
        break;
      }
    }
    if (opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.has_value()) {
      dci_sz_cfg.pdsch_dmrs_A_type    = opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.value().is_dmrs_type2
                                            ? dmrs_config_type::type2
                                            : dmrs_config_type::type1;
      dci_sz_cfg.pdsch_dmrs_A_max_len = opt_pdsch_cfg.value().pdsch_mapping_type_a_dmrs.value().is_max_length_len2
                                            ? dmrs_max_length::len2
                                            : dmrs_max_length::len1;
    }
    if (opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.has_value()) {
      dci_sz_cfg.pdsch_dmrs_B_type    = opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.value().is_dmrs_type2
                                            ? dmrs_config_type::type2
                                            : dmrs_config_type::type1;
      dci_sz_cfg.pdsch_dmrs_B_max_len = opt_pdsch_cfg.value().pdsch_mapping_type_b_dmrs.value().is_max_length_len2
                                            ? dmrs_max_length::len2
                                            : dmrs_max_length::len1;
    }
  }
  dci_sz_cfg.multiple_scells     = is_ue_configured_multiple_serving_cells;
  dci_sz_cfg.pdsch_tci           = false;
  dci_sz_cfg.cbg_flush_indicator = false;
  if (opt_pdsch_serving_cell_cfg.has_value()) {
    dci_sz_cfg.cbg_flush_indicator = false;
    if (opt_pdsch_serving_cell_cfg.value().code_block_group_tx.has_value()) {
      dci_sz_cfg.cbg_flush_indicator =
          opt_pdsch_serving_cell_cfg.value().code_block_group_tx.value().code_block_group_flush_indicator;
      dci_sz_cfg.max_cbg_tb_pdsch =
          static_cast<unsigned>(opt_pdsch_serving_cell_cfg.value().code_block_group_tx.value().max_cbg_per_tb);
    }
  }

  return dci_sz_cfg;
}

/// Find the number of DL ports for a given UE serving cell configuration.
static unsigned compute_nof_dl_ports(const serving_cell_config& serv_cell_cfg)
{
  if (not serv_cell_cfg.csi_meas_cfg.has_value()) {
    return 1;
  }

  unsigned max_ports = 1;
  for (const auto& nzp : serv_cell_cfg.csi_meas_cfg->nzp_csi_rs_res_list) {
    if (max_ports < nzp.res_mapping.nof_ports) {
      max_ports = nzp.res_mapping.nof_ports;
    }
  }
  return max_ports;
}

static units::bits get_dl_dci_size(const search_space_info& ss_info)
{
  if (ss_info.cfg->is_common_search_space()) {
    return ss_info.dci_sz.format1_0_common_size.total;
  }
  return ss_info.get_dl_dci_format() == dci_dl_format::f1_0 ? ss_info.dci_sz.format1_0_ue_size->total
                                                            : ss_info.dci_sz.format1_1_ue_size->total;
}

/// List of PDCCH candidates for different SearchSpaces of a BWP for different slot indexes.
using frame_pdcch_candidate_list =
    slotted_id_table<search_space_id,
                     std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>>,
                     MAX_NOF_SEARCH_SPACE_PER_BWP>;

/// \brief Implement TS 38.213, section 10.1 rules to remove ambiguous PDCCH candidates.
///
/// "A PDCCH candidate with index m^{(L)}_{s_j,n_{CI}} for a search space s_j  using a set of L CCEs in a CORESET p on
/// the active DL BWP for serving cell n_{CI} is not counted for monitoring if there is a PDCCH candidate with index
/// m^{(L)}_{s_i,n_{CI}} for a search space set ss_i < ss_j, or if there is a PDCCH candidate with index
/// n^{(L)}_{s_j,n_{CI}} and n^{(L)}_{s_j,n_{CI}} < m^{(L)}_{s_j,n_{CI}}, in the CORESET p on the active DL BWP for
/// serving cell n_{CI} using the same set of L CCEs, the PDCCH candidates have identical scrambling, and the
/// corresponding DCI formats for the PDCCH candidates have the same size; otherwise, the PDCCH candidate with index
/// m^{(L)}_{s_j,n_{CI}} is counted for monitoring."
/// \param[in/out] candidates List of PDCCH candidates for each SearchSpace, slot index and aggregation level.
/// \param[in] bwp BWP configuration.
static void remove_ambiguous_pdcch_candidates(frame_pdcch_candidate_list& candidates, const bwp_info& bwp)
{
  const unsigned ss_period_lcm = candidates.begin()->size();
  for (unsigned slot_index = 0; slot_index != ss_period_lcm; ++slot_index) {
    // Conditions only apply to candidates with same set of CCES, thus, same aggregation level.
    for (unsigned i = 0; i != NOF_AGGREGATION_LEVELS; ++i) {
      for (auto ss_it = bwp.search_spaces.begin(); ss_it != bwp.search_spaces.end(); ++ss_it) {
        const search_space_info& ss1            = **ss_it;
        pdcch_candidate_list&    ss_candidates1 = candidates[ss1.cfg->get_id()][slot_index][i];
        if (ss_candidates1.empty()) {
          // shortcut.
          continue;
        }

        // Case: Same SearchSpaceId "s_j" (See above).
        for (unsigned idx1 = 0; idx1 != ss_candidates1.size(); ++idx1) {
          for (unsigned idx2 = idx1 + 1; idx2 != ss_candidates1.size();) {
            if (ss_candidates1[idx1] == ss_candidates1[idx2]) {
              // Case: ss_candidates1[idx1] is PDCCH candidate with index n^{(L)}_{s_j,n_{CI}} (See above).
              ss_candidates1.erase(ss_candidates1.begin() + idx2);
            } else {
              idx2++;
            }
          }
        }

        // Case: Search Spaces "s_i < s_j" (See above).
        auto ss_it2 = ss_it;
        ++ss_it2;
        for (; ss_it2 != bwp.search_spaces.end(); ++ss_it2) {
          const search_space_info& ss2 = **ss_it2;

          if (ss2.coreset->id != ss1.coreset->id or get_dl_dci_size(ss1) != get_dl_dci_size(ss2)) {
            // Conditions only apply to same CORESET p, same DCI sizes.
            // TODO: The TS refers to candidates having the same scrambling, but if they are in the same CORESET, this
            // is implied.
            continue;
          }

          pdcch_candidate_list& ss_candidates2 = candidates[ss2.cfg->get_id()][slot_index][i];
          for (const uint8_t candidate1 : ss_candidates1) {
            for (auto it2 = ss_candidates2.begin(); it2 != ss_candidates2.end();) {
              if (*it2 == candidate1) {
                // Case: *candidate_it1 is PDCCH candidate with index m^{(L)}_{s_i,n_{CI}} (See above).
                it2 = ss_candidates2.erase(it2);
              } else {
                ++it2;
              }
            }
          }
        }
      }
    }
  }
}

/// \brief This function implements TS 38.213, section 10.1 rules to limit the number of monitored PDCCH candidates
/// per slot, in particular, the limits in tables 10.1-2 and 10.1-3.
static void apply_pdcch_candidate_monitoring_limits(frame_pdcch_candidate_list& candidates, const bwp_info& bwp)
{
  // TS 38.213, Table 10.1-3 - Maximum number of non-overlapped CCEs.
  const static std::array<uint8_t, 4> max_non_overlapped_cces_per_slot = {56, 56, 48, 32};
  // Maximum nof. CCEs in a CORESET = maximum PDCCH frequency resources * maximum CORESET duration.
  const static unsigned maximum_nof_cces =
      pdcch_constants::MAX_NOF_FREQ_RESOURCES * pdcch_constants::MAX_CORESET_DURATION;

  const unsigned max_pdcch_candidates = max_nof_monitored_pdcch_candidates(bwp.dl_common->generic_params.scs);
  const unsigned max_non_overlapped_cces =
      max_non_overlapped_cces_per_slot[to_numerology_value(bwp.dl_common->generic_params.scs)];

  const unsigned ss_period_lcm = candidates.begin()->size();
  for (unsigned slot_index = 0; slot_index != ss_period_lcm; ++slot_index) {
    // Limit number of PDCCH candidates for the slot.
    unsigned candidate_count = 0;
    for (const search_space_info* ss : bwp.search_spaces) {
      for (unsigned i = 0; i != NOF_AGGREGATION_LEVELS; ++i) {
        pdcch_candidate_list& ss_candidates = candidates[ss->cfg->get_id()][slot_index][i];

        if (max_pdcch_candidates < candidate_count + ss_candidates.size()) {
          ss_candidates.resize(max_pdcch_candidates - candidate_count);
        }
        candidate_count += ss_candidates.size();
      }
    }

    // Limit number of non-overlapped CCEs monitored per slot.
    // CCES for PDCCH candidates are non-overlapped if they correspond to:
    // - different CORESET indexes; or
    // - different first symbols for the reception of the respective PDCCH candidates.
    // TODO: Account for multiple SSB beams.
    std::array<std::array<bounded_bitset<maximum_nof_cces>, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP>, MAX_NOF_CORESETS_PER_BWP>
        cce_bitmap_per_coreset_per_first_pdcch_symb;
    for (unsigned cs_idx = 0; cs_idx < MAX_NOF_CORESETS_PER_BWP; ++cs_idx) {
      std::fill(cce_bitmap_per_coreset_per_first_pdcch_symb[cs_idx].begin(),
                cce_bitmap_per_coreset_per_first_pdcch_symb[cs_idx].end(),
                bounded_bitset<maximum_nof_cces>(maximum_nof_cces));
    }
    auto get_cce_monitored_sum = [&cce_bitmap_per_coreset_per_first_pdcch_symb]() {
      unsigned monitored_cces = 0;
      for (unsigned cs_idx = 0; cs_idx < MAX_NOF_CORESETS_PER_BWP; ++cs_idx) {
        for (unsigned symb = 0; symb < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP; ++symb) {
          monitored_cces += cce_bitmap_per_coreset_per_first_pdcch_symb[cs_idx][symb].count();
        }
      }
      return monitored_cces;
    };
    for (unsigned ss1_idx = 0; ss1_idx < bwp.search_spaces.size(); ++ss1_idx) {
      const search_space_info* ss1 = bwp.search_spaces[static_cast<search_space_id>(ss1_idx)];
      for (unsigned i = 0; i != NOF_AGGREGATION_LEVELS; ++i) {
        pdcch_candidate_list& ss_candidates = candidates[ss1->cfg->get_id()][slot_index][i];
        for (auto* it2 = ss_candidates.begin(); it2 != ss_candidates.end();) {
          // Take backup of monitored CCEs at current CORESET and first PDCCH monitoring occasion symbol of
          // corresponding SearchSpace. This is used to reset the monitored CCEs back to original state if max. nof.
          // non-overlapped CCEs monitored limit is exceeded.
          const bounded_bitset<maximum_nof_cces> cce_monitored_backup(
              cce_bitmap_per_coreset_per_first_pdcch_symb[ss1->coreset->id][ss1->cfg->get_first_symbol_index()]);
          // Set the CCEs monitored.
          cce_bitmap_per_coreset_per_first_pdcch_symb[ss1->coreset->id][ss1->cfg->get_first_symbol_index()].fill(
              *it2, *it2 + to_nof_cces(static_cast<aggregation_level>(i)), true);
          if (get_cce_monitored_sum() > max_non_overlapped_cces) {
            // Case: max. nof. non-overlapped CCEs exceeded.
            it2 = ss_candidates.erase(it2);
            // Reset the monitored CCEs.
            cce_bitmap_per_coreset_per_first_pdcch_symb[ss1->coreset->id][ss1->cfg->get_first_symbol_index()] =
                cce_monitored_backup;
          } else {
            ++it2;
          }
        }
      }
    }
  }
}

/// \brief Compute the list of PDCCH candidates being monitored for each SearchSpace for a given slot index.
static void generate_crnti_monitored_pdcch_candidates(bwp_info& bwp_cfg, rnti_t crnti, pci_t pci)
{
  const unsigned slots_per_frame =
      NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(bwp_cfg.dl_common->generic_params.scs);

  // We compute the candidates for all search spaces, considering their slot monitoring periodicity.
  unsigned max_slot_periodicity = 0;
  {
    static_vector<unsigned, MAX_NOF_SEARCH_SPACE_PER_BWP> ss_periods;
    for (const search_space_info* ss : bwp_cfg.search_spaces) {
      ss_periods.push_back(ss->cfg->get_monitoring_slot_periodicity());
    }
    max_slot_periodicity = lcm<unsigned>(ss_periods);
    max_slot_periodicity = lcm(max_slot_periodicity, slots_per_frame);
  }

  frame_pdcch_candidate_list candidates;

  // Compute PDCCH candidates for each Search Space, without accounting for special monitoring rules.
  for (search_space_info* ss : bwp_cfg.search_spaces) {
    candidates.emplace(ss->cfg->get_id());
    auto& ss_candidates = candidates[ss->cfg->get_id()];
    ss_candidates.resize(max_slot_periodicity);

    for (unsigned slot_count = 0; slot_count != ss_candidates.size(); ++slot_count) {
      const slot_point slot_mod{bwp_cfg.dl_common->generic_params.scs, slot_count};

      // If the SearchSpace is not monitored in this slot, skip its candidate generation.
      if (not pdcch_helper::is_pdcch_monitoring_active(slot_mod, *ss->cfg)) {
        continue;
      }

      for (unsigned i = 0; i != NOF_AGGREGATION_LEVELS; ++i) {
        const aggregation_level aggr_lvl = aggregation_index_to_level(i);

        if (ss->cfg->is_common_search_space()) {
          ss_candidates[slot_count][i] =
              pdcch_candidates_common_ss_get_lowest_cce(pdcch_candidates_common_ss_configuration{
                  aggr_lvl, ss->cfg->get_nof_candidates()[i], ss->coreset->get_nof_cces()});
        } else {
          ss_candidates[slot_count][i] = pdcch_candidates_ue_ss_get_lowest_cce(
              pdcch_candidates_ue_ss_configuration{aggr_lvl,
                                                   ss->cfg->get_nof_candidates()[i],
                                                   ss->coreset->get_nof_cces(),
                                                   ss->coreset->id,
                                                   crnti,
                                                   slot_mod.slot_index()});
        }
      }
    }
  }

  // Apply TS 38.213 section 10.1 rules to remove ambiguous candidates.
  remove_ambiguous_pdcch_candidates(candidates, bwp_cfg);

  // Apply TS 38.213 section 10.1 limits on the number of monitored PDCCH candidates and non-overlapped CCEs per slot.
  apply_pdcch_candidate_monitoring_limits(candidates, bwp_cfg);

  // Save resulting candidates for this slot.
  for (search_space_info* ss : bwp_cfg.search_spaces) {
    ss->update_pdcch_candidates(candidates[ss->cfg->get_id()], pci);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_cell_configuration::ue_cell_configuration(rnti_t                     crnti_,
                                             const cell_configuration&  cell_cfg_common_,
                                             const serving_cell_config& serv_cell_cfg_,
                                             bool                       multi_cells_configured_) :
  crnti(crnti_),
  cell_cfg_common(cell_cfg_common_),
  multi_cells_configured(multi_cells_configured_),
  nof_dl_ports(compute_nof_dl_ports(serv_cell_cfg_))
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

  // Compute DCI sizes
  for (search_space_info& ss : search_spaces) {
    ss.dci_sz_cfg = get_dci_size_config(*this, multi_cells_configured, ss.cfg->get_id());
    srsran_assert(
        validate_dci_size_config(ss.dci_sz_cfg), "Invalid DCI size configuration for SearchSpace={}", ss.cfg->get_id());
    ss.dci_sz = get_dci_sizes(ss.dci_sz_cfg);
  }

  // Generate PDCCH candidates.
  for (bwp_info& bwp : bwp_table) {
    if (bwp.dl_common != nullptr) {
      generate_crnti_monitored_pdcch_candidates(bwp, crnti, cell_cfg_common.pci);
    }
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
  for (const search_space_configuration& ss_cfg : bwp_dl_common.pdcch_common.search_spaces) {
    search_spaces.emplace(ss_cfg.get_id());
    search_space_info& ss = search_spaces[ss_cfg.get_id()];
    bwp_table[bwpid].search_spaces.emplace(ss_cfg.get_id(), &ss);

    ss.cfg     = &ss_cfg;
    ss.coreset = &*coresets[ss_cfg.get_coreset_id()];
    ss.bwp     = &bwp_table[bwpid];
    ss.pdsch_time_domain_list =
        get_c_rnti_pdsch_time_domain_list(ss_cfg, *ss.bwp->dl_common, nullptr, cell_cfg_common.dmrs_typeA_pos);
    ss.dl_crb_lims = pdsch_helper::get_ra_crb_limits(
        ss.get_dl_dci_format(), cell_cfg_common.dl_cfg_common.init_dl_bwp, *ss.bwp->dl_common, *ss.cfg, *ss.coreset);
  }
}

void ue_cell_configuration::configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_common = &bwp_ul_common;

  for (const search_space_configuration& ss_cfg : bwp_table[bwpid].dl_common->pdcch_common.search_spaces) {
    search_space_info& ss     = search_spaces[ss_cfg.get_id()];
    ss.pusch_time_domain_list = get_c_rnti_pusch_time_domain_list(ss_cfg, *bwp_table[bwpid].ul_common, nullptr);
    const dci_ul_rnti_config_type crnti_dci_type = ss.get_ul_dci_format() == dci_ul_format::f0_0
                                                       ? dci_ul_rnti_config_type::c_rnti_f0_0
                                                       : dci_ul_rnti_config_type::c_rnti_f0_1;
    ss.ul_crb_lims                               = pusch_helper::get_ra_crb_limits(crnti_dci_type,
                                                     cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params,
                                                     bwp_ul_common.generic_params,
                                                     ss.cfg->is_common_search_space());
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
  for (const search_space_configuration& ss_cfg : bwp_dl_ded.pdcch_cfg->search_spaces) {
    search_spaces.emplace(ss_cfg.get_id());
    search_space_info& ss = search_spaces[ss_cfg.get_id()];
    bwp_table[bwpid].search_spaces.emplace(ss_cfg.get_id(), &ss);

    ss.cfg                    = &ss_cfg;
    ss.coreset                = coresets[ss_cfg.get_coreset_id()];
    ss.bwp                    = &bwp_table[bwpid];
    ss.pdsch_time_domain_list = get_c_rnti_pdsch_time_domain_list(
        ss_cfg, *bwp_table[bwpid].dl_common, bwp_table[bwpid].dl_ded, cell_cfg_common.dmrs_typeA_pos);
    ss.dl_crb_lims = pdsch_helper::get_ra_crb_limits(
        ss.get_dl_dci_format(), cell_cfg_common.dl_cfg_common.init_dl_bwp, *ss.bwp->dl_common, *ss.cfg, *ss.coreset);
  }
}

void ue_cell_configuration::configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded)
{
  // Compute UL BWP-Id lookup table.
  bwp_table[bwpid].ul_ded = &bwp_ul_ded;

  for (const search_space_configuration& ss_cfg : bwp_table[bwpid].dl_ded->pdcch_cfg->search_spaces) {
    search_space_info& ss = search_spaces[ss_cfg.get_id()];
    ss.pusch_time_domain_list =
        get_c_rnti_pusch_time_domain_list(ss_cfg, *bwp_table[bwpid].ul_common, bwp_table[bwpid].ul_ded);
    const dci_ul_rnti_config_type crnti_dci_type = ss.get_ul_dci_format() == dci_ul_format::f0_0
                                                       ? dci_ul_rnti_config_type::c_rnti_f0_0
                                                       : dci_ul_rnti_config_type::c_rnti_f0_1;
    ss.ul_crb_lims                               = pusch_helper::get_ra_crb_limits(crnti_dci_type,
                                                     cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params,
                                                     ss.bwp->ul_common->generic_params,
                                                     ss.cfg->is_common_search_space());
  }
}
