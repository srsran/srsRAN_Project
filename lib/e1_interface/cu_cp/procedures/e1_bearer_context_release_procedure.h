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

#include "../e1_cu_cp_impl.h"
#include "common/e1ap_asn1_utils.h"
#include "e1_cu_cp_event_manager.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class e1_bearer_context_release_procedure
{
public:
  e1_bearer_context_release_procedure(const e1ap_bearer_context_release_command_message& command_,
                                      e1_message_notifier&                               e1_notif_,
                                      e1_event_manager&                                  ev_mng_,
                                      srslog::basic_logger&                              logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_release_complete_message>>& ctx);

private:
  /// Send E1 Bearer Context Release Command to CU-UP.
  void send_bearer_context_release_command();

  /// Creates procedure result to send back to procedure caller.
  e1ap_bearer_context_release_complete_message create_bearer_context_release_complete();

  const e1ap_bearer_context_release_command_message command;
  e1_message_notifier&                              e1_notifier;
  e1_event_manager&                                 ev_mng;
  srslog::basic_logger&                             logger;

  e1_event_manager::e1_bearer_context_release_outcome_t e1_bearer_ctxt_rel_outcome = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb