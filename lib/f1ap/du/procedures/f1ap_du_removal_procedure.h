/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../f1ap_du_connection_handler.h"
#include "f1ap_du_event_manager.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

struct f1ap_du_context;

/// This coroutines handles the F1 removal procedure as per TS 38.473, 8.2.8.
class f1ap_du_removal_procedure
{
public:
  f1ap_du_removal_procedure(f1ap_du_connection_handler& du_conn_handler,
                            f1ap_message_notifier&      tx_pdu_notifier,
                            f1ap_event_manager&         ev_mng,
                            f1ap_du_context&            du_ctxt);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "F1 Removal"; }

  void send_f1_removal_request();

  void handle_f1_removal_response();

  f1ap_du_connection_handler& du_conn_handler;
  f1ap_message_notifier&      cu_notifier;
  f1ap_event_manager&         ev_mng;
  f1ap_du_context&            du_ctxt;
  srslog::basic_logger&       logger;

  f1ap_transaction transaction;
};

} // namespace srs_du
} // namespace srsran
