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
#include "srsgnb/cu_cp/ue_manager.h" // for ngap_ue
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ngap_pdu_session_resource_setup_procedure
{
public:
  ngap_pdu_session_resource_setup_procedure(ngap_ue&                                  ue_,
                                            cu_cp_pdu_session_resource_setup_request& request_,
                                            ngap_du_processor_control_notifier&       du_processor_ctrl_notif_,
                                            ngap_message_notifier&                    amf_notif_,
                                            srslog::basic_logger&                     logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // results senders
  void send_pdu_session_resource_setup_response();

  ngap_ue&                                  ue;
  cu_cp_pdu_session_resource_setup_request& request;
  cu_cp_pdu_session_resource_setup_response response;
  ngap_du_processor_control_notifier&       du_processor_ctrl_notifier;
  ngap_message_notifier&                    amf_notifier;
  srslog::basic_logger&                     logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb
