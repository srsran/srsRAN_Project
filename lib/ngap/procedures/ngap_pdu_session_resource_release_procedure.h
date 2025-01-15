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

#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_release_procedure
{
public:
  ngap_pdu_session_resource_release_procedure(const cu_cp_pdu_session_resource_release_command& command_,
                                              const ngap_ue_ids&                                ue_ids_,
                                              ngap_cu_cp_notifier&                              cu_cp_notifier_,
                                              ngap_message_notifier&                            amf_notif_,
                                              ngap_ue_logger&                                   logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Release Procedure"; }

private:
  // results senders
  void send_pdu_session_resource_release_response();

  cu_cp_pdu_session_resource_release_command  command;
  const ngap_ue_ids                           ue_ids;
  cu_cp_pdu_session_resource_release_response response;
  ngap_cu_cp_notifier&                        cu_cp_notifier;
  ngap_message_notifier&                      amf_notifier;
  ngap_ue_logger&                             logger;
};

} // namespace srs_cu_cp
} // namespace srsran
