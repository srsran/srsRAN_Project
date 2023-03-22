/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  using namespace asn1::e2ap;
  logger.info("E2AP: Received subscription request");
  sub_notif.handle_subscription(request_);

  e2_subscribe_reponse_message response;
  sub_notif.get_subscription_result(response, request_->ri_crequest_id.value);
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
