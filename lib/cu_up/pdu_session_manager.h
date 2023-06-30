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

#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/ran/cause.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/up_transport_layer_info.h"

namespace srsran {

namespace srs_cu_up {

// Result when creating a new QoS flow
struct qos_flow_setup_result {
  bool          success     = false;
  qos_flow_id_t qos_flow_id = qos_flow_id_t::invalid;
  cause_t       cause; // Cause if setup was unsuccessful.
};

// Result when creating a new DRB containing QoS flow results
struct drb_setup_result {
  bool                               success = false;
  drb_id_t                           drb_id  = drb_id_t::invalid;
  cause_t                            cause; // Cause if setup was unsuccessful.
  up_transport_layer_info            gtp_tunnel;
  std::vector<qos_flow_setup_result> qos_flow_results;
};

// Final result when creating a PDU session with all DRBs and QoS flow results.
struct pdu_session_setup_result {
  bool                          success        = false;                     // True if PDU session could be set up.
  pdu_session_id_t              pdu_session_id = pdu_session_id_t::invalid; // The PDU session ID.
  cause_t                       cause;                                      // Cause if setup was unsuccessful.
  up_transport_layer_info       gtp_tunnel;
  std::vector<drb_setup_result> drb_setup_results;
};

// Final result when modifying a PDU session with all DRBs and QoS flow results.
struct pdu_session_modification_result {
  bool                          success        = false;                     // True if PDU session could be set up.
  pdu_session_id_t              pdu_session_id = pdu_session_id_t::invalid; // The PDU session ID.
  cause_t                       cause;                                      // Cause if modification was unsuccessful.
  std::vector<drb_setup_result> drb_setup_results;
  std::vector<drb_setup_result> drb_modification_results;
};

class pdu_session_manager_ctrl
{
public:
  virtual ~pdu_session_manager_ctrl() = default;

  virtual pdu_session_setup_result        setup_pdu_session(const e1ap_pdu_session_res_to_setup_item& session) = 0;
  virtual pdu_session_modification_result modify_pdu_session(const e1ap_pdu_session_res_to_modify_item& session,
                                                             bool new_tnl_info_required)                       = 0;
  virtual void                            remove_pdu_session(pdu_session_id_t pdu_session_id)                  = 0;
  virtual size_t                          get_nof_pdu_sessions()                                               = 0;
};

} // namespace srs_cu_up

} // namespace srsran
