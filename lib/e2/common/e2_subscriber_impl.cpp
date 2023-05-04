/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_subscriber_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"

using namespace asn1::e2ap;
using namespace srsran;

e2_subscriber_impl::e2_subscriber_impl(e2sm_kpm_handler& e2sm_kpm) : e2sm_handler(e2sm_kpm) {}

e2_subscribe_reponse_message
e2_subscriber_impl::handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg)
{
  e2_subscription_t subscription;
  subscription.request_id.ric_requestor_id = msg->ri_crequest_id.value.ric_requestor_id;
  for (auto& action : msg->ricsubscription_details.value.ric_action_to_be_setup_list) {
    auto& action_def = action.value().ri_caction_to_be_setup_item();
    subscription.action_list.push_back(
        {e2sm_handler.handle_packed_e2sm_kpm_action_definition(action_def.ric_action_definition),
         action_def.ric_action_id});
  }
  auto event_trigger_def =
      e2sm_handler.handle_packed_event_trigger_definition(msg->ricsubscription_details->ric_event_trigger_definition);
  subscription.report_period = event_trigger_def.event_definition_formats.event_definition_format1().report_period;
  subscriptions.insert(std::pair<int, e2_subscription_t>(subscription.request_id.ric_instance_id, subscription));
  e2_subscribe_reponse_message outcome;
  outcome.request_id.ric_requestor_id = subscription.request_id.ric_requestor_id;
  get_subscription_result(outcome, msg->ri_crequest_id.value);
  return outcome;
}

int e2_subscriber_impl::start_subscription()
{
  return 0;
}

void e2_subscriber_impl::get_subscription_result(e2_subscribe_reponse_message& outcome,
                                                 asn1::e2ap::ri_crequest_id_s  request_id)
{
  for (auto& subscription : subscriptions) {
    if (subscription.second.request_id.ric_requestor_id == request_id.ric_requestor_id) {
      outcome.request_id.ric_requestor_id = subscription.second.request_id.ric_requestor_id;
      outcome.request_id.ric_instance_id  = subscription.second.request_id.ric_instance_id;
      outcome.admitted_list.resize(subscription.second.action_list.size());
      for (unsigned i = 0; i < subscription.second.action_list.size(); i++) {
        outcome.admitted_list[i].value().ri_caction_admitted_item().ric_action_id =
            subscription.second.action_list[i].ric_action_id;
      }
      return;
    }
  }
}