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

#include "e2_impl.h"
#include "e2ap_asn1_helpers.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/ran/nr_cgi.h"
#include <memory>

using namespace srsran;
using namespace asn1::e2ap;
using namespace asn1;

e2_impl::e2_impl(e2ap_configuration&      cfg_,
                 timer_factory            timers_,
                 e2_message_notifier&     e2_pdu_notifier_,
                 e2_subscription_manager& subscription_mngr_) :
  logger(srslog::fetch_basic_logger("E2")),
  cfg(cfg_),
  timers(timers_),
  pdu_notifier(e2_pdu_notifier_),
  subscription_mngr(subscription_mngr_),
  subscribe_proc(e2_pdu_notifier_, subscription_mngr_, timers, logger),
  subscribe_delete_proc(e2_pdu_notifier_, subscription_mngr_, timers, logger),
  events(std::make_unique<e2_event_manager>(timers))
{
}

async_task<e2_setup_response_message> e2_impl::handle_e2_setup_request(e2_setup_request_message& request)
{
  for (unsigned i = 0; i < request.request->ra_nfunctions_added.value.size(); i++) {
    auto&    ran_function_item = request.request->ra_nfunctions_added.value[i].value().ra_nfunction_item();
    uint16_t id                = ran_function_item.ran_function_id;

    logger.info("Added RAN function OID {} to candidate list under RAN function ID {}",
                ran_function_item.ran_function_oid.to_string().c_str(),
                id);
    std::string     ran_oid  = ran_function_item.ran_function_oid.to_string();
    e2sm_interface* e2_iface = subscription_mngr.get_e2sm_interface(ran_oid);
    if (e2_iface == nullptr) {
      logger.error("No E2SM interface found for RAN OID {}", ran_oid.c_str());
      continue;
    }
    ran_function_item.ran_function_definition = e2_iface->get_e2sm_packer().pack_ran_function_description();
    if (ran_function_item.ran_function_definition.size() == 0) {
      logger.error("Failed to pack RAN function description");
      continue;
    }
    candidate_ran_functions[id] = ran_function_item;
  }
  return launch_async<e2_setup_procedure>(request, pdu_notifier, *events, timers, logger);
}

async_task<e2_setup_response_message> e2_impl::start_initial_e2_setup_routine()
{
  e2_setup_request_message request;
  fill_asn1_e2ap_setup_request(request.request, cfg, subscription_mngr);

  for (const auto& ran_function : request.request->ra_nfunctions_added.value) {
    auto&    ran_function_item = ran_function.value().ra_nfunction_item();
    uint16_t id                = ran_function_item.ran_function_id;
    logger.info("Added RAN function OID {} to candidate list under RAN Function ID {}",
                ran_function_item.ran_function_oid.to_string().c_str(),
                id);
    candidate_ran_functions[id] = ran_function_item;
  }

  return launch_async<e2_setup_procedure>(request, pdu_notifier, *events, timers, logger);
}

void e2_impl::handle_e2_setup_response(const e2_setup_response_message& msg)
{
  e2_message e2_msg;
  if (msg.success) {
    logger.info("Received E2 Setup Response message");
    e2_msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);
    e2_msg.pdu.successful_outcome().value.e2setup_resp() = msg.response;
  } else {
    logger.error("E2 Setup Failure message received");
    return;
  }
  if (e2_msg.pdu.successful_outcome().value.e2setup_resp()->ra_nfunctions_accepted_present) {
    for (unsigned i = 0, e = e2_msg.pdu.successful_outcome().value.e2setup_resp()->ra_nfunctions_accepted.value.size();
         i != e;
         ++i) {
      auto& ran_function_item = e2_msg.pdu.successful_outcome()
                                    .value.e2setup_resp()
                                    ->ra_nfunctions_accepted.value[i]
                                    .value()
                                    .ra_nfunction_id_item();
      uint16_t id = ran_function_item.ran_function_id;
      set_allowed_ran_functions(id);
    }
  }
}

void e2_impl::handle_e2_setup_failure(const e2_setup_response_message& msg)
{
  e2_message e2_msg;
  if (!msg.success) {
    logger.info("Transmitting E2 Setup Failure message");
    e2_msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);
    e2_msg.pdu.unsuccessful_outcome().value.e2setup_fail() = msg.failure;
  } else {
    logger.error("E2 Setup Response message received");
    return;
  }
}

void e2_impl::handle_ric_subscription_request(const asn1::e2ap::ricsubscription_request_s& msg)
{
  logger.info("Received RIC Subscription Request");
  subscribe_proc.run_subscription_procedure(msg, *events);
}

void e2_impl::handle_ric_subscription_delete_request(const asn1::e2ap::ricsubscription_delete_request_s& msg)
{
  logger.info("Received RIC Subscription Delete Request");
  subscribe_delete_proc.run_subscription_delete_procedure(msg, *events);
}

void e2_impl::handle_message(const e2_message& msg)
{
  logger.info("Handling E2 PDU of type {}", msg.pdu.type().to_string());

  // Log message.
  expected<uint8_t> transaction_id = get_transaction_id(msg.pdu);
  if (transaction_id.has_value()) {
    logger.info("E2AP msg, \"{}.{}\", transaction id={}",
                msg.pdu.type().to_string(),
                get_message_type_str(msg.pdu),
                transaction_id.value());
  } else {
    logger.info("E2AP SDU, \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));
  }

  switch (msg.pdu.type().value) {
    case asn1::e2ap::e2_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    case asn1::e2ap::e2_ap_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(msg.pdu.successful_outcome());
      break;
    case asn1::e2ap::e2_ap_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid E2 PDU type");
      break;
  }
}

void e2_impl::handle_initiating_message(const asn1::e2ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::options::ricsubscription_request:
      handle_ric_subscription_request(msg.value.ricsubscription_request());
      break;
    case asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::options::ricsubscription_delete_request:
      handle_ric_subscription_delete_request(msg.value.ricsubscription_delete_request());
      break;
    default:
      logger.error("Invalid E2AP initiating message type");
      break;
  }
}

void e2_impl::handle_successful_outcome(const asn1::e2ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e2ap::e2_ap_elem_procs_o::successful_outcome_c::types_opts::options::e2setup_resp: {
      // Handle successful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }
      // Set transaction result and resume suspended procedure.
      if (not events->transactions.set(transaction_id.value(), outcome)) {
        logger.warning("Unrecognized transaction id={}", transaction_id.value());
      }
      handle_e2_setup_response({outcome.value.e2setup_resp(), {}, true});
    } break;
    default:
      logger.error("Invalid E2AP successful outcome message type");
      break;
  }
}

void e2_impl::handle_unsuccessful_outcome(const asn1::e2ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e2ap::e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::e2setup_fail: {
      // Handle successful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }
      // Set transaction result and resume suspended procedure.
      if (not events->transactions.set(transaction_id.value(), outcome)) {
        logger.warning("Unrecognized transaction id={}", transaction_id.value());
      }
      handle_e2_setup_failure({{}, outcome.value.e2setup_fail(), false});
    } break;
    default:
      logger.error("Invalid E2AP unsuccessful outcome message type");
      break;
  }
}

void e2_impl::set_allowed_ran_functions(uint16_t ran_function_id)
{
  if (candidate_ran_functions.count(ran_function_id)) {
    allowed_ran_functions[ran_function_id] = candidate_ran_functions[ran_function_id];
    std::string ran_func_oid               = allowed_ran_functions[ran_function_id].ran_function_oid.to_string();
    subscription_mngr.add_ran_function_oid(ran_function_id, ran_func_oid);
    logger.info("Added RAN function with id {}", ran_function_id);
  } else {
    logger.warning("RAN function with id {} is not a candidate", ran_function_id);
  }
}