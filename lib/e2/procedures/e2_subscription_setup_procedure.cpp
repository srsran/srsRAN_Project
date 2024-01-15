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

#include "e2_subscription_setup_procedure.h"
using namespace srsran;
using namespace asn1::e2ap;

e2_subscription_setup_procedure::e2_subscription_setup_procedure(e2_message_notifier&  ric_notif_,
                                                                 e2_subscription_proc& subscription_mngr_,
                                                                 timer_factory         timers_,
                                                                 srslog::basic_logger& logger_) :
  logger(logger_), ric_notif(ric_notif_), subscription_mngr(subscription_mngr_), timers(timers_)
{
}

void e2_subscription_setup_procedure::run_subscription_procedure(const asn1::e2ap::ricsubscription_request_s request_,
                                                                 e2_event_manager& event_manager)
{
  logger.info("E2AP: Received subscription request");
  e2_subscribe_reponse_message response;
  response = subscription_mngr.handle_subscription_setup(request_);
  if (response.success) {
    event_manager.add_sub_del_req(request_->ri_crequest_id.value, timers);
    subscription_mngr.start_subscription(response.request_id, event_manager, request_->ra_nfunction_id.value);
    send_e2_subscription_setup_response(response);
  } else {
    send_e2_subscription_setup_failure(response);
  }
}

void e2_subscription_setup_procedure::send_e2_subscription_setup_response(const e2_subscribe_reponse_message& response)
{
  e2_message msg;
  msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION);
  auto& sub_resp                                  = msg.pdu.successful_outcome().value.ricsubscription_resp();
  sub_resp->ri_crequest_id.value.ric_instance_id  = response.request_id.ric_instance_id;
  sub_resp->ri_crequest_id.value.ric_requestor_id = response.request_id.ric_requestor_id;
  int size                                        = response.admitted_list.size();
  sub_resp->ra_nfunction_id->value                = response.ran_function_id;
  sub_resp->ri_cactions_admitted.value.resize(size);
  for (int i = 0; i < size; i++) {
    sub_resp->ri_cactions_admitted.value[0].value().ri_caction_admitted_item().ric_action_id =
        response.admitted_list[i].value().ri_caction_admitted_item().ric_action_id;
  }
  logger.info("E2AP: Sending subscription response");
  ric_notif.on_new_message(msg);
}

void e2_subscription_setup_procedure::send_e2_subscription_setup_failure(const e2_subscribe_reponse_message& failure)
{
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_RICSUBSCRIPTION);
  auto& sub_fail                                  = msg.pdu.unsuccessful_outcome().value.ricsubscription_fail();
  sub_fail->ri_crequest_id.value.ric_instance_id  = failure.request_id.ric_instance_id;
  sub_fail->ri_crequest_id.value.ric_requestor_id = failure.request_id.ric_requestor_id;
  sub_fail->cause.value                           = failure.cause;
  ric_notif.on_new_message(msg);
  logger.info("E2AP: Sending subscription failure");
}
