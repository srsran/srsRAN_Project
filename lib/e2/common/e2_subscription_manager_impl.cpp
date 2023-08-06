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

#define E2SM_IFACE(ra_nfunction_id_value) e2sm_iface_list[supported_ran_functions[ra_nfunction_id_value]]

e2_subscription_manager_impl::e2_subscription_manager_impl(e2_message_notifier& notif_) :
  notif(notif_), logger(srslog::fetch_basic_logger("E2-SUBSCRIBER"))
{
}

e2_subscribe_reponse_message
e2_subscription_manager_impl::handle_subscription_setup(const asn1::e2ap::ricsubscription_request_s& msg)
{
  e2_subscription_t            subscription = {};
  e2_subscribe_reponse_message outcome;
  subscription.subscription_info.request_id.ric_requestor_id = msg->ri_crequest_id.value.ric_requestor_id;
  subscription.subscription_info.ran_function_id             = msg->ra_nfunction_id->value;
  subscription.subscription_info.request_id.ric_instance_id  = msg->ri_crequest_id.value.ric_instance_id;
  e2_sm_kpm_event_trigger_definition_s event_trigger_def;

  if (supported_ran_functions.count(msg->ra_nfunction_id.value)) {
    event_trigger_def =
        e2sm_iface_list[supported_ran_functions[msg->ra_nfunction_id.value]]
            ->get_e2sm_packer()
            .handle_packed_event_trigger_definition(msg->ricsubscription_details->ric_event_trigger_definition);
    subscription.subscription_info.report_period =
        event_trigger_def.event_definition_formats.event_definition_format1().report_period;
    outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
    outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
    outcome.ran_function_id             = subscription.subscription_info.ran_function_id;
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
    outcome.request_id.ric_requestor_id = subscription.subscription_info.request_id.ric_requestor_id;
    outcome.request_id.ric_instance_id  = subscription.subscription_info.request_id.ric_instance_id;
    outcome.success                     = false;
    outcome.cause.set_protocol();
    logger.error("RAN function ID not supported");
  }
  return outcome;
}

e2_subscribe_delete_response_message
e2_subscription_manager_impl::handle_subscription_delete(const asn1::e2ap::ricsubscription_delete_request_s& msg)
{
  e2_subscribe_delete_response_message outcome;
  outcome.request_id.ric_requestor_id     = msg->ri_crequest_id.value.ric_requestor_id;
  outcome.request_id.ric_instance_id      = msg->ri_crequest_id.value.ric_instance_id;
  outcome.response->ra_nfunction_id.value = msg->ra_nfunction_id.value;
  outcome.response->ri_crequest_id.value  = msg->ri_crequest_id.value;
  outcome.success                         = false;
  if (subscriptions.count(outcome.request_id.ric_instance_id)) {
    outcome.success = true;
  } else {
    outcome.failure->cause->set_misc();
    logger.error("RIC instance ID not found");
  }
  return outcome;
}

void e2_subscription_manager_impl::start_subscription(int               ric_instance_id,
                                                      e2_event_manager& ev_mng,
                                                      uint16_t          ran_func_id)
{
  subscriptions[ric_instance_id].indication_task =
      launch_async<e2_indication_procedure>(notif,
                                            *(e2sm_iface_list[supported_ran_functions[ran_func_id]]),
                                            ev_mng,
                                            subscriptions[ric_instance_id].subscription_info,
                                            logger);
}

void e2_subscription_manager_impl::stop_subscription(int               ric_instance_id,
                                                     e2_event_manager& ev_mng,
                                                     const asn1::e2ap::ricsubscription_delete_request_s& msg)
{
  if (subscriptions.count(ric_instance_id)) {
    ev_mng.sub_del_reqs[ric_instance_id]->set(msg);
    subscriptions[ric_instance_id].indication_task.await_ready();
    subscriptions.erase(ric_instance_id);
  } else {
    logger.error("RIC instance ID not found");
  }
}
bool e2_subscription_manager_impl::action_supported(const ri_caction_to_be_setup_item_s& action,
                                                    uint16_t                             ran_func_id,
                                                    uint32_t                             ric_instance_id)
{
  auto action_def =
      e2sm_iface_list[supported_ran_functions[ran_func_id]]->get_e2sm_packer().handle_packed_e2sm_kpm_action_definition(
          action.ric_action_definition);
  auto action_type = action_def.action_definition_formats.type().value;
  if (action_type == e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::nulltype) {
    subscriptions[ric_instance_id].subscription_info.action_list.push_back(
        {action.ric_action_definition.deep_copy(), action.ric_action_id, action.ric_action_type});
    return true;
  }
  if (action_type ==
      e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::action_definition_format1) {
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

void e2_subscription_manager_impl::add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface)
{
  e2sm_iface_list.emplace(oid, std::move(e2sm_iface));
}

e2sm_interface* e2_subscription_manager_impl::get_e2sm_interface(std::string oid)
{
  auto it = e2sm_iface_list.find(oid);
  if (it == e2sm_iface_list.end()) {
    logger.error("OID not supported");
    return nullptr;
  }
  return &(*(it->second));
}

void e2_subscription_manager_impl::add_ran_function_oid(uint16_t ran_func_id, std::string oid)
{
  if (e2sm_iface_list.count(oid)) {
    supported_ran_functions.emplace(ran_func_id, oid);
  } else {
    logger.error("OID not supported");
  }
}
