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

#include "../f1c_cu_impl.h"
#include "common/f1ap_asn1_utils.h"
#include "f1c_cu_event_manager.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/cu/f1c_cu.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

class f1_ue_context_modification_procedure
{
public:
  f1_ue_context_modification_procedure(const asn1::f1ap::ue_context_mod_request_s& request_,
                                       f1c_message_notifier&                       f1c_notif_,
                                       f1c_event_manager&                          ev_mng_,
                                       srslog::basic_logger&                       logger_);

  void operator()(coro_context<async_task<f1ap_ue_context_modification_response_message>>& ctx);

private:
  /// Send F1 UE Context Modification Request to DU.
  void send_ue_context_modification_request();

  /// Creates procedure result to send back to procedure caller.
  f1ap_ue_context_modification_response_message create_ue_context_modification_result();

  const asn1::f1ap::ue_context_mod_request_s request;
  f1c_message_notifier&                      f1c_notifier;
  f1c_event_manager&                         ev_mng;
  srslog::basic_logger&                      logger;

  f1c_event_manager::f1_ue_context_modification_outcome_t ue_ctxt_mod_outcome;
};

} // namespace srs_cu_cp
} // namespace srsgnb