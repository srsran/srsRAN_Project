/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_subscription_manager_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"

using namespace asn1::e2ap;
using namespace srsran;

e2_subscription_manager_impl::e2_subscription_manager_impl(e2sm_handler&            e2sm_packer_,
                                                           e2sm_interface&          e2sm_,
                                                           e2_message_notifier&     notif_,
                                                           e2_du_metrics_interface& du_metrics_interface_) :
  e2sm_packer(e2sm_packer_),
  e2sm_iface(e2sm_),
  notif(notif_),
  du_metrics_interface(du_metrics_interface_),
  logger(srslog::fetch_basic_logger("E2-SUBSCRIBER"))
{
}

e2_subscribe_reponse_message
e2_subscription_manager_impl::handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg)
{
  e2_subscription_t subscription                             = {};
  subscription.subscription_info.request_id.ric_requestor_id = msg->ri_crequest_id.value.ric_requestor_id;
  auto event_trigger_def =
      e2sm_packer.handle_packed_event_trigger_definition(msg->ricsubscription_details->ric_event_trigger_definition);
  subscription.subscription_info.report_period =
      event_trigger_def.event_definition_formats.event_definition_format1().report_period;
  e2_subscribe_reponse_message outcome;
  outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
  outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
  subscriptions.insert(std::pair<int, e2_subscription_t>(subscription.subscription_info.request_id.ric_instance_id,
                                                         std::move(subscription)));
  get_subscription_result(outcome,
                          subscriptions[outcome.request_id.ric_instance_id],
                          msg->ricsubscription_details->ric_action_to_be_setup_list);
  if (!outcome.success) {
    logger.error("Failed to setup subscription");
    subscriptions.erase(outcome.request_id.ric_instance_id);
  }
  return outcome;
}

// in this function we start the indication procedure for this subscription
int e2_subscription_manager_impl::start_subscription(int ric_instance_id, e2_event_manager& ev_mng)
{
  subscriptions[ric_instance_id].indication_task = launch_async<e2_indication_procedure>(
      notif, e2sm_iface, ev_mng, subscriptions[ric_instance_id].subscription_info, logger);
  return 0;
}

// check if action is supported by the E2 agent and return the result
bool e2_subscription_manager_impl::action_supported(const srsran::byte_buffer& action_definition,
                                                    uint32_t                   ric_instance_id,
                                                    uint16_t                   ric_action_id)
{
  auto action_def  = e2sm_packer.handle_packed_e2sm_kpm_action_definition(action_definition);
  auto action_type = action_def.action_definition_formats.type().value;
  if (action_type ==
      asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::nulltype) {
    subscriptions[ric_instance_id].subscription_info.action_list.push_back(
        {action_definition.deep_copy(), ric_action_id});
    return true;
  } else if (action_type == asn1::e2sm_kpm::e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::
                                action_definition_format3) {
    subscriptions[ric_instance_id].subscription_info.action_list.push_back(
        {action_definition.deep_copy(), ric_action_id});
    return true;
  } else {
    logger.error("Action not supported");
    return false;
  }
}

// create subscription result message
void e2_subscription_manager_impl::get_subscription_result(e2_subscribe_reponse_message&         outcome,
                                                           e2_subscription_t&                    subscription,
                                                           const ri_cactions_to_be_setup_list_l& actions)
{
  outcome.success                     = false;
  outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
  outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
  for (unsigned i = 0; i < actions.size(); i++) {
    auto& action = actions[i].value().ri_caction_to_be_setup_item();
    if (action_supported(action.ric_action_definition, outcome.request_id.ric_instance_id, action.ric_action_id)) {
      outcome.success = true;
      outcome.admitted_list.resize(outcome.admitted_list.size() + 1);
      outcome.admitted_list[i].value().ri_caction_admitted_item().ric_action_id = action.ric_action_id;
    } else {
      outcome.not_admitted_list.resize(outcome.not_admitted_list.size() + 1);
      outcome.not_admitted_list[i].value().ri_caction_not_admitted_item().ric_action_id = action.ric_action_id;
    }
  }
}
