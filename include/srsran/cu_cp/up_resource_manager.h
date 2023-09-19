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

/// \brief Update for a PDU session.
struct up_pdu_session_context_update {
  up_pdu_session_context_update(pdu_session_id_t id_) : id(id_){};
  pdu_session_id_t                   id;
  std::map<drb_id_t, up_drb_context> drb_to_add;
  std::vector<drb_id_t>              drb_to_remove;
};

// Struct that contains all fields required to update the UP config based on an incoming
// PDU sessions resource setup request over NGAP. This config is then used to:
// * Initiate or modifiy the CU-UPs bearer context over E1AP
// * Modify the DU's UE context over F1AP
// * Modify the CU-UPs bearer context over E1AP (update TEIDs, etc)
// * Modify the UE's configuration over RRC signaling
// For PDU sessions to be setup the entire session context is included in the struct as this has been allocated by UP
// resource manager. For removal of PDU sessions or DRBs only the respective identifiers are included.
struct up_config_update {
  bool initial_context_creation = true; // True if this is the first PDU session to be created.
  bool context_removal_required =
      false; // Set to true if all UP resources will be removed and the context can be deleted.
  std::map<pdu_session_id_t, up_pdu_session_context_update>
      pdu_sessions_to_setup_list; // List of PDU sessions to be added.
  std::map<pdu_session_id_t, up_pdu_session_context_update>
                                pdu_sessions_to_modify_list;        // List of PDU sessions to be modified.
  std::vector<pdu_session_id_t> pdu_sessions_to_remove_list;        // List of PDU sessions to be removed.
  std::vector<pdu_session_id_t> pdu_sessions_failed_to_modify_list; // List of PDU sessions that failed to be modified.
  std::vector<drb_id_t>         drb_to_remove_list;                 // List of DRBs to be removed.
};

// Response given back to the UP resource manager containing the full context
// that could be setup.
struct up_config_update_result {
  std::vector<up_pdu_session_context_update> pdu_sessions_added_list;    // List of sessions that have been added.
  std::vector<up_pdu_session_context_update> pdu_sessions_modified_list; // List of sessions that have been modified.
  std::vector<pdu_session_id_t>              pdu_sessions_removed_list;  // List of sessions that have been removed.
};

/// \brief Free function to convert existing UP context into config update (useful to setup new UEs).
up_config_update to_config_update(const up_context& old_context);

/// Object to manage user-plane (UP) resources including configs, PDU session, DRB and QoS flow
/// allocation/creation/deletion
class up_resource_manager
{
public:
  virtual ~up_resource_manager() = default;

  /// \brief Checks whether incoming PDU session resource setup items are valid.
  virtual bool
  validate_request(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items) = 0;

  /// \brief Checks whether an incoming PDU session resource modify request is valid.
  virtual bool validate_request(const cu_cp_pdu_session_resource_modify_request& pdu) = 0;

  /// \brief Checks whether an incoming PDU session resource release command is valid.
  virtual bool validate_request(const cu_cp_pdu_session_resource_release_command& pdu) = 0;

  /// \brief Returns updated UP config based on the PDU session resource setup items.
  virtual up_config_update
  calculate_update(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items) = 0;

  /// \brief Returns updated UP config based on the PDU session resource modification request.
  virtual up_config_update calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu) = 0;

  /// \brief Returns updated UP config based on the PDU session resource release command.
  virtual up_config_update calculate_update(const cu_cp_pdu_session_resource_release_command& pdu) = 0;

  /// \brief Apply and merge the config with the currently stored one.
  virtual bool apply_config_update(const up_config_update_result& config) = 0;

  /// \brief Return context for given PDU session ID.
  virtual const up_pdu_session_context& get_pdu_session_context(pdu_session_id_t psi) = 0;

  /// \brief Return context for a given DRB ID.
  virtual const up_drb_context& get_drb_context(drb_id_t drb_id) = 0;

  /// \brief Returns True if PDU session with given ID already exists.
  virtual bool has_pdu_session(pdu_session_id_t pdu_session_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;

  /// \brief Returns the number of PDU sessions of the UE.
  virtual size_t get_nof_pdu_sessions() = 0;

  /// \brief Returns the number of active QoS flows in a particular PDU sessions.
  virtual size_t get_nof_qos_flows(pdu_session_id_t psi) = 0;

  /// \brief Returns the number of *all* active QoS flows across all PDU sessions.
  virtual size_t get_total_nof_qos_flows() = 0;

  /// \brief Return a map of all PDU sessions as const reference.
  virtual const std::map<pdu_session_id_t, up_pdu_session_context>& get_pdu_sessions_map() = 0;

  /// \brief Return vector of ID of all active PDU sessions.
  virtual std::vector<pdu_session_id_t> get_pdu_sessions() = 0;

  /// \brief Return vector of active DRBs.
  virtual std::vector<drb_id_t> get_drbs() = 0;

  /// \brief Return up context for transfer.
  virtual up_context get_up_context() = 0;

  /// \brief Set up context from an already existing context.
  virtual void set_up_context(const up_context& ctx) = 0;
};

/// Creates an instance of an UP resource manager.
std::unique_ptr<up_resource_manager> create_up_resource_manager(const up_resource_manager_cfg& cfg_);

} // namespace srs_cu_cp

} // namespace srsran
