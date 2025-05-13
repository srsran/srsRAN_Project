/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ngap_validators/ngap_validators.h"
#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_modify_procedure
{
public:
  ngap_pdu_session_resource_modify_procedure(const cu_cp_pdu_session_resource_modify_request&    request_,
                                             const asn1::ngap::pdu_session_res_modify_request_s& asn1_request_,
                                             const ngap_ue_ids&                                  ue_ids_,
                                             ngap_cu_cp_notifier&                                cu_cp_notifier_,
                                             ngap_message_notifier&                              amf_notifier_,
                                             ngap_control_message_handler&                       ngap_ctrl_handler_,
                                             ngap_ue_logger&                                     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Modify Procedure"; }

private:
  void combine_pdu_session_resource_modify_response();

  // Result senders.
  bool send_pdu_session_resource_modify_response();

  const cu_cp_pdu_session_resource_modify_request    request;
  const asn1::ngap::pdu_session_res_modify_request_s asn1_request;
  const ngap_ue_ids                                  ue_ids;
  cu_cp_pdu_session_resource_modify_response         response;
  ngap_cu_cp_notifier&                               cu_cp_notifier;
  ngap_message_notifier&                             amf_notifier;
  ngap_control_message_handler&                      ngap_ctrl_handler;
  ngap_ue_logger&                                    logger;

  cu_cp_ue_context_release_request ue_context_release_request;

  // Procedure outcomes.
  pdu_session_resource_modify_validation_outcome verification_outcome;
};

} // namespace srs_cu_cp
} // namespace srsran
