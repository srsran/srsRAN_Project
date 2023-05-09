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

// using std::map<five_qi_t, drb_id_t> = five_qi_map_t;

struct up_drb_context {
  srsran::drb_id_t           drb_id         = drb_id_t::invalid;
  pdu_session_id_t           pdu_session_id = pdu_session_id_t::min;
  s_nssai_t                  s_nssai        = {};
  bool                       default_drb    = false;
  five_qi_t                  five_qi        = five_qi_t::invalid;
  std::vector<qos_flow_id_t> mapped_qos_flows; // QoS flow IDs of all QoS flows mapped to this DRB

  pdcp_config   pdcp_cfg;
  sdap_config_t sdap_cfg;
};

struct up_pdu_session_context {
  pdu_session_id_t            id;
  std::vector<up_drb_context> drbs;
};

struct up_context {
  std::vector<up_pdu_session_context> pdu_sessions; // Map with active flag for existing PDU sessions

  /// Hash-maps for quick access.
  std::map<five_qi_t, drb_id_t>      five_qi_map; // Maps QoS flow characteristics to existing DRBs
  std::map<drb_id_t, up_drb_context> drb_map;     // Stores existing DRBs
};

// This struct contains all fields required to update the UP config based on an incoming
// PDU sessions resource setup request over NGAP. This config is then used to:
// * Initiate or modifiy the CU-UPs bearer context over E1AP
// * Modify the DU's UE context over F1AP
// * Modify the CU-UPs bearer context over E1AP (update TEIDs, etc)
// * Modify the UE's configuration over RRC signaling
struct up_config_update {
  bool                          initial_context_creation = true;
  std::vector<pdu_session_id_t> pdu_sessions_to_setup_list;  // List of PDU sessions to be added.
  std::vector<pdu_session_id_t> pdu_sessions_to_remove_list; // List of PDU sessions to be removed.

  std::vector<up_drb_context> drb_to_add_list; // list of DRBs to be added.
  std::vector<qos_flow_id_t>  qos_flow_to_setup_list;
};

/// Object to manage user-plane (UP) resources including configs, PDU session, DRB and QoS flow
/// allocation/creation/deletion
class up_resource_manager
{
public:
  virtual ~up_resource_manager() = default;

  /// \brief Returns updated UP config based on the PDU session resource setup message.
  virtual up_config_update calculate_update(const cu_cp_pdu_session_resource_setup_request& pdu) = 0;

  /// @brief Apply and merge the config with the currently stored one.
  virtual bool apply_config_update(const up_config_update& config) = 0;

  /// \brief Return PDPC config for a given DRB.
  virtual pdcp_config get_pdcp_config(drb_id_t drb_id) = 0;

  /// \brief Return SDAP config for a given DRB.
  virtual sdap_config_t get_sdap_config(drb_id_t drb_id) = 0;

  /// \brief Return list of QoS flow IDs mapped on a given DRB.
  virtual std::vector<qos_flow_id_t> get_mapped_qos_flows(drb_id_t drb_id) = 0;

  /// \brief Return list of QoS flow IDs of a given PDU session.
  virtual std::vector<qos_flow_id_t> get_mapped_qos_flows(pdu_session_id_t pdu_session_id) = 0;

  /// \brief Return list of DRB IDs of a given PDU session.
  virtual std::vector<drb_id_t> get_drbs(pdu_session_id_t pdu_session_id) = 0;

  /// \brief Return PDU session ID for a given DRB.
  virtual pdu_session_id_t get_pdu_session_id(drb_id_t drb_id) = 0;

  /// \brief Return slice information for a given DRB.
  virtual s_nssai_t get_s_nssai(drb_id_t drb_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;

  /// \brief Returns the number of PDU sessions of the UE.
  virtual size_t get_nof_pdu_sessions() = 0;

  /// \brief Return all PDU sessions.
  virtual std::vector<pdu_session_id_t> get_pdu_sessions() = 0;

  /// \brief Return whether a configuration for a 5QI exists.
  virtual bool valid_5qi(five_qi_t five_qi) = 0;
};

/// Creates an instance of an UP resource manager.
std::unique_ptr<up_resource_manager> create_up_resource_manager(const up_resource_manager_cfg& cfg_);

} // namespace srs_cu_cp

} // namespace srsran
