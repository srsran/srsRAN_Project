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

#include "../converters/rlc_config_helpers.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/mac/mac_cell_group_config.h"
#include "srsran/ran/logical_channel/lcid.h"
#include "srsran/ran/meas_gap_config.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/ran/qos/qos_parameters.h"
#include "srsran/rlc/rlc_config.h"
#include "srsran/scheduler/config/serving_cell_config.h"

namespace srsran {
namespace srs_du {

/// This struct stores the accumulated CellGroupConfig.
struct cell_group_config {
  mac_cell_group_config                 mcg_cfg;
  physical_cell_group_config            pcg_cfg;
  slotted_vector<cell_config_dedicated> cells;
};

/// Parameters of an SRB of the DU UE context.
struct du_ue_srb_config {
  srb_id_t      srb_id;
  rlc_config    rlc_cfg;
  mac_lc_config mac_cfg;
};

/// Parameters of a DRB of the DU UE context.
struct du_ue_drb_config {
  drb_id_t                      drb_id;
  lcid_t                        lcid;
  pdcp_sn_size                  pdcp_sn_len = pdcp_sn_size::invalid;
  s_nssai_t                     s_nssai;
  qos_flow_level_qos_parameters qos;
  f1u_config                    f1u;
  rlc_config                    rlc_cfg;
  mac_lc_config                 mac_cfg;

  bool operator==(const du_ue_drb_config& other) const
  {
    return drb_id == other.drb_id and pdcp_sn_len == other.pdcp_sn_len and s_nssai == other.s_nssai and
           qos == other.qos and f1u == other.f1u;
  }
};

/// Contention-free random access (CFRA) configuration for a UE.
struct cfra_config {
  unsigned preamble_id;
};

/// Snapshot of the DU resources taken by a UE at a given instant.
struct du_ue_resource_config {
  slotted_id_table<srb_id_t, du_ue_srb_config, MAX_NOF_SRBS> srbs;
  /// Upper layer configuration of UE DRBs
  slotted_id_vector<drb_id_t, du_ue_drb_config> drbs;
  /// CellGroupConfiguration of the RAN resources allocated to a UE.
  cell_group_config cell_group;
  /// measGapConfig chosen for the UE.
  std::optional<meas_gap_config> meas_gap;
  /// Resources allocated for contention-free random access (CFRA).
  std::optional<cfra_config> cfra;
};

} // namespace srs_du
} // namespace srsran
