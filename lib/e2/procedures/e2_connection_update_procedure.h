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

#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {

class e2_connection_update_procedure
{
public:
  e2_connection_update_procedure(const asn1::e2ap::e2conn_upd_s& request_,
                                 e2_message_notifier&            ric_notif_,
                                 timer_factory                   timers_,
                                 srslog::basic_logger&           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2 Connection Update Procedure"; }

private:
  // results senders
  void send_e2_connection_update_ack();
  void send_e2_connection_update_failure();

  const asn1::e2ap::e2conn_upd_s request;
  srslog::basic_logger&          logger;
  e2_message_notifier&           ric_notif;
  timer_factory                  timers;
};

} // namespace srsran
