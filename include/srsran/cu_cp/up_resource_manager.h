/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_types.h"
#include "srsran/ran/lcid.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief List of all supported 5QIs and their corresponding PDCP/SDAP configs
struct up_resource_manager_cfg {
  std::map<five_qi_t, cu_cp_qos_config> five_qi_config; ///< Configuration for available 5QI.
};

struct up_drb_context {
  srsran::drb_id_t           drb_id         = drb_id_t::invalid;
  pdu_session_id_t           pdu_session_id = pdu_session_id_t::min;
  s_nssai_t                  s_nssai        = {};
  bool                       default_drb    = false;
  five_qi_t                  five_qi        = five_qi_t::invalid;
  std::vector<qos_flow_id_t> qos_flows; // QoS flow IDs of all QoS flows mapped to this DRB

  pdcp_config   pdcp_cfg;
  sdap_config_t sdap_cfg;
};

struct up_pdu_session_context {
  pdu_session_id_t                   id;
  std::map<drb_id_t, up_drb_context> drbs;
};

struct up_context {
  std::map<pdu_session_id_t, up_pdu_session_context> pdu_sessions; // Map with active flag for existing PDU sessions

  /// Hash-maps for quick access.
  std::map<five_qi_t, drb_id_t>        five_qi_map; // Maps QoS flow characteristics to existing DRBs.
  std::map<drb_id_t, pdu_session_id_t> drb_map;     // Maps ID of currently active DRB to PDU corresponding PDU session.
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
  std::map<pdu_session_id_t, up_pdu_session_context> pdu_sessions_to_setup_list; // List of PDU sessions to be added.
  std::vector<pdu_session_id_t> pdu_sessions_to_remove_list;                     // List of PDU sessions to be removed.s
  std::vector<drb_id_t>         drb_to_remove_list;
};

// Response given back to the UP resource manager containing the full context
// that could be setup.
struct up_config_update_result {
  std::vector<up_pdu_session_context> pdu_sessions_added_list; // List of DRBs that have been added.
};

/// Object to manage user-plane (UP) resources including configs, PDU session, DRB and QoS flow
/// allocation/creation/deletion
class up_resource_manager
{
public:
  virtual ~up_resource_manager() = default;

  /// \brief Returns updated UP config based on the PDU session resource setup message.
  virtual up_config_update calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu) = 0;

  /// \brief Apply and merge the config with the currently stored one.
  virtual bool apply_config_update(const up_config_update_result& config) = 0;

  /// \brief Return context for given PDU session ID.
  virtual up_pdu_session_context get_context(pdu_session_id_t psi) = 0;

  /// \brief Return context for a given DRB ID.
  virtual up_drb_context get_context(drb_id_t drb_id) = 0;

  /// \brief Returns True if PDU session with given ID already exists.
  virtual bool has_pdu_session(pdu_session_id_t pdu_session_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;

  /// \brief Returns the number of PDU sessions of the UE.
  virtual size_t get_nof_pdu_sessions() = 0;

  /// \brief Return vector of ID of all active PDU sessions.
  virtual std::vector<pdu_session_id_t> get_pdu_sessions() = 0;

  /// \brief Return whether a configuration for a 5QI exists.
  virtual bool valid_5qi(five_qi_t five_qi) = 0;
};

/// Creates an instance of an UP resource manager.
std::unique_ptr<up_resource_manager> create_up_resource_manager(const up_resource_manager_cfg& cfg_);

} // namespace srs_cu_cp

} // namespace srsran
