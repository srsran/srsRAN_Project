/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "logical_channel_list_config.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {

struct sched_ue_config_request;

using coreset_config_ptr      = config_ptr<coreset_configuration>;
using serving_cell_config_ptr = config_ptr<serving_cell_config>;

/// Configuration of a BWP.
struct bwp_config {
  /// UE-specific BWP Identifier
  bwp_id_t bwp_id;
  /// BWP Downlink Common Configuration
  std::optional<bwp_downlink_common> bwp_dl_common;
  /// BWP Downlink Dedicated Configuration
  std::optional<bwp_downlink_dedicated> bwp_dl_ded;
  /// BWP Uplink Common Configuration
  std::optional<bwp_uplink_common> bwp_ul_common;
  /// BWP Uplink Dedicated Configuration
  std::optional<bwp_uplink_dedicated> bwp_ul_ded;
  /// Search Spaces associated with this BWP.
  slotted_id_vector<search_space_id, search_space_configuration> search_spaces;

  bool operator==(const bwp_config& other) const
  {
    return bwp_id == other.bwp_id and bwp_dl_common == other.bwp_dl_common and bwp_dl_ded == other.bwp_dl_ded and
           bwp_ul_common == other.bwp_ul_common and bwp_ul_ded == other.bwp_ul_ded and
           search_spaces == other.search_spaces;
  }
};

using bwp_config_ptr  = config_ptr<bwp_config>;
using bwp_config_list = slotted_id_vector<bwp_id_t, bwp_config_ptr>;

struct ue_cell_config {
  /// DU-specific cell identifier.
  du_cell_index_t cell_index;
  /// Base serving cell configuration without derived parameters.
  serving_cell_config_ptr cfg;
  /// Container that maps Coreset-Ids to CORESET configurations for this BWP.
  /// Note: The ID space of CoresetIds is common among the BWPs of a Serving Cell as per TS 38.331.
  slotted_id_vector<coreset_id, coreset_config_ptr> coresets;
  /// Container that maps searchSpaceIds to searchSpace configurations for this BWP.
  /// Note: The ID space of searchSpaceIds is common among the BWPs of a Serving Cell as per TS 38.331.
  slotted_id_vector<search_space_id, search_space_configuration> search_spaces;
  /// List of BWPs configured in this cell.
  bwp_config_list bwps;
  /// \c uplinkConfig, containing the UL configuration.
  std::optional<config_ptr<uplink_config>> ul_config;
  /// \brief \c pdsch-ServingCellConfig, used to configure UE specific PDSCH parameters that are common across the UE's
  /// BWPs of one serving cell.
  std::optional<config_ptr<pdsch_serving_cell_config>> pdsch_serv_cell_cfg;
  /// \c CSI-MeasConfig.
  std::optional<config_ptr<csi_meas_config>> csi_meas_cfg;
  /// Timing Advance Group ID to which this cell belongs to.
  time_alignment_group::id_t tag_id{0};

  bool operator==(const ue_cell_config& other) const
  {
    return cfg == other.cfg and cell_index == other.cell_index and coresets == other.coresets and
           search_spaces == other.search_spaces and bwps == other.bwps and csi_meas_cfg == other.csi_meas_cfg;
  }
};
using ue_cell_config_ptr = config_ptr<ue_cell_config>;

struct ue_creation_params {
  const sched_ue_config_request&                         cfg_req;
  logical_channel_config_list_ptr                        lc_ch_list;
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cells;
};

struct ue_reconfig_params {
  const sched_ue_config_request&                         cfg_req;
  std::optional<logical_channel_config_list_ptr>         lc_ch_list;
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cells;
};

} // namespace srsran
