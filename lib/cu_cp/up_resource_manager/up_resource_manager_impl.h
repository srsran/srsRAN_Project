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

#include "srsran/cu_cp/up_context.h"
#include <map>

namespace srsran {

namespace srs_cu_cp {

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

/// UP resource manager implementation
class up_resource_manager
{
public:
  up_resource_manager(const up_resource_manager_cfg& cfg);
  ~up_resource_manager() = default;

  bool validate_request(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items) const;
  bool validate_request(const cu_cp_pdu_session_resource_modify_request& pdu) const;
  bool validate_request(const cu_cp_pdu_session_resource_release_command& pdu) const;

  up_config_update
  calculate_update(const slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item>& setup_items);
  up_config_update calculate_update(const cu_cp_pdu_session_resource_modify_request& pdu);
  up_config_update calculate_update(const cu_cp_pdu_session_resource_release_command& pdu);

  bool                          apply_config_update(const up_config_update_result& config);
  const up_pdu_session_context& get_pdu_session_context(pdu_session_id_t psi) const;
  const up_drb_context&         get_drb_context(drb_id_t drb_id) const;
  bool                          has_pdu_session(pdu_session_id_t pdu_session_id) const;
  size_t                        get_nof_drbs() const;
  size_t                        get_nof_pdu_sessions() const;
  size_t                        get_nof_qos_flows(pdu_session_id_t psi) const;
  size_t                        get_total_nof_qos_flows() const;
  std::vector<pdu_session_id_t> get_pdu_sessions() const;
  std::vector<drb_id_t>         get_drbs() const;

  up_context get_up_context() const;
  void       set_up_context(const up_context& ctx);

  const std::map<pdu_session_id_t, up_pdu_session_context>& get_pdu_sessions_map() const;

private:
  bool valid_5qi(const five_qi_t five_qi);

  up_resource_manager_cfg cfg;

  up_context context; // The currently active state.

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp

} // namespace srsran
