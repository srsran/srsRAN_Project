/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

namespace srs_cu_cp {

/// \brief List of all supported FiveQIs and their correspondig PDCP/SDAP config
struct drb_manager_cfg {};

/// Object to manage DRB configs, allocation, creation and deletion for UE.
class drb_manager
{
public:
  virtual ~drb_manager() = default;

  /// \brief Return list of DRBs to be added based on the PDU session resource setup message.
  virtual std::vector<drb_id_t> calculate_drb_to_add_list(const cu_cp_pdu_session_resource_setup_message& pdu) = 0;

  /// \brief Return PDPC config for a given DRB.
  virtual cu_cp_pdcp_config get_pdcp_config(const drb_id_t drb_id) = 0;

  /// \brief Return SDAP config for a given DRB.
  virtual cu_cp_sdap_config get_sdap_config(const drb_id_t drb_id) = 0;

  /// \brief Return list of QoS flow IDs mapped on a given DRB.
  virtual std::vector<uint8_t> get_mapped_qos_flows(const drb_id_t drb_id) = 0;

  /// \brief Return PDU session ID for a given DRB.
  virtual uint16_t get_pdu_session_id(const drb_id_t drb_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
