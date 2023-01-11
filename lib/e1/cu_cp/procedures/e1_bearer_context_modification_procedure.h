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
#include "e1ap_cu_cp_transaction_manager.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class e1_bearer_context_modification_procedure
{
public:
  e1_bearer_context_modification_procedure(const e1_message&                request_,
                                           e1_message_notifier&             e1_notif_,
                                           e1ap_bearer_transaction_manager& ev_mng_,
                                           srslog::basic_logger&            logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx);

private:
  /// Send E1 Bearer Context Modification Request to CU-UP.
  void send_bearer_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  e1ap_bearer_context_modification_response create_bearer_context_modification_result();

  const e1_message                 request;
  e1_message_notifier&             e1_notifier;
  e1ap_bearer_transaction_manager& ev_mng;
  srslog::basic_logger&            logger;

  protocol_transaction_outcome_observer<asn1::e1ap::bearer_context_mod_resp_s, asn1::e1ap::bearer_context_mod_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb