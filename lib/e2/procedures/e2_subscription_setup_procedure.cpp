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

#include "e2_subscription_setup_procedure.h"
using namespace srsran;
using namespace asn1::e2ap;

e2_subscription_setup_procedure::e2_subscription_setup_procedure(e2_message_notifier&  ric_notif_,
                                                                 e2_subscriber&        sub_notif_,
                                                                 timer_factory         timers_,
                                                                 srslog::basic_logger& logger_) :
  logger(logger_), ric_notif(ric_notif_), sub_notif(sub_notif_), timers(timers_)
{
}

void e2_subscription_setup_procedure::run_subscription_procedure(const asn1::e2ap::ricsubscription_request_s request_)
{
  logger.info("E2AP: Received subscription request");
  e2_subscribe_reponse_message response;
  response = sub_notif.handle_subscription_setup(request_);
  if (response.success) {
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
  logger.info("E2AP: Sending subscription response");
}
