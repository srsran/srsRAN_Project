/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsran/support/timers.h"
#include <memory>

using namespace srsran;
using namespace asn1::e2ap;

e2_impl::e2_impl(timer_factory timers_, e2_message_notifier& e2_pdu_notifier_) :
  logger(srslog::fetch_basic_logger("E2")),
  timers(timers_),
  pdu_notifier(e2_pdu_notifier_),
  events(std::make_unique<e2_event_manager>(timers))
{
}

async_task<e2_setup_response_message> e2_impl::handle_e2_setup_request(const e2_setup_request_message& request)
{
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
