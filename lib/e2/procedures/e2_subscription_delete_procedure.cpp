/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "e2_subscription_delete_procedure.h"
using namespace srsran;
using namespace asn1::e2ap;

e2_subscription_delete_procedure::e2_subscription_delete_procedure(e2_message_notifier&  ric_notif_,
                                                                   e2_subscription_proc& subscription_mngr_,
                                                                   timer_factory         timers_,
                                                                   srslog::basic_logger& logger_) :
  logger(logger_), ric_notif(ric_notif_), subscription_mngr(subscription_mngr_), timers(timers_)
{
}

void e2_subscription_delete_procedure::run_subscription_delete_procedure(
    const asn1::e2ap::ricsubscription_delete_request_s request_,
    e2_event_manager&                                  event_manager)
{
  logger.info("E2AP: Received subscription delete request");
  e2_subscribe_delete_response_message response;
  response = subscription_mngr.handle_subscription_delete(request_);
  if (response.success) {
    subscription_mngr.stop_subscription(request_->ri_crequest_id.value, event_manager, request_);
    send_e2_subscription_delete_response(response);
  } else {
    send_e2_subscription_delete_failure(response);
  }
}

void e2_subscription_delete_procedure::send_e2_subscription_delete_response(
    const e2_subscribe_delete_response_message& response)
{
  e2_message msg;
  logger.info("E2AP: Sending subscription delete response");
  msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION_DELETE);
  msg.pdu.successful_outcome().value.ricsubscription_delete_resp() = response.response;
  ric_notif.on_new_message(msg);
}

void e2_subscription_delete_procedure::send_e2_subscription_delete_failure(
    const e2_subscribe_delete_response_message& failure)
{
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION_DELETE);
  msg.pdu.unsuccessful_outcome().value.ricsubscription_delete_fail() = failure.failure;
  ric_notif.on_new_message(msg);
  logger.info("E2AP: Sending subscription delete failure");
}
