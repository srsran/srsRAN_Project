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

#include "../e1ap_cu_cp_impl.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "common/e1ap_asn1_utils.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class bearer_context_release_procedure
{
public:
  bearer_context_release_procedure(const e1ap_configuration& e1ap_cfg_,
                                   const e1ap_message&       command_,
                                   ue_index_t                ue_index_,
                                   e1ap_ue_context_list&     ue_ctxt_list_,
                                   e1ap_message_notifier&    e1ap_notif_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Bearer Context Release Procedure"; }

private:
  /// Send Bearer Context Release Command to CU-UP.
  void send_bearer_context_release_command();

  /// Handles procedure result and returns back to procedure caller.
  void handle_bearer_context_release_complete();

  const e1ap_configuration e1ap_cfg;
  const e1ap_message       command;
  ue_index_t               ue_index;
  e1ap_ue_context_list&    ue_ctxt_list;
  e1ap_message_notifier&   e1ap_notifier;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_release_complete_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsran
