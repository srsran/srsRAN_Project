/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/scheduler/config/sched_cell_config_helpers.h"
#include "srsran/scheduler/config/pucch_resource_generator.h"
#include <map>

using namespace srsran;

std::vector<sched_grid_resource>
srsran::config_helpers::build_pucch_guardbands_list(const pucch_builder_params& user_params, unsigned bwp_size)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  std::vector<pucch_resource> res_list = config_helpers::generate_cell_pucch_res_list(
      user_params.nof_ue_pucch_f0_or_f1_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_sr_resources,
      user_params.nof_ue_pucch_f2_or_f3_or_f4_res_harq.to_uint() * user_params.nof_cell_harq_pucch_res_sets +
          user_params.nof_csi_resources,
      user_params.f0_or_f1_params,
      user_params.f2_or_f3_or_f4_params,
      bwp_size,
      user_params.max_nof_symbols);

  srsran_assert(not res_list.empty(), "The PUCCH resource list cannot be empty");

  std::vector<sched_grid_resource> pucch_guardbands;

  auto list_contains_resource = [&pucch_guardbands](const sched_grid_resource& res) {
    return std::find(pucch_guardbands.begin(), pucch_guardbands.end(), res) != pucch_guardbands.end();
  };

  for (const auto& pucch_res : res_list) {
    unsigned starting_sym = pucch_res.starting_sym_idx;
    unsigned nof_symbols  = pucch_res.nof_symbols;

    // For PUCCH Formats 0/1/4, the resource has 1 PRB only.
    const unsigned nof_prbs = std::holds_alternative<pucch_format_2_3_cfg>(pucch_res.format_params)
                                  ? std::get<pucch_format_2_3_cfg>(pucch_res.format_params).nof_prbs
                                  : 1U;

    // In the following, \c res_no_freq_hop contains the PRBs/symbols of the PUCCH resource with no frequency hopping,
    // or, if frequency hopping is enabled, the PRBs/symbols of the first hop.
    // \c res_freq_hop is only used if frequency hopping is enabled and contains the PRBs/symbols of the second hop.
    sched_grid_resource res_no_freq_hop;
    sched_grid_resource res_freq_hop;
    res_no_freq_hop.prbs.set(pucch_res.starting_prb, pucch_res.starting_prb + nof_prbs);

    if (pucch_res.second_hop_prb.has_value()) {
      res_freq_hop.prbs.set(pucch_res.second_hop_prb.value(), pucch_res.second_hop_prb.value() + nof_prbs);
      res_no_freq_hop.symbols.set(starting_sym, starting_sym + nof_symbols / 2);
      res_freq_hop.symbols.set(starting_sym + nof_symbols / 2, starting_sym + nof_symbols);
    } else {
      res_no_freq_hop.symbols.set(starting_sym, starting_sym + nof_symbols);
    }

    if (not res_no_freq_hop.is_empty() and not list_contains_resource(res_no_freq_hop)) {
      pucch_guardbands.emplace_back(res_no_freq_hop);
    }
    if (not res_freq_hop.is_empty() and not list_contains_resource(res_freq_hop)) {
      pucch_guardbands.emplace_back(res_freq_hop);
    }
  }
  return pucch_guardbands;
}

unsigned
srsran::config_helpers::compute_tot_nof_monitored_pdcch_candidates_per_slot(const serving_cell_config& ue_cell_cfg,
                                                                            const dl_config_common&    dl_cfg_common)
{
  // NOTE: We assume DCI formats other than 1_0, 0_0, 1_1 and 0_1 are not configured in SearchSpaces.
  // NOTE: Total nof. monitored PDCCH candidates are calculated considering a slot at which all SearchSpaces are active
  // for simplification.

  // As per TS 38.213, clause 10.1, "A UE expects to monitor PDCCH candidates for up to 4 sizes of DCI formats that
  // include up to 3 sizes of DCI formats with CRC scrambled by C-RNTI per serving cell. The UE counts a number of sizes
  // for DCI formats per serving cell based on a number of configured PDCCH candidates in respective search space sets
  // for the corresponding active DL BWP".

  // As per TS 38.212, clause 7.3.1.2.1, "If DCI format 1_0 is monitored in UE specific search space and satisfies both
  // of the following
  // - the total number of different DCI sizes monitored per slot is no more than 4 for the cell, and
  // - the total number of different DCI sizes with C-RNTI monitored per slot is no more than 3 for the cell
  // and if the number of information bits in the DCI format 1_0 prior to padding is less than the payload size of the
  // DCI format 0_0 monitored in UE specific search space for scheduling the same serving cell, zeros shall be appended
  // to the DCI format 1_0 until the payload size equals that of the DCI format 0_0".

  // NOTE: After performing the DCI size alignment mentioned in TS 38.212 (release 16), clause 7.3.1.0. The size of DCI
  // 1_0 scrambled by C-RNTI, TC-RNTI, P-RNTI, SI-RNTI and RA-RNTI will be same. And, the size of DCI 0_0 scrambled by
  // C-RNTI will be same as size of DCI 1_0 scrambled by C-RNTI.
  static const unsigned nof_fallback_dci_sizes_to_be_monitored = 1;
  // UE needs to monitor PDCCH candidates for both DCI 1_1 and 0_1, which have different sizes.
  static const unsigned nof_non_fallback_dci_sizes_to_be_monitored_in_uss = 2;

  std::map<search_space_id, unsigned> nof_monitored_pdcch_candidates_per_ss;

  const bwp_downlink_common&    bwp_cmn = dl_cfg_common.init_dl_bwp;
  const bwp_downlink_dedicated& bwp_ded = ue_cell_cfg.init_dl_bwp;

  for (const search_space_configuration& ss : bwp_ded.pdcch_cfg->search_spaces) {
    const auto& dci_format_variant = ss.get_monitored_dci_formats();
    const bool  non_fallback_dci_fmt =
        std::holds_alternative<search_space_configuration::ue_specific_dci_format>(dci_format_variant) and
        std::get<search_space_configuration::ue_specific_dci_format>(dci_format_variant) ==
            search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;

    unsigned nof_monitored_pdcch_candidates;
    if (ss.is_common_search_space() or not non_fallback_dci_fmt) {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_fallback_dci_sizes_to_be_monitored;
    } else {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_non_fallback_dci_sizes_to_be_monitored_in_uss;
    }

    for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
      const auto it = std::find_if(
          bwp_ded.pdcch_cfg->search_spaces.begin(),
          bwp_ded.pdcch_cfg->search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      // [Implementation-defined] Reset nof. monitored PDCCH candidates for earlier accounted SearchSpace so that we
      // account only the highest nof. monitored PDCCH candidates for SeachSpaces sharing the same CORESET and
      // same DCI formats. This is done to simplify calculation of total nof. PDCCH candidates monitored across all
      // SearchSpaces.
      // For example: Assume SS#2 and SS#3 share CORESET#1 and monitoring DCI format 1_0/0_0. And, nof.
      // PDCCH candidates to monitor is 4 and 12 respectively. In this case, we consider only 12 PDCCH candidates to
      // monitor.
      if (it != bwp_ded.pdcch_cfg->search_spaces.end() and it->get_coreset_id() == ss.get_coreset_id() and
          it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
    }
    nof_monitored_pdcch_candidates_per_ss[ss.get_id()] = nof_monitored_pdcch_candidates;
  }

  for (const search_space_configuration& ss : bwp_cmn.pdcch_common.search_spaces) {
    const auto& dci_format_variant = ss.get_monitored_dci_formats();
    const bool  non_fallback_dci_fmt =
        std::holds_alternative<search_space_configuration::ue_specific_dci_format>(dci_format_variant) and
        std::get<search_space_configuration::ue_specific_dci_format>(dci_format_variant) ==
            search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;

    unsigned nof_monitored_pdcch_candidates;
    if (ss.is_common_search_space() or not non_fallback_dci_fmt) {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_fallback_dci_sizes_to_be_monitored;
    } else {
      nof_monitored_pdcch_candidates =
          std::accumulate(ss.get_nof_candidates().begin(), ss.get_nof_candidates().end(), 0) *
          nof_non_fallback_dci_sizes_to_be_monitored_in_uss;
    }

    for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
      const auto it = std::find_if(
          bwp_ded.pdcch_cfg->search_spaces.begin(),
          bwp_ded.pdcch_cfg->search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      // [Implementation-defined] Reset nof. monitored PDCCH candidates for earlier accounted SearchSpace so that we
      // account only the highest nof. monitored PDCCH candidates for SeachSpaces sharing the same CORESET and
      // same DCI formats. This is done to simplify calculation of total nof. PDCCH candidates monitored across all
      // SearchSpaces.
      // For example: Assume SS#2 and SS#3 share CORESET#1 and monitoring DCI format 1_0/0_0. And, nof.
      // PDCCH candidates to monitor is 4 and 12 respectively. In this case, we consider only 12 PDCCH candidates to
      // monitor.
      if (it != bwp_ded.pdcch_cfg->search_spaces.end() and it->get_coreset_id() == ss.get_coreset_id() and
          it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
      const auto cmn_it = std::find_if(
          bwp_cmn.pdcch_common.search_spaces.begin(),
          bwp_cmn.pdcch_common.search_spaces.end(),
          [ss_id = entry.first](const search_space_configuration& ss_cfg) { return ss_cfg.get_id() == ss_id; });
      if (cmn_it != bwp_cmn.pdcch_common.search_spaces.end() and cmn_it->get_coreset_id() == ss.get_coreset_id() and
          cmn_it->get_monitored_dci_formats() == ss.get_monitored_dci_formats()) {
        if (nof_monitored_pdcch_candidates > nof_monitored_pdcch_candidates_per_ss[entry.first]) {
          nof_monitored_pdcch_candidates_per_ss[entry.first] = 0;
          break;
        }
        nof_monitored_pdcch_candidates = 0;
      }
    }
    nof_monitored_pdcch_candidates_per_ss[ss.get_id()] = nof_monitored_pdcch_candidates;
  }

  unsigned total_nof_monitored_pdcch_candidates = 0;
  for (const auto& entry : nof_monitored_pdcch_candidates_per_ss) {
    total_nof_monitored_pdcch_candidates += entry.second;
  }

  return total_nof_monitored_pdcch_candidates;
}
