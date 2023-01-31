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

class ngap_initial_context_setup_procedure
{
public:
  ngap_initial_context_setup_procedure(const ue_index_t                                ue_index_,
                                       const asn1::ngap::init_context_setup_request_s& request_,
                                       ngc_ue_manager&                                 ue_manager_,
                                       ngc_message_notifier&                           amf_notif_,
                                       srslog::basic_logger&                           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // results senders
  void send_initial_context_setup_response(const initial_context_response_message& msg,
                                           const amf_ue_id_t&                      amf_ue_id,
                                           const ran_ue_id_t&                      ran_ue_id);
  void send_initial_context_setup_failure(const initial_context_failure_message& msg,
                                          const amf_ue_id_t&                     amf_ue_id,
                                          const ran_ue_id_t&                     ran_ue_id);

  const ue_index_t                               ue_index;
  const asn1::ngap::init_context_setup_request_s request;
  ngc_ue_manager&                                ue_manager;
  ngc_message_notifier&                          amf_notifier;
  srslog::basic_logger&                          logger;

  ngc_ue* ue = nullptr;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsgnb
