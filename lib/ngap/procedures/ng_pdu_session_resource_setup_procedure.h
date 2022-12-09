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

struct pdu_session_resource_response_message {
  ngap_pdu_session_res_list                succeed_to_setup;
  ngap_pdu_session_res_list                failed_to_setup;
  optional<asn1::ngap::crit_diagnostics_s> crit_diagnostics;
};

class ng_pdu_session_resource_setup_procedure
{
public:
  ng_pdu_session_resource_setup_procedure(ngc_ue&                             ue_,
                                          pdu_session_resource_setup_message& msg_,
                                          ngc_e1_control_notifier&            e1_notif_,
                                          ngc_message_notifier&               amf_notif_,
                                          srslog::basic_logger&               logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // results senders
  void send_pdu_session_resource_setup_response(const pdu_session_resource_response_message& resp_msg);

  ngc_ue&                                  ue;
  pdu_session_resource_setup_message       msg;
  asn1::ngap::pdu_session_res_setup_resp_s response;
  ngc_e1_control_notifier&                 e1_notifier;
  ngc_message_notifier&                    amf_notifier;
  srslog::basic_logger&                    logger;
};

} // namespace srs_cu_cp
} // namespace srsgnb
