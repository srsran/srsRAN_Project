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

#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_init_context_setup.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_initial_context_setup_procedure
{
public:
  ngap_initial_context_setup_procedure(const ngap_init_context_setup_request& request_,
                                       const ngap_ue_ids&                     ue_ids_,
                                       ngap_rrc_ue_control_notifier&          rrc_ue_ctrl_notifier_,
                                       ngap_rrc_ue_pdu_notifier&              rrc_ue_pdu_notifier_,
                                       ngap_du_processor_control_notifier&    du_processor_ctrl_notifier_,
                                       ngap_message_notifier&                 amf_notifier_,
                                       ngap_ue_logger&                        logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Initial Context Setup Procedure"; }

private:
  // results senders
  void send_initial_context_setup_response(const ngap_init_context_setup_response& msg,
                                           const amf_ue_id_t&                      amf_ue_id,
                                           const ran_ue_id_t&                      ran_ue_id);
  void send_initial_context_setup_failure(const ngap_init_context_setup_failure& msg,
                                          const amf_ue_id_t&                     amf_ue_id,
                                          const ran_ue_id_t&                     ran_ue_id);

  ngap_init_context_setup_request     request;
  const ngap_ue_ids&                  ue_ids;
  ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier;
  ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier;
  ngap_du_processor_control_notifier& du_processor_ctrl_notifier;
  ngap_message_notifier&              amf_notifier;
  ngap_ue_logger&                     logger;

  cu_cp_pdu_session_resource_setup_response pdu_session_response;

  // (sub-)routine results
  ngap_init_context_setup_failure       fail_msg;
  cu_cp_ngap_ue_context_release_command rel_cmd;
  ngap_init_context_setup_response      resp_msg;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran
