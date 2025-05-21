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

#pragma once

#include "../support/pdcch/search_space_helper.h"
#include "../support/pdsch/pdsch_config_params.h"
#include "cell_configuration.h"
#include "logical_channel_list_config.h"
#include "sched_config_params.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"
#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include <optional>

namespace srsran {

struct ue_creation_params;
struct ue_reconfig_params;
struct ue_cell_res_config;
class ue_cell_configuration;
struct search_space_info;

/// List of CRBs for a given PDCCH candidate.
using crb_index_list = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;

/// \brief Grouping of common and UE-dedicated information associated with a given search space.
struct search_space_info {
  const search_space_configuration*                 cfg     = nullptr;
  const coreset_configuration*                      coreset = nullptr;
  std::optional<vrb_to_prb::interleaved_mapping>    interleaved_mapping;
  bwp_config_ptr                                    bwp;
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

  const pdsch_config_params& get_pdsch_config(unsigned pdsch_td_res_index, unsigned nof_layers) const
  {
    srsran_assert(pdsch_cfg_list.size() > pdsch_td_res_index and
                      pdsch_cfg_list[pdsch_td_res_index].size() >= nof_layers,
                  "get_pdsch_config: Invalid params");
    return pdsch_cfg_list[pdsch_td_res_index][nof_layers - 1];
  }

  /// \brief Assigns computed PDCCH candidates to a SearchSpace.
  void update_pdcch_candidates(const std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>>& candidates,
                               pci_t                                                                        pci);

  void update_pdsch_time_domain_list(const ue_cell_configuration& ue_cell_cfg);

  void update_pdsch_mappings(vrb_to_prb::mapping_type interleaving_bundle_size);

private:
  // PDCCH candidates of the SearchSpace for different slot offsets and aggregation levels. Indexed by
  // ss_pddch_candidates[slot_offset % ss_pdcch_candidates.size()][aggr_level_index][candidate_index].
  // We need to keep separate lists for different slot offsets because PDCCH candidates change with the slot index,
  // may have a monitoring periodicity above 1 slot, and may be affected by the candidates of other search spaces.
  std::vector<std::array<pdcch_candidate_list, NOF_AGGREGATION_LEVELS>> ss_pdcch_candidates;

  // List of CRBs used by each PDCCH candidate.
  std::vector<std::array<std::vector<crb_index_list>, NOF_AGGREGATION_LEVELS>> crbs_of_candidates;

  // Mapping of PDSCH time domain resources to lists of PDSCH Configs. Each index corresponds to a different PDSCH
  // time resource index, which points to a list whose indexes correspond to different numbers of DL layers.
  std::vector<std::vector<pdsch_config_params>> pdsch_cfg_list;
};

/// UE-dedicated configuration for a given cell.
class ue_cell_configuration
{
public:
  ue_cell_configuration(rnti_t                                crnti_,
                        const cell_configuration&             cell_cfg_common_,
                        const ue_cell_config_ptr&             ue_cell_params,
                        const std::optional<meas_gap_config>& meas_gap_cfg_          = std::nullopt,
                        bool                                  multi_cells_configured = false);
  ue_cell_configuration(const ue_cell_configuration& other);
  ue_cell_configuration(ue_cell_configuration&&)                 = delete;
  ue_cell_configuration& operator=(const ue_cell_configuration&) = delete;
  ue_cell_configuration& operator=(ue_cell_configuration&&)      = delete;

  void reconfigure(const ue_cell_config_ptr&             ue_cell_params,
                   const std::optional<meas_gap_config>& meas_gaps = std::nullopt,
                   const std::optional<drx_config>&      drx_cfg_  = std::nullopt);

  void set_rrm_config(const sched_ue_resource_alloc_config& ue_res_alloc_cfg);

  const rnti_t              crnti;
  const cell_configuration& cell_cfg_common;

  /// Retrieve the parameters relative to the RRM of a UE in the scheduler.
  const sched_ue_resource_alloc_config& rrm_cfg() const { return ue_res_alloc_cfg; }

  const std::optional<drx_config>& get_drx_cfg() const { return drx_cfg; }

  /// Returns whether UE dedicated configuration is considered complete or not for scheduling the UE as a non-fallback
  /// UE.
  /// \remark UE can be scheduled in fallback scheduler even if UE does not have a complete UE dedicated configuration.
  bool is_cfg_dedicated_complete() const;

  /// Get BWP information given a BWP-Id.
  bool                   has_bwp_id(bwp_id_t bwp_id) const { return cell_ded->bwps.contains(bwp_id); }
  const bwp_config*      find_bwp(bwp_id_t bwp_id) const { return has_bwp_id(bwp_id) ? &bwp(bwp_id) : nullptr; }
  const bwp_config&      bwp(bwp_id_t bwp_id) const { return cell_ded->bwps[bwp_id].value(); }
  const bwp_config&      init_bwp() const { return cell_ded->bwps[to_bwp_id(0)].value(); }
  const bwp_config_list& bwps() const { return cell_ded->bwps; }

  /// Fetches CORESET configuration based on Coreset-Id.
  const coreset_configuration* find_coreset(coreset_id cs_id) const
  {
    return cell_ded->coresets.contains(cs_id) ? &cell_ded->coresets[cs_id].value() : nullptr;
  }
  const coreset_configuration& coreset(coreset_id cs_id) const { return cell_ded->coresets[cs_id].value(); }

  /// Fetches SearchSpace configuration based on SearchSpace-Id.
  /// Note: The ID space of SearchSpaceIds is common across all the BWPs of a Serving Cell.
  const search_space_info* find_search_space(search_space_id ss_id) const
  {
    return search_spaces.contains(ss_id) ? &search_spaces[ss_id] : nullptr;
  }
  const search_space_info& search_space(search_space_id ss_id) const { return search_spaces[ss_id]; }

  /// Get the number of active DL ports for this UE.
  unsigned get_nof_dl_ports() const { return nof_dl_ports; }

  /// Determines whether DL allocations are possible in the provided slot.
  bool is_dl_enabled(slot_point dl_slot) const
  {
    if (not cell_cfg_common.is_dl_enabled(dl_slot)) {
      return false;
    }
    if (meas_gap_cfg.has_value()) {
      if (is_inside_meas_gap(meas_gap_cfg.value(), dl_slot)) {
        return false;
      }
    }
    return true;
  }

  /// Determines whether UL allocations are possible in the provided slot.
  bool is_ul_enabled(slot_point ul_slot) const;

  /// Get CSI-MeasConfig for the UE.
  const csi_meas_config* csi_meas_cfg() const
  {
    return cell_ded->csi_meas_cfg.has_value() ? &*cell_ded->csi_meas_cfg.value() : nullptr;
  }

  /// PDSCH parameters common to all BWPs.
  const pdsch_serving_cell_config* pdsch_serving_cell_cfg() const
  {
    return cell_ded->pdsch_serv_cell_cfg.has_value() ? &*cell_ded->pdsch_serv_cell_cfg.value() : nullptr;
  }

  /// PUSCH parameters common to all BWPs.
  const pusch_serving_cell_config* pusch_serving_cell_cfg() const
  {
    return cell_ded->pusch_serv_cell_cfg.has_value() ? &*cell_ded->pusch_serv_cell_cfg.value() : nullptr;
  }

  time_alignment_group::id_t tag_id() const { return cell_ded->tag_id; }

  /// Determines the use of transform precoding for DCI Format 0_1 for C-RNTI.
  bool use_pusch_transform_precoding_dci_0_1() const
  {
    // If the UE is not configured with the higher layer parameter transformPrecoder in pusch-Config, determine the
    // transform precoder use according to parameter msg3-transformPrecoder.
    if (not init_bwp().ul_ded.has_value() or not init_bwp().ul_ded->pusch_cfg.has_value() or
        init_bwp().ul_ded->pusch_cfg->trans_precoder == pusch_config::transform_precoder::not_set) {
      return cell_cfg_common.use_msg3_transform_precoder();
    }

    // Otherwise, determine the use of transform pecoding according to transformPrecoder in pusch-Config.
    return init_bwp().ul_ded->pusch_cfg->trans_precoder == pusch_config::transform_precoder::enabled;
  }

  /// \brief Gets the PUSCH transmit scheme codebook configuration.
  ///
  /// The codebook subset selection procedure is described in TS 38.214 Section 6.1.1.1.
  ///
  /// \remark An assertion is triggered if the transmission scheme is not present or not set to codebook.
  const tx_scheme_codebook& get_pusch_codebook_config() const
  {
    srsran_assert(init_bwp().ul_ded.has_value(), "Missing dedicated UL configuration.");
    const auto& ul_ded = init_bwp().ul_ded.value();
    srsran_assert(ul_ded.pusch_cfg.has_value(), "Missing dedicated PUSCH configuration.");
    srsran_assert(ul_ded.pusch_cfg.value().tx_cfg.has_value(), "Missing transmit configuration.");
    srsran_assert(std::holds_alternative<tx_scheme_codebook>(ul_ded.pusch_cfg.value().tx_cfg.value()),
                  "PUSCH Transmission scheme must be set to codebook.");
    return std::get<tx_scheme_codebook>(ul_ded.pusch_cfg.value().tx_cfg.value());
  }

  /// \brief Gets the SRS transmit number of ports.
  /// \remark An assertion is triggered if no SRS resource is present.
  const auto& get_srs_nof_ports() const
  {
    srsran_assert(init_bwp().ul_ded.has_value(), "Missing dedicated UL configuration.");
    srsran_assert(init_bwp().ul_ded->srs_cfg.has_value(), "Missing dedicated SRS configuration.");
    srsran_assert(init_bwp().ul_ded->srs_cfg.value().srs_res_list.size() == 1, "SRS resource list size must be one.");
    return init_bwp().ul_ded->srs_cfg.value().srs_res_list.front().nof_ports;
  }

private:
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_downlink_common& bwp_dl_common);
  void configure_bwp_common_cfg(bwp_id_t bwpid, const bwp_uplink_common& bwp_ul_common);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_downlink_dedicated& bwp_dl_ded);
  void configure_bwp_ded_cfg(bwp_id_t bwpid, const bwp_uplink_dedicated& bwp_ul_ded);

  /// Dedicated cell configuration.
  ue_cell_config_ptr             cell_ded;
  std::optional<meas_gap_config> meas_gap_cfg;
  std::optional<drx_config>      drx_cfg;
  bool                           multi_cells_configured;

  /// This array maps SearchSpace-Ids (the array indexes) to SearchSpace parameters (the array values).
  slotted_array<search_space_info, MAX_NOF_SEARCH_SPACES> search_spaces;

  /// Number of DL ports for this UE.
  unsigned nof_dl_ports = 1;

  // Parameters relative to the RRM of a UE.
  sched_ue_resource_alloc_config ue_res_alloc_cfg;
};

/// Structure that holds all the information related with the configuration of a UE in the scheduler.
///
/// The information held by this structure includes configured cells and logical channels by the upper layers based on
/// the UE RRC configuration, and other RRM-related parameters that can be controlled by the app or via O1/E2.
class ue_configuration
{
public:
  ue_configuration(du_ue_index_t                         ue_index,
                   rnti_t                                crnti_,
                   const cell_common_configuration_list& common_cells,
                   const ue_creation_params&             params);
  ue_configuration(const ue_configuration& other);

  const du_ue_index_t ue_index;
  const rnti_t        crnti;

  /// Retrieve parameters set at the application level for the UEs instantiated in the gNB.
  const scheduler_ue_expert_config& expert_cfg() const { return pcell_common_cfg().expert_cfg.ue; }

  /// Checks whether the cell with the provided index is configured for the UE.
  bool contains(du_cell_index_t cell_index) const { return du_cells.contains(cell_index); }

  /// Number of configured cells in the UE cell group config.
  size_t nof_cells() const { return ue_cell_to_du_cell_index.size(); }

  /// Get the configuration of a cell that is common to all UEs.
  const cell_configuration& common_cell_cfg(du_cell_index_t cell_index) const
  {
    srsran_assert(du_cells.contains(cell_index), "Invalid cell_index={}", fmt::underlying(cell_index));
    return du_cells[cell_index]->cell_cfg_common;
  }
  const cell_configuration& pcell_common_cfg() const
  {
    return common_cell_cfg(ue_cell_to_du_cell_index[to_ue_cell_index(0)]);
  }

  /// Get the configuration of a cell that is dedicated to the UE.
  const ue_cell_configuration& ue_cell_cfg(du_cell_index_t cell_index) const
  {
    srsran_assert(du_cells.contains(cell_index), "Invalid cell_index={}", fmt::underlying(cell_index));
    return *du_cells[cell_index];
  }
  const ue_cell_configuration& ue_cell_cfg(ue_cell_index_t ue_cell_index) const
  {
    srsran_assert(
        ue_cell_index < ue_cell_to_du_cell_index.size(), "Invalid cell_index={}", fmt::underlying(ue_cell_index));
    return ue_cell_cfg(ue_cell_to_du_cell_index[ue_cell_index]);
  }
  const ue_cell_configuration& pcell_cfg() const { return ue_cell_cfg(to_ue_cell_index(0)); }

  /// Get logical channels configured for the UE.
  logical_channel_config_list_ptr logical_channels() const { return lc_list; }

  /// Update the UE dedicated configuration given a configuration request coming from outside the scheduler.
  void update(const cell_common_configuration_list& common_cells, const ue_reconfig_params& reconf_params);

  /// Returns whether UE configuration is considered complete or not for scheduling the UE as a non-fallback UE.
  /// \remark UE can be scheduled in fallback scheduler even if UE does not have a complete configuration.
  bool is_ue_cfg_complete() const;

  /// Get DRX configuration for the UE cell group.
  const std::optional<drx_config>& drx_cfg() const { return ue_drx_cfg; }

private:
  // List of configured logical channels
  logical_channel_config_list_ptr lc_list;

  // List of cells configured for a UE.
  slotted_id_vector<du_cell_index_t, std::unique_ptr<ue_cell_configuration>> du_cells;

  // Mapping of UE Cell indexes to DU cell indexes.
  std::vector<du_cell_index_t> ue_cell_to_du_cell_index;

  // DRX config for the UE cell group.
  std::optional<drx_config> ue_drx_cfg;
};

} // namespace srsran
