/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#pragma once

#include "../common/e2ap_asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_event_manager.h"
#include "srsran/e2/subscription/e2_subscription.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

class e2_subsciption_notifier;
namespace srsran {

class e2_subscription_delete_procedure
{
public:
  e2_subscription_delete_procedure(const asn1::e2ap::ric_sub_delete_request_s& request_,
                                   e2_event_manager&                           event_manager_,
                                   e2_message_notifier&                        ric_notif_,
                                   e2_subscription_proc&                       subscription_mngr_,
                                   timer_factory                               timers_,
                                   srslog::basic_logger&                       logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "E2 Subscription Delete Procedure"; }

private:
  // results senders
  void send_e2_subscription_delete_response(const e2_subscribe_delete_response_message& response);
  void send_e2_subscription_delete_failure(const e2_subscribe_delete_response_message& failure);

  const asn1::e2ap::ric_sub_delete_request_s& request;
  e2_event_manager&                           event_manager;
  srslog::basic_logger&                       logger;
  e2_message_notifier&                        ric_notif;
  e2_subscription_proc&                       subscription_mngr;
  timer_factory                               timers;
};

} // namespace srsran
