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

#pragma once

#include "../cell/cell_configuration.h"
#include "../support/pdcch/search_space_helper.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

struct search_space_info;

/// \brief Grouping of common and UE-dedicated information associated with a given BWP.
struct bwp_info {
  bwp_id_t                      bwp_id;
  const bwp_downlink_common*    dl_common = nullptr;
  const bwp_downlink_dedicated* dl_ded    = nullptr;
  const bwp_uplink_common*      ul_common = nullptr;
  const bwp_uplink_dedicated*   ul_ded    = nullptr;

  /// \brief List of SearchSpaces associated with this BWP.
  slotted_id_table<search_space_id, search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP> search_spaces;
};

/// List of CRBs for a given PDCCH candidate.
using crb_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

/// \brief Grouping of common and UE-dedicated information associated with a given search space.
struct search_space_info {
  const search_space_configuration*                 cfg     = nullptr;
  const coreset_configuration*                      coreset = nullptr;
  const bwp_info*                                   bwp     = nullptr;
  crb_interval                                      dl_crb_lims;
  crb_interval                                      ul_crb_lims;
  span<const pdsch_time_domain_resource_allocation> pdsch_time_domain_list;
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list;
  dci_size_config                                   dci_sz_cfg;
  dci_sizes                                         dci_sz;

  /// \brief Gets DL DCI format type to use based on SearchSpace configuration.
  /// \return DL DCI format.
  dci_dl_format get_dl_dci_format() const { return pdcch_helper::get_dl_dci_format(*cfg); }

  /// \brief Gets UL DCI format type to use based on SearchSpace configuration.
  /// \return UL DCI format.
  dci_ul_format get_ul_dci_format() const { return pdcch_helper::get_ul_dci_format(*cfg); }

  /// \brief Get table of PDSCH-to-HARQ candidates as per TS38.213, clause 9.2.3.
  span<const uint8_t> get_k1_candidates() const;

  /// \brief Retrieve all the PDCCH candidates for a given aggregation level and slot for this SearchSpace.
  span<const uint8_t> get_pdcch_candidates(aggregation_level aggr_lvl, slot_point pdcch_slot) const
  {
    return ss_pdcch_candidates[pdcch_slot.to_uint() % ss_pdcch_candidates.size()][to_aggregation_level_index(aggr_lvl)];
  }

  /// \brief Retrieve all the CRBs for a given aggregation level and searchSpace candidate.
  span<const crb_index_list> get_crb_list_of_pdcch_candidates(aggregation_level aggr_lvl, slot_point pdcch_slot) const
  {
    return crbs_of_candidates[pdcch_slot.to_uint() % crbs_of_candidates.size()][to_aggregation_level_index(aggr_lvl)];
  }

  /// \brief Assigns computed PDCCH candidates to a SearchSpace.
  void update_pdcch_candidates(const std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>>& candidates,
                               pci_t                                                                        pci);

private:
  // PDCCH candidates of the SearchSpace for different slot offsets and aggregation levels. Indexed by
  // ss_pddch_candidates[slot_offset % ss_pdcch_candidates.size()][aggr_level_index][candidate_index].
  // We need to keep separate lists for different slot offsets because PDCCH candidates change with the slot index,
  // may have a monitoring periodicity above 1 slot, and may be affected by the candidates of other search spaces.
  std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>> ss_pdcch_candidates;

  // List of CRBs used by each PDCCH candidate.
  std::vector<std::array<std::vector<crb_index_list>, NOF_AGGREGATION_LEVELS>> crbs_of_candidates;
};

/// UE-dedicated configuration for a given cell.
class ue_cell_configuration
{
public:
  ue_cell_configuration(rnti_t                     crnti_,
                        const cell_configuration&  cell_cfg_common_,
                        const serving_cell_config& serv_cell_cfg_,
                        bool                       multi_cells_configured = false);
  ue_cell_configuration(const ue_cell_configuration&)            = delete;
  ue_cell_configuration(ue_cell_configuration&&)                 = delete;
  ue_cell_configuration& operator=(const ue_cell_configuration&) = delete;
  ue_cell_configuration& operator=(ue_cell_configuration&&)      = delete;

  void reconfigure(const serving_cell_config& cell_cfg_ded_);

  const rnti_t              crnti;
  const cell_configuration& cell_cfg_common;

  const serving_cell_config& cfg_dedicated() const { return cell_cfg_ded; }

  bool has_bwp_id(bwp_id_t bwp_id) const { return bwp_table[bwp_id].dl_common != nullptr; }

  /// Get BWP information given a BWP-Id.
  const bwp_info* find_bwp(bwp_id_t bwp_id) const { return has_bwp_id(bwp_id) ? &bwp_table[bwp_id] : nullptr; }
  const bwp_info& bwp(bwp_id_t bwp_id) const
  {
    const bwp_info* bwp = find_bwp(bwp_id);
    srsran_assert(bwp != nullptr, "Invalid BWP-Id={} access", bwp_id);
    return *bwp;
  }

  /// Fetches CORESET configuration based on Coreset-Id.
  const coreset_configuration* find_coreset(coreset_id cs_id) const { return coresets[cs_id]; }
  const coreset_configuration& coreset(coreset_id cs_id) const
  {
    const coreset_configuration* ret = find_coreset(cs_id);
    srsran_assert(ret != nullptr, "Inexistent CORESET-Id={}", cs_id);
    return *ret;
  }

  /// Fetches SearchSpace configuration based on SearchSpace-Id.
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  const search_space_info* find_search_space(search_space_id ss_id) const
  {
    return search_spaces.contains(ss_id) ? &search_spaces[ss_id] : nullptr;
  }
  const search_space_info& search_space(search_space_id ss_id) const { return search_spaces[ss_id]; }

  /// Get the number of active DL ports for this UE.
  unsigned get_nof_dl_ports() const { return nof_dl_ports; }

private:
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl_common);
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded);

  /// Dedicated cell configuration.
  serving_cell_config cell_cfg_ded;
  bool                multi_cells_configured;

  /// Lookup table for BWP params indexed by BWP-Id.
  std::array<bwp_info, MAX_NOF_BWPS> bwp_table = {};

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace parameters (the array values).
  slotted_array<search_space_info, MAX_NOF_SEARCH_SPACES> search_spaces;

  /// This array maps Coreset-Ids (the array indexes) to CORESET configurations (the array values).
  /// Note: The ID space of CoresetIds is common across all the BWPs of a Serving Cell.
  std::array<const coreset_configuration*, MAX_NOF_CORESETS> coresets = {};

  /// This array maps Coreset-Ids (the array indexes) to BWP-Ids (the array values).
  std::array<bwp_id_t, MAX_NOF_BWPS> coreset_id_to_bwp_id;

  /// Number of DL ports for this UE.
  unsigned nof_dl_ports = 1;
};

} // namespace srsran
