/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1_cu_cp_impl.h"
#include "../../ran/gnb_format.h"
#include "srsgnb/asn1/e1ap.h"

using namespace srsgnb;
using namespace asn1::e1ap;
using namespace srs_cu_cp;

e1_cu_cp_impl::e1_cu_cp_impl(timer_manager&            timers_,
                             e1_message_notifier&      e1_pdu_notifier_,
                             e1_du_processor_notifier& e1_du_processor_notifier_) :
  logger(srslog::fetch_basic_logger("CU-CP-E1")),
  timers(timers_),
  pdu_notifier(e1_pdu_notifier_),
  du_processor_notifier(e1_du_processor_notifier_),
  events(std::make_unique<e1_event_manager>(timers))
{
  e1ap_ue_context empty_context = {};
  std::fill(cu_cp_ue_id_to_e1ap_ue_context.begin(), cu_cp_ue_id_to_e1ap_ue_context.end(), empty_context);
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
e1_cu_cp_impl::~e1_cu_cp_impl() {}

void e1_cu_cp_impl::handle_cu_up_e1_setup_response(const cu_up_e1_setup_response_message& msg)
{
  // Pack message into PDU
  e1_message e1_msg;
  if (msg.success) {
    logger.info("Transmitting CuUpE1SetupResponse message");

    e1_msg.pdu.set_successful_outcome();
    e1_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_G_NB_CU_UP_E1_SETUP);
    e1_msg.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp() = msg.response;

    // set values handled by E1
    e1_msg.pdu.successful_outcome().value.gnb_cu_up_e1_setup_resp()->transaction_id.value = current_transaction_id;

    // send response
    pdu_notifier.on_new_message(e1_msg);
  } else {
    logger.info("Transmitting CuUpE1SetupFailure message");
    e1_msg.pdu.set_unsuccessful_outcome();
    e1_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_G_NB_CU_UP_E1_SETUP);
    e1_msg.pdu.unsuccessful_outcome().value.gnb_cu_up_e1_setup_fail() = msg.failure;
    auto& setup_fail = e1_msg.pdu.unsuccessful_outcome().value.gnb_cu_up_e1_setup_fail();

    // set values handled by E1
    setup_fail->transaction_id.value = current_transaction_id;
    setup_fail->cause.value.set_radio_network();
    setup_fail->cause.value.radio_network() = asn1::e1ap::cause_radio_network_opts::options::no_radio_res_available;

    // send response
    pdu_notifier.on_new_message(e1_msg);
  }
}

async_task<cu_cp_e1_setup_response_message>
e1_cu_cp_impl::handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request_message& request)
{
  return launch_async<cu_cp_e1_setup_procedure>(request, pdu_notifier, *events, timers, logger);
}

async_task<e1ap_bearer_context_setup_response_message>
e1_cu_cp_impl::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request_message& request)
{
  return launch_async<e1_bearer_context_setup_procedure>(request.msg, pdu_notifier, *events, logger);
}

async_task<e1ap_bearer_context_modification_response_message> e1_cu_cp_impl::handle_bearer_context_modification_request(
    const e1ap_bearer_context_modification_request_message& request)
{
  return launch_async<e1_bearer_context_modification_procedure>(request.msg, pdu_notifier, *events, logger);
}

async_task<e1ap_bearer_context_release_complete_message>
e1_cu_cp_impl::handle_bearer_context_release_command(const e1ap_bearer_context_release_command_message& command)
{
  return launch_async<e1_bearer_context_release_procedure>(command, pdu_notifier, *events, logger);
}

void e1_cu_cp_impl::handle_message(const e1_message& msg)
{
  logger.info("Handling E1 PDU of type {}", msg.pdu.type().to_string());

  // Log message.
  expected<gnb_cu_cp_ue_e1ap_id_t> gnb_cu_cp_ue_e1ap_id = get_gnb_cu_cp_ue_e1ap_id(msg.pdu);
  expected<uint8_t>                transaction_id       = get_transaction_id(msg.pdu);
  if (transaction_id.has_value()) {
    logger.info("E1AP SDU, \"{}.{}\", transaction id={}",
                msg.pdu.type().to_string(),
                get_message_type_str(msg.pdu),
                transaction_id.value());
  } else if (gnb_cu_cp_ue_e1ap_id.has_value()) {
    logger.info("E1AP SDU, \"{}.{}\", GNB-CU-CP-UE-E1AP-ID={}",
                msg.pdu.type().to_string(),
                get_message_type_str(msg.pdu),
                gnb_cu_cp_ue_e1ap_id.value());
  } else {
    logger.info("E1AP SDU, \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));
  }

  switch (msg.pdu.type().value) {
    case asn1::e1ap::e1_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    case asn1::e1ap::e1_ap_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(msg.pdu.successful_outcome());
      break;
    case asn1::e1ap::e1_ap_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid E1 PDU type");
      break;
  }
}

void e1_cu_cp_impl::handle_initiating_message(const asn1::e1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types_opts::options::gnb_cu_up_e1_setup_request: {
      cu_up_e1_setup_request_message req_msg = {};
      current_transaction_id                 = msg.value.gnb_cu_up_e1_setup_request()->transaction_id.value;
      req_msg.request                        = msg.value.gnb_cu_up_e1_setup_request();
      du_processor_notifier.on_e1_setup_request_received(req_msg);
    } break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void e1_cu_cp_impl::handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e1ap::e1_ap_elem_procs_o::successful_outcome_c::types_opts::bearer_context_release_complete: {
      events->e1ap_bearer_context_release_complete.set(&outcome.value.bearer_context_release_complete());
    } break;
    case asn1::e1ap::e1_ap_elem_procs_o::successful_outcome_c::types_opts::bearer_context_setup_resp: {
      events->e1ap_bearer_context_setup_response_message.set(&outcome.value.bearer_context_setup_resp());
    } break;
    case asn1::e1ap::e1_ap_elem_procs_o::successful_outcome_c::types_opts::bearer_context_mod_resp: {
      events->e1ap_bearer_context_modification_response_message.set(&outcome.value.bearer_context_mod_resp());
    } break;
    default:
      // Handle successful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }

      // Set transaction result and resume suspended procedure.
      events->transactions.set(transaction_id.value(), outcome);
  }
}

void e1_cu_cp_impl::handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::e1ap::e1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::bearer_context_setup_fail: {
      events->e1ap_bearer_context_setup_response_message.set(&outcome.value.bearer_context_setup_fail());
    } break;
    case asn1::e1ap::e1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::bearer_context_mod_fail: {
      events->e1ap_bearer_context_modification_response_message.set(&outcome.value.bearer_context_mod_fail());
    } break;
    default:
      // Handle unsuccessful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }

      // Set transaction result and resume suspended procedure.
      events->transactions.set(transaction_id.value(), outcome);
  }
}

gnb_cu_cp_ue_e1ap_id_t e1_cu_cp_impl::get_next_cu_cp_ue_id()
{
  for (int cu_cp_ue_id = MIN_UE_INDEX; cu_cp_ue_id < MAX_NOF_UES; cu_cp_ue_id++) {
    if (cu_cp_ue_id_to_e1ap_ue_context[cu_cp_ue_id].ue_index == INVALID_UE_INDEX) {
      return int_to_gnb_cu_cp_ue_e1ap_id_t(cu_cp_ue_id);
    }
  }
  logger.error("No CU-CP UE ID available");
  return gnb_cu_cp_ue_e1ap_id_t::invalid;
}

gnb_cu_cp_ue_e1ap_id_t e1_cu_cp_impl::find_cu_cp_ue_id(ue_index_t ue_index)
{
  for (int cu_cp_ue_id = MIN_UE_INDEX; cu_cp_ue_id < MAX_NOF_UES; cu_cp_ue_id++) {
    if (cu_cp_ue_id_to_e1ap_ue_context[cu_cp_ue_id].ue_index == ue_index) {
      return int_to_gnb_cu_cp_ue_e1ap_id_t(cu_cp_ue_id);
    }
  }
  logger.error("CU-CP UE ID for ue_index={} not found", ue_index);
  return gnb_cu_cp_ue_e1ap_id_t::invalid;
}
