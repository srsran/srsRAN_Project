/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_cp_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/rlc/rlc_config.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief List of all supported 5QIs and their corresponding PDCP/SDAP configs
struct up_resource_manager_cfg {
  std::map<five_qi_t, cu_cp_qos_config> five_qi_config; ///< Configuration for available 5QI.
};

struct up_qos_flow_context {
  qos_flow_id_t                   qfi = qos_flow_id_t::invalid;
  cu_cp_qos_flow_level_qos_params qos_params;
};

struct up_drb_context {
  srsran::drb_id_t                             drb_id         = drb_id_t::invalid;
  pdu_session_id_t                             pdu_session_id = pdu_session_id_t::invalid;
  s_nssai_t                                    s_nssai        = {};
  bool                                         default_drb    = false;
  srsran::rlc_mode                             rlc_mod;
  cu_cp_qos_flow_level_qos_params              qos_params; // DRB QoS params.
  std::map<qos_flow_id_t, up_qos_flow_context> qos_flows;  // QoS flow IDs of all QoS flows mapped to this DRB.
  std::vector<up_transport_layer_info>         ul_up_tnl_info_to_be_setup_list; // Allocated by CU-UP.

  pdcp_config   pdcp_cfg;
  sdap_config_t sdap_cfg;
};

struct up_pdu_session_context {
  up_pdu_session_context(pdu_session_id_t id_) : id(id_){};
  pdu_session_id_t                   id = pdu_session_id_t::invalid;
  std::map<drb_id_t, up_drb_context> drbs;
};

/// \brief This struct holds the UP configuration currently in place.
struct up_context {
  std::map<pdu_session_id_t, up_pdu_session_context> pdu_sessions; // Map of existing PDU sessions.

  /// Hash-maps for quick access.
  std::map<drb_id_t, pdu_session_id_t> drb_map;      // Maps DRB ID to corresponding PDU session.
  std::map<qos_flow_id_t, drb_id_t>    qos_flow_map; // Maps QoS flow to corresponding DRB.
};

} // namespace srs_cu_cp

} // namespace srsran
