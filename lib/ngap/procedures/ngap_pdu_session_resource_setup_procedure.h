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
#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_setup_procedure
{
public:
  ngap_pdu_session_resource_setup_procedure(const cu_cp_pdu_session_resource_setup_request& request_,
                                            byte_buffer                                     nas_pdu_,
                                            const ngap_ue_ids&                              ue_ids_,
                                            ngap_rrc_ue_pdu_notifier&                       rrc_ue_pdu_notifier_,
                                            ngap_du_processor_control_notifier&             du_processor_ctrl_notif_,
                                            ngap_message_notifier&                          amf_notif_,
                                            ngap_ue_logger&                                 logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Setup Procedure"; }

private:
  // results senders
  void send_pdu_session_resource_setup_response();

  cu_cp_pdu_session_resource_setup_request  request;
  byte_buffer                               nas_pdu;
  const ngap_ue_ids                         ue_ids;
  ngap_rrc_ue_pdu_notifier&                 rrc_ue_pdu_notifier;
  cu_cp_pdu_session_resource_setup_response response;
  ngap_du_processor_control_notifier&       du_processor_ctrl_notifier;
  ngap_message_notifier&                    amf_notifier;
  ngap_ue_logger&                           logger;
};

} // namespace srs_cu_cp
} // namespace srsran
