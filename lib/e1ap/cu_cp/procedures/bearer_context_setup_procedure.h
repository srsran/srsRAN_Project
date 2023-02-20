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

#include "../e1ap_cu_cp_impl.h"
#include "../ue_context/e1ap_cu_cp_ue_context.h"
#include "common/e1ap_asn1_utils.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class bearer_context_setup_procedure
{
public:
  bearer_context_setup_procedure(const e1ap_message&    request_,
                                 e1ap_ue_context&       ue_ctxt_,
                                 e1ap_message_notifier& e1ap_notif_,
                                 srslog::basic_logger&  logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx);

private:
  /// Send Bearer Context Setup Request to DU.
  void send_bearer_context_setup_request();

  /// Creates procedure result to send back to procedure caller.
  e1ap_bearer_context_setup_response create_bearer_context_setup_result();

  const e1ap_message     request;
  e1ap_ue_context&       ue_ctxt;
  e1ap_message_notifier& e1ap_notifier;
  srslog::basic_logger&  logger;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_setup_resp_s,
                                        asn1::e1ap::bearer_context_setup_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb