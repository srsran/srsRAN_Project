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

#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

namespace srs_cu_cp {

/// @brief List of all supported FiveQIs and their correspondig PDCP/SDAP config
struct drb_manager_cfg {};

/// Object to manage DRB configs, allocation, creation and deletion for UE.
class drb_manager
{
public:
  virtual ~drb_manager() = default;

  /// \brief Return list of DRBs to be added based on the PDU session resource setup message.
  virtual std::vector<drb_id_t> calculate_drb_to_add_list(const cu_cp_pdu_session_resource_setup_message& pdu) = 0;

  /// \brief Return PDPC config for a given DRB.
  virtual asn1::rrc_nr::pdcp_cfg_s get_pdcp_config(const drb_id_t drb_id) = 0;

  /// \brief Return SDAP config for a given DRB.
  virtual asn1::rrc_nr::sdap_cfg_s get_sdap_config(const drb_id_t drb_id) = 0;

  /// \brief Return number of DRBs.
  virtual size_t get_nof_drbs() = 0;
};

} // namespace srs_cu_cp

} // namespace srsgnb
