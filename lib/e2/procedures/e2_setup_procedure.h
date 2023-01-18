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

#include "../common/e2ap_asn1_utils.h"
#include "e2_event_manager.h"
#include "srsgnb/asn1/e2ap/e2ap.h"
#include "srsgnb/e2/e2.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/timers.h"

class e2_event_manager;

namespace srsgnb {

class e2_setup_procedure
{
public:
  e2_setup_procedure(const e2_setup_request_message& request_,
                     e2_message_notifier&            notif_,
                     e2_event_manager&               ev_mng_,
                     timer_manager&                  timers,
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

  e2ap_transaction transaction;
  unsigned         e2_setup_retry_no = 0;
  unsigned         time_to_wait      = 0;
};

} // namespace srsgnb