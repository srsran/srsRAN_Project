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

#include "../../common/f1ap_asn1_utils.h"
#include "../f1ap_cu_impl.h"
#include "../ue_context/f1ap_cu_ue_context.h"
#include "cu_cp/ue_context/f1ap_cu_ue_transaction_manager.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class f1_ue_context_modification_procedure
{
public:
  f1_ue_context_modification_procedure(const cu_cp_ue_context_modification_request& request_,
                                       f1ap_ue_context&                             ue_ctx_,
                                       f1c_message_notifier&                        f1c_notif_,
                                       srslog::basic_logger&                        logger_);

  void operator()(coro_context<async_task<cu_cp_ue_context_modification_response>>& ctx);

private:
  /// Send F1 UE Context Modification Request to DU.
  void send_ue_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  cu_cp_ue_context_modification_response create_ue_context_modification_result();

  const cu_cp_ue_context_modification_request request;
  f1ap_ue_context&                            ue_ctx;
  f1c_message_notifier&                       f1c_notifier;
  srslog::basic_logger&                       logger;

  protocol_transaction_outcome_observer<asn1::f1ap::ue_context_mod_resp_s, asn1::f1ap::ue_context_mod_fail_s>
      transaction_sink;
};

} // namespace srs_cu_cp
} // namespace srsgnb