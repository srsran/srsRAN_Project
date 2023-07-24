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
#include "srsran/e2/subscription/e2_subscription.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

class e2_subsciption_notifier;
namespace srsran {

class e2_subscription_setup_procedure
{
public:
  e2_subscription_setup_procedure(e2_message_notifier&  ric_notif_,
                                  e2_subscription_proc& subscription_mngr_,
                                  timer_factory         timers_,
                                  srslog::basic_logger& logger_);

  void run_subscription_procedure(const asn1::e2ap::ricsubscription_request_s request_,
                                  e2_event_manager&                           event_manager);

private:
  // results senders
  void send_e2_subscription_setup_response(const e2_subscribe_reponse_message& response);
  void send_e2_subscription_setup_failure(const e2_subscribe_reponse_message& failure);

  srslog::basic_logger& logger;
  e2_message_notifier&  ric_notif;
  e2_subscription_proc& subscription_mngr;
  timer_factory         timers;
};

} // namespace srsran
