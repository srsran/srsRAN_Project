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

class e1_bearer_context_setup_procedure
{
public:
  e1_bearer_context_setup_procedure(const asn1::e1ap::bearer_context_setup_request_s& request_,
                                    e1_message_notifier&                              e1_notif_,
                                    e1_event_manager&                                 ev_mng_,
                                    srslog::basic_logger&                             logger_);

  void operator()(coro_context<async_task<e1ap_bearer_context_setup_response_message>>& ctx);

private:
  /// Send E1 Bearer Context Setup Request to DU.
  void send_bearer_context_setup_request();

  /// Creates procedure result to send back to procedure caller.
  e1ap_bearer_context_setup_response_message create_bearer_context_setup_result();

  const asn1::e1ap::bearer_context_setup_request_s request;
  e1_message_notifier&                             e1_notifier;
  e1_event_manager&                                ev_mng;
  srslog::basic_logger&                            logger;

  e1_event_manager::e1_bearer_context_setup_outcome_t e1_bearer_ctxt_setup_outcome;
};

} // namespace srs_cu_cp
} // namespace srsgnb