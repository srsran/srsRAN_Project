/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                                       ngap_cu_cp_notifier&                   cu_cp_notifier_,
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

  ngap_init_context_setup_request request;
  const ngap_ue_ids&              ue_ids;
  ngap_rrc_ue_control_notifier&   rrc_ue_ctrl_notifier;
  ngap_rrc_ue_pdu_notifier&       rrc_ue_pdu_notifier;
  ngap_cu_cp_notifier&            cu_cp_notifier;
  ngap_message_notifier&          amf_notifier;
  ngap_ue_logger&                 logger;

  cu_cp_pdu_session_resource_setup_response pdu_session_response;

  // (sub-)routine results
  ngap_init_context_setup_failure  fail_msg;
  cu_cp_ue_context_release_command rel_cmd;
  ngap_init_context_setup_response resp_msg;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran
