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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/lcid.h"

namespace srsran {

namespace srs_cu_cp {

/// \brief List of all supported FiveQIs and their correspondig PDCP/SDAP config
struct drb_manager_cfg {
  std::map<uint8_t, cu_cp_qos_config> five_qi_config; ///< Configuration for available 5QI.
};

/// Object to manage DRB configs, allocation, creation and deletion for UE.
class drb_manager
{
public:
  virtual ~drb_manager() = default;

  /// \brief Return list of DRBs to be added based on the PDU session resource setup message.
  virtual std::vector<drb_id_t> calculate_drb_to_add_list(const cu_cp_pdu_session_resource_setup_request& pdu) = 0;

  /// \brief Return PDPC config for a given DRB.
  virtual pdcp_config_t get_pdcp_config(const drb_id_t drb_id) = 0;

  /// \brief Return SDAP config for a given DRB.
  virtual sdap_config_t get_sdap_config(const drb_id_t drb_id) = 0;

  /// \brief Return list of QoS flow IDs mapped on a given DRB.
  virtual std::vector<qos_flow_id_t> get_mapped_qos_flows(const drb_id_t drb_id) = 0;

  /// \brief Return list of QoS flow IDs of a given PDU session.
  virtual std::vector<qos_flow_id_t> get_mapped_qos_flows(const pdu_session_id_t pdu_session_id) = 0;

  /// \brief Return PDU session ID for a given DRB.
  virtual pdu_session_id_t get_pdu_session_id(const drb_id_t drb_id) = 0;

  /// \brief Return slice information for a given DRB.
  virtual s_nssai_t get_s_nssai(const drb_id_t drb_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;

  /// \brief Return whether a configuration for a 5QI exists.
  virtual bool valid_5qi(uint8_t five_qi) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
