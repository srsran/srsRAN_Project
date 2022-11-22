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

#include "pdu_session.h"
#include "srsgnb/asn1/e1ap/e1ap.h"

namespace srsgnb {

namespace srs_cu_up {

// Result when creating a new QoS flow
struct qos_flow_setup_result {
  bool                success;
  uint16_t            qos_flow_id;
  asn1::e1ap::cause_c cause; // Cause if setup was unsuccessful.
};

// Result when creating a new DRB containing QoS flow results
struct drb_setup_result {
  bool                               success;
  uint16_t                           drb_id;
  asn1::e1ap::cause_c                cause; // Cause if setup was unsuccessful.
  std::vector<qos_flow_setup_result> qos_flow_results;
};

// Final result when creating a PDU session with all DRBs and QoS flow results.
struct pdu_session_setup_result {
  bool                          success;        // True if PDU session could be set up.
  uint8_t                       pdu_session_id; // The PDU session ID.
  asn1::e1ap::cause_c           cause;          // Cause if setup was unsuccessful.
  std::vector<drb_setup_result> drb_setup_results;
};

class pdu_session_manager_ctrl
{
public:
  virtual ~pdu_session_manager_ctrl() = default;

  virtual pdu_session_setup_result setup_pdu_session(const asn1::e1ap::pdu_session_res_to_setup_item_s& session) = 0;
  virtual void                     remove_pdu_session(uint8_t pdu_session_id)                                    = 0;
  virtual size_t                   get_nof_pdu_sessions()                                                        = 0;
};

} // namespace srs_cu_up

} // namespace srsgnb
