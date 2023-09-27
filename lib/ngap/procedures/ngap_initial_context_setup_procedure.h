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

#include "../ngap_asn1_utils.h"
#include "../ngap_context.h"
#include "../ue_context/ngap_ue_context.h"
#include "srsran/cu_cp/ue_manager.h" // for ngap_ue
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_initial_context_setup_procedure
{
public:
  ngap_initial_context_setup_procedure(const asn1::ngap::init_context_setup_request_s& request_,
                                       ngap_context_t&                                 context_,
                                       ngap_ue_context&                                ue_ctxt_,
                                       ngap_rrc_ue_control_notifier&                   rrc_ue_ctrl_notifier_,
                                       ngap_rrc_ue_pdu_notifier&                       rrc_ue_pdu_notifier_,
                                       ngap_du_processor_control_notifier&             du_processor_ctrl_notifier_,
                                       ngap_message_notifier&                          amf_notifier_,
                                       srslog::basic_logger&                           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Initial Context Setup Procedure"; }

private:
  // results senders
  void send_initial_context_setup_response(const ngap_initial_context_response_message& msg,
                                           const amf_ue_id_t&                           amf_ue_id,
                                           const ran_ue_id_t&                           ran_ue_id);
  void send_initial_context_setup_failure(const ngap_initial_context_failure_message& msg,
                                          const amf_ue_id_t&                          amf_ue_id,
                                          const ran_ue_id_t&                          ran_ue_id);

  const asn1::ngap::init_context_setup_request_s request;
  ngap_context_t&                                context;
  ngap_ue_context&                               ue_ctxt;
  ngap_rrc_ue_control_notifier&                  rrc_ue_ctrl_notifier;
  ngap_rrc_ue_pdu_notifier&                      rrc_ue_pdu_notifier;
  ngap_du_processor_control_notifier&            du_processor_ctrl_notifier;
  ngap_message_notifier&                         amf_notifier;
  srslog::basic_logger&                          logger;

  cu_cp_pdu_session_resource_setup_request  pdu_session_setup_request;
  cu_cp_pdu_session_resource_setup_response pdu_session_response;

  // (sub-)routine results
  ngap_initial_context_failure_message  fail_msg;
  cu_cp_ngap_ue_context_release_command rel_cmd;
  ngap_initial_context_response_message resp_msg;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran
