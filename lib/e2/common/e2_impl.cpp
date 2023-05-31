/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_impl.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/e2/e2.h"
#include "srsran/ran/nr_cgi.h"
#include <memory>

using namespace srsran;
using namespace asn1::e2ap;

e2_impl::e2_impl(timer_factory            timers_,
                 e2_message_notifier&     e2_pdu_notifier_,
                 e2_subscription_manager& subscription_mngr_) :
  logger(srslog::fetch_basic_logger("E2")),
  timers(timers_),
  pdu_notifier(e2_pdu_notifier_),
  subscription_mngr(subscription_mngr_),
  subscribe_proc(e2_pdu_notifier_, subscription_mngr, timers, logger),
  events(std::make_unique<e2_event_manager>(timers))
{
}

async_task<e2_setup_response_message> e2_impl::handle_e2_setup_request(const e2_setup_request_message& request)
{
  for (unsigned i = 0; i < request.request->ra_nfunctions_added.value.size(); i++) {
    auto&    item               = request.request->ra_nfunctions_added.value[i].value().ra_nfunction_item();
    uint16_t id                 = item.ran_function_id;
    candidate_ran_functions[id] = item;
    logger.info("Added RAN function OID {} to candidate list",
                candidate_ran_functions[id].ran_function_oid.to_string().c_str());
  }
  return launch_async<e2_setup_procedure>(request, pdu_notifier, *events, timers, logger);
}

void e2_impl::handle_e2_setup_response(const e2_setup_response_message& msg)
{
  e2_message e2_msg;
  if (msg.success) {
    logger.info("Transmitting E2 Setup Response message");
    e2_msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_E2SETUP);
    e2_msg.pdu.successful_outcome().value.e2setup_resp() = msg.response;
  }
  pdu_notifier.on_new_message(e2_msg);
}

void e2_impl::handle_ric_subscription_request(const asn1::e2ap::ricsubscription_request_s& msg)
{
  logger.info("Received RIC Subscription Request");
  subscribe_proc.run_subscription_procedure(msg, *events);
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
    case asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::options::e2setup_request:
      current_transaction_id = msg.value.e2setup_request()->transaction_id.value.value;
      // handle_e2_setup_request({msg.value.e2setup_request()});
      break;
    case asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::options::ricsubscription_request:
      handle_ric_subscription_request(msg.value.ricsubscription_request());
      break;
    case asn1::e2ap::e2_ap_elem_procs_o::init_msg_c::types_opts::options::ricsubscription_delete_request:
      logger.info("Received RIC Subscription Delete Request");
      events->sub_del_request.set(msg.value.ricsubscription_delete_request());
      // TODO - add subscription delete procedure
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
    } break;
    default:
      logger.error("Invalid E2AP unsuccessful outcome message type");
      break;
  }
}

void e2_impl::set_allowed_ran_functions(const uint16_t ran_function_id)
{
  if (candidate_ran_functions.count(ran_function_id)) {
    allowed_ran_functions[ran_function_id] = candidate_ran_functions[ran_function_id];
    logger.info("Added RAN function with id {}", ran_function_id);
    auto ran_oid = allowed_ran_functions[ran_function_id].ran_function_oid.to_string();
    if (ran_oid == "E2SM-KPM") {
      logger.info("Added RAN function with id {} and OID {}", ran_function_id, ran_oid);
      std::unique_ptr<e2sm_handler> e2sm_handler = std::make_unique<e2sm_kpm_asn1_packer>();
      subscription_mngr.add_e2sm_service(ran_function_id, std::move(e2sm_handler));
    } else if (ran_oid == "E2SM-RC") {
      logger.info("Added RAN function with id {} and OID {}", ran_function_id, ran_oid);
      // TODO add E2SM-RC handler
    } else if (ran_oid == "E2SM-NI") {
      logger.info("Added RAN function with id {} and OID {}", ran_function_id, ran_oid);
      // TODO add E2SM-NI handler
    } else {
      logger.warning("Not adding RAN function with id {} and OID {}", ran_function_id, ran_oid);
    }
  } else {
    logger.warning("RAN function with id {} is not a candidate", ran_function_id);
  }
}