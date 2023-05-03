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
#include "srsran/cu_cp/ue_manager.h" // for ngap_ue
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_initial_context_setup_procedure
{
public:
  ngap_initial_context_setup_procedure(const ue_index_t                                ue_index_,
                                       const asn1::ngap::init_context_setup_request_s& request_,
                                       ngap_ue_manager&                                ue_manager_,
                                       ngap_message_notifier&                          amf_notif_,
                                       srslog::basic_logger&                           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  // results senders
  void send_initial_context_setup_response(const ngap_initial_context_response_message& msg,
                                           const amf_ue_id_t&                           amf_ue_id,
                                           const ran_ue_id_t&                           ran_ue_id);
  void send_initial_context_setup_failure(const ngap_initial_context_failure_message& msg,
                                          const amf_ue_id_t&                          amf_ue_id,
                                          const ran_ue_id_t&                          ran_ue_id);

  const ue_index_t                               ue_index;
  const asn1::ngap::init_context_setup_request_s request;
  cu_cp_pdu_session_resource_setup_request       pdu_session_setup_request;
  cu_cp_pdu_session_resource_setup_response      pdu_session_response;
  ngap_ue_manager&                               ue_manager;
  ngap_message_notifier&                         amf_notifier;
  srslog::basic_logger&                          logger;

  ngap_ue* ue = nullptr;

  bool success = false;
};

} // namespace srs_cu_cp
} // namespace srsran
