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
#include "common/f1ap_asn1_utils.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class ue_context_setup_procedure
{
public:
  ue_context_setup_procedure(const asn1::f1ap::ue_context_setup_request_s& request_,
                             f1ap_ue_context&                              ue_ctx_,
                             f1ap_message_notifier&                        f1ap_notif_,
                             srslog::basic_logger&                         logger_);

  void operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx);

private:
  /// Send F1 UE Context Setup Request to DU.
  void send_ue_context_setup_request();

  /// Creates procedure result to send back to procedure caller.
  f1ap_ue_context_setup_response create_ue_context_setup_result();

  const asn1::f1ap::ue_context_setup_request_s request;
  f1ap_ue_context&                             ue_ctx;
  f1ap_message_notifier&                       f1ap_notifier;
  srslog::basic_logger&                        logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_setup_resp_s, asn1::f1ap::ue_context_setup_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb