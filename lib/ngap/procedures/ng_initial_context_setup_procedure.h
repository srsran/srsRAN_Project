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
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

struct initial_context_failure_message {
  asn1::ngap::cause_c                      cause;
  ngap_pdu_session_res_list                failed_to_setup;
  optional<asn1::ngap::crit_diagnostics_s> crit_diagnostics;
};

struct initial_context_response_message {
  ngap_pdu_session_res_list                succeed_to_setup;
  ngap_pdu_session_res_list                failed_to_setup;
  optional<asn1::ngap::crit_diagnostics_s> crit_diagnostics;
};

class ng_initial_context_setup_procedure
{
public:
  ng_initial_context_setup_procedure(ngc_ue&                                         ue_,
                                     const asn1::ngap::init_context_setup_request_s& request_,
                                     ngc_message_notifier&                           amf_notif_,
                                     srslog::basic_logger&                           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // helpers
  void handle_nas_pdu();

  // results senders
  void send_initial_context_setup_response(const initial_context_response_message& msg);
  void send_initial_context_setup_failure(const initial_context_failure_message& msg);

  ngc_ue&                                        ue;
  const asn1::ngap::init_context_setup_request_s request;
  ngc_message_notifier&                          amf_notifier;
  srslog::basic_logger&                          logger;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsgnb
