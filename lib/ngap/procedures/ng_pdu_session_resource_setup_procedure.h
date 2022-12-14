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

#include "../ngap_asn1_utils.h"
#include "ngc_event_manager.h"
#include "srsgnb/cu_cp/ue_manager.h" // for ngc_ue
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ng_pdu_session_resource_setup_procedure
{
public:
  ng_pdu_session_resource_setup_procedure(ngc_ue&                                  ue_,
                                          e1ap_pdu_session_resource_setup_message& msg_,
                                          ngc_e1_control_notifier&                 e1_notif_,
                                          ngc_message_notifier&                    amf_notif_,
                                          srslog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // results senders
  void send_pdu_session_resource_setup_response();

  ngc_ue&                                          ue;
  e1ap_pdu_session_resource_setup_message          msg;
  e1ap_pdu_session_resource_setup_response_message response;
  ngc_e1_control_notifier&                         e1_notifier;
  ngc_message_notifier&                            amf_notifier;
  srslog::basic_logger&                            logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb
