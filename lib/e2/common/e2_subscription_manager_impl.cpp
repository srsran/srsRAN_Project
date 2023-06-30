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

#include "e2_subscription_manager_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2_subscription_manager_impl::e2_subscription_manager_impl(e2sm_interface& e2sm_, e2_message_notifier& notif_) :

  e2sm_iface(e2sm_), notif(notif_), logger(srslog::fetch_basic_logger("E2-SUBSCRIBER"))
{
}

e2_subscribe_reponse_message
e2_subscription_manager_impl::handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg)
{
  e2_subscription_t            subscription = {};
  e2_subscribe_reponse_message outcome;
  subscription.subscription_info.request_id.ric_requestor_id = msg->ri_crequest_id.value.ric_requestor_id;
  e2_sm_kpm_event_trigger_definition_s event_trigger_def;

  if (e2sm_packer_list.count(msg->ra_nfunction_id.value)) {
    event_trigger_def = e2sm_packer_list[msg->ra_nfunction_id.value]->handle_packed_event_trigger_definition(
        msg->ricsubscription_details->ric_event_trigger_definition);
    subscription.subscription_info.report_period =
        event_trigger_def.event_definition_formats.event_definition_format1().report_period;
    outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
    outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
    subscriptions.insert(std::pair<int, e2_subscription_t>(subscription.subscription_info.request_id.ric_instance_id,
                                                           std::move(subscription)));
    get_subscription_result(msg->ra_nfunction_id.value,
                            outcome,
                            subscriptions[outcome.request_id.ric_instance_id],
                            msg->ricsubscription_details->ric_action_to_be_setup_list);
    if (!outcome.success) {
      logger.error("Failed to setup subscription");
      subscriptions.erase(outcome.request_id.ric_instance_id);
    }
  } else {
    outcome.success = false;
    logger.error("RAN function ID not supported");
  }
  return outcome;
}

int e2_subscription_manager_impl::start_subscription(int ric_instance_id, e2_event_manager& ev_mng)
{
  subscriptions[ric_instance_id].indication_task = launch_async<e2_indication_procedure>(
      notif, e2sm_iface, ev_mng, subscriptions[ric_instance_id].subscription_info, logger);
  return 0;
}

bool e2_subscription_manager_impl::action_supported(const ri_caction_to_be_setup_item_s& action,
                                                    uint16_t                             ran_func_id,
                                                    uint32_t                             ric_instance_id)
{
  auto action_def =
      e2sm_packer_list[ran_func_id]->handle_packed_e2sm_kpm_action_definition(action.ric_action_definition);
  auto action_type = action_def.action_definition_formats.type().value;
  if (action_type == e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::nulltype) {
    subscriptions[ric_instance_id].subscription_info.action_list.push_back(
        {action.ric_action_definition.deep_copy(), action.ric_action_id, action.ric_action_type});
    return true;
  }
  if (action_type ==
      e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::action_definition_format3) {
    subscriptions[ric_instance_id].subscription_info.action_list.push_back(
        {action.ric_action_definition.deep_copy(), action.ric_action_id, action.ric_action_type});
    return true;
  }

  logger.error("Action not supported");
  return false;
}

void e2_subscription_manager_impl::get_subscription_result(uint16_t                              ran_func_id,
                                                           e2_subscribe_reponse_message&         outcome,
                                                           e2_subscription_t&                    subscription,
                                                           const ri_cactions_to_be_setup_list_l& actions)
{
  outcome.success                     = false;
  outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
  outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
  for (unsigned i = 0, e = actions.size(); i != e; ++i) {
    auto& action = actions[i].value().ri_caction_to_be_setup_item();
    if (action_supported(action, ran_func_id, outcome.request_id.ric_instance_id)) {
      outcome.success = true;
      outcome.admitted_list.resize(outcome.admitted_list.size() + 1);
      outcome.admitted_list[i].value().ri_caction_admitted_item().ric_action_id = action.ric_action_id;
    } else {
      outcome.not_admitted_list.resize(outcome.not_admitted_list.size() + 1);
      outcome.not_admitted_list[i].value().ri_caction_not_admitted_item().ric_action_id = action.ric_action_id;
    }
  }
}

void e2_subscription_manager_impl::add_e2sm_service(uint16_t ran_func_id, std::unique_ptr<e2sm_handler> e2sm_packer)
{
  e2sm_packer_list.emplace(ran_func_id, std::move(e2sm_packer));
}