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

#include "../f1ap_du_connection_handler.h"
#include "f1ap_du_event_manager.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// This coroutines handles the F1 removal procedure as per TS 38.473, 8.2.8.
class f1ap_du_removal_procedure
{
public:
  f1ap_du_removal_procedure(f1ap_du_connection_handler& du_conn_handler,
                            f1ap_message_notifier&      tx_pdu_notifier,
                            f1ap_event_manager&         ev_mng);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "F1 Removal"; }

  void send_f1_removal_request();

  void handle_f1_removal_response();

  f1ap_du_connection_handler& du_conn_handler;
  f1ap_message_notifier&      cu_notifier;
  f1ap_event_manager&         ev_mng;
  srslog::basic_logger&       logger;

  f1ap_transaction transaction;
};

} // namespace srs_du
} // namespace srsran