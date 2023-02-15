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

#include "../f1ap_cu_impl.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "common/f1ap_asn1_utils.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ue_context_release_procedure
{
public:
  ue_context_release_procedure(f1ap_ue_context_list&                  ue_ctx_list_,
                               const f1ap_ue_context_release_command& cmd_,
                               f1ap_message_notifier&                 f1ap_notif_,
                               srslog::basic_logger&                  logger_);

  void operator()(coro_context<async_task<ue_index_t>>& ctx);

private:
  /// Send F1 UE Context Release Command to DU.
  void send_ue_context_release_command();

  /// Creates procedure result to send back to procedure caller.
  ue_index_t create_ue_context_release_complete(const asn1::f1ap::ue_context_release_complete_s& msg);

  f1ap_ue_context_list&                ue_ctxt_list;
  f1ap_ue_context&                     ue_ctxt;
  asn1::f1ap::ue_context_release_cmd_s command;
  f1ap_message_notifier&               f1ap_notifier;
  srslog::basic_logger&                logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_release_complete_s> transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb