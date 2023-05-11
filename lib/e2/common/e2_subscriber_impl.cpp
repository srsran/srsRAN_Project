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

e2_subscriber_impl::e2_subscriber_impl(e2sm_kpm_handler&        e2sm_kpm,
                                       e2_message_notifier&     notif_,
                                       e2_du_metrics_interface& du_metrics_interface_) :
  e2sm_handler(e2sm_kpm),
  notif(notif_),
  du_metrics_interface(du_metrics_interface_),
  logger(srslog::fetch_basic_logger("E2-SUBSCRIBER"))
{
}

e2_subscribe_reponse_message
e2_subscriber_impl::handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg)
{
  e2_subscription_t subscription                             = {};
  subscription.subscription_info.request_id.ric_requestor_id = msg->ri_crequest_id.value.ric_requestor_id;
  for (auto& action : msg->ricsubscription_details.value.ric_action_to_be_setup_list) {
    auto& action_def = action.value().ri_caction_to_be_setup_item();
    subscription.subscription_info.action_list.push_back(
        {e2sm_handler.handle_packed_e2sm_kpm_action_definition(action_def.ric_action_definition),
         action_def.ric_action_id});
  }
  auto event_trigger_def =
      e2sm_handler.handle_packed_event_trigger_definition(msg->ricsubscription_details->ric_event_trigger_definition);
  subscription.subscription_info.report_period =
      event_trigger_def.event_definition_formats.event_definition_format1().report_period;
  e2_subscribe_reponse_message outcome;
  outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
  outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
  get_subscription_result(outcome, subscription);
  if (outcome.success) {
    subscriptions.insert(std::pair<int, e2_subscription_t>(subscription.subscription_info.request_id.ric_instance_id,
                                                           std::move(subscription)));
  } else {
    logger.error("Subscription not supported");
  }
  return outcome;
}

// in this function we start the indication procedure for this subscription
int e2_subscriber_impl::start_subscription(int ric_instance_id, e2_event_manager& ev_mng)
{
  subscriptions[ric_instance_id].indication_task = launch_async<e2_indication_procedure>(
      notif, ev_mng, subscriptions[ric_instance_id].subscription_info, du_metrics_interface, logger);
  return 0;
}

// check if action is supported by the E2 agent and return the result
bool e2_subscriber_impl::action_supported(ric_action_t action)
{
  auto action_type = action.action_definition.action_definition_formats.type().value;
  if (action_type ==
      asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::nulltype) {
    return true;
  } else if (action_type == asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
                                action_definition_format3) {
    return true;
  } else {
    logger.error("Action not supported");
    return false;
  }
}

// create subscription result message
void e2_subscriber_impl::get_subscription_result(e2_subscribe_reponse_message& outcome, e2_subscription_t& subscription)
{
  outcome.success                     = false;
  outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
  outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
  for (unsigned i = 0; i < subscription.subscription_info.action_list.size(); i++) {
    if (action_supported(subscription.subscription_info.action_list[i])) {
      outcome.success = true;
      outcome.admitted_list.resize(outcome.admitted_list.size() + 1);
      outcome.admitted_list[i].value().ri_caction_admitted_item().ric_action_id =
          subscription.subscription_info.action_list[i].ric_action_id;
    } else {
      outcome.not_admitted_list.resize(outcome.not_admitted_list.size() + 1);
      outcome.not_admitted_list[i].value().ri_caction_not_admitted_item().ric_action_id =
          subscription.subscription_info.action_list[i].ric_action_id;
    }
  }
}