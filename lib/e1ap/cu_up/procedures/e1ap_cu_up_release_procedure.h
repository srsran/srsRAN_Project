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

#include "../e1ap_cu_up_connection_handler.h"
#include "e1ap_cu_up_event_manager.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

/// This coroutines handles the E1AP CU UP release procedure as per TS 38.483, 8.2.7.2.2.
class e1ap_cu_up_release_procedure
{
public:
  e1ap_cu_up_release_procedure(e1ap_cu_up_connection_handler& cu_up_conn_handler_,
                               e1ap_message_notifier&         tx_pdu_notifier_,
                               e1ap_event_manager&            ev_mng_,
                               srslog::basic_logger&          logger_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "E1AP CU-UP Release Procedure"; }

  void send_e1ap_release_request();

  void handle_e1ap_release_response();

  e1ap_cu_up_connection_handler& cu_up_conn_handler;
  e1ap_message_notifier&         cu_notifier;
  e1ap_event_manager&            ev_mng;
  srslog::basic_logger&          logger;

  e1ap_transaction transaction;
};

} // namespace srs_cu_up
} // namespace srsran
