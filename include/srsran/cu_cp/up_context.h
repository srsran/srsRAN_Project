/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  uint8_t                               max_nof_drbs_per_ue;
};

struct up_qos_flow_context {
  qos_flow_id_t                 qfi = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_params;
};

struct up_drb_context {
  srsran::drb_id_t                             drb_id         = drb_id_t::invalid;
  pdu_session_id_t                             pdu_session_id = pdu_session_id_t::invalid;
  s_nssai_t                                    s_nssai        = {};
  bool                                         default_drb    = false;
  srsran::rlc_mode                             rlc_mod;
  qos_flow_level_qos_parameters                qos_params; // DRB QoS params.
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
