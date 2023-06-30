/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../common/e2ap_asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/support/async/async_task.h"

class e2_event_manager;

namespace srsran {

class e2_setup_procedure
{
public:
  e2_setup_procedure(const e2_setup_request_message& request_,
                     e2_message_notifier&            notif_,
                     e2_event_manager&               ev_mng_,
                     timer_factory                   timers,
                     srslog::basic_logger&           logger);

  void operator()(coro_context<async_task<e2_setup_response_message>>& ctx);

private:
  /// Send E2 SETUP REQUEST.
  void send_e2_setup_request();

  /// Checks whether we should attempt again to connect to E2.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  e2_setup_response_message create_e2_setup_result();

  const e2_setup_request_message request;
  e2_message_notifier&           notifier;
  e2_event_manager&              ev_mng;
  srslog::basic_logger&          logger;

  unique_timer e2_setup_wait_timer;

  e2ap_transaction     transaction;
  unsigned             e2_setup_retry_no = 0;
  std::chrono::seconds time_to_wait{0};
};

} // namespace srsran