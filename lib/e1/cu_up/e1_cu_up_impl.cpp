/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1_cu_up_impl.h"
#include "../../ran/gnb_format.h"
#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/ran/bcd_helpers.h"

using namespace srsgnb;
using namespace asn1::e1ap;
using namespace srs_cu_up;

e1_cu_up_impl::e1_cu_up_impl(e1_message_notifier& e1_pdu_notifier_,
                             e1ap_cu_cp_notifier& e1_cu_up_notifier_,
                             task_executor&       cu_up_exec_) :
  cu_up_exec(cu_up_exec_),
  logger(srslog::fetch_basic_logger("CU-UP-E1")),
  pdu_notifier(e1_pdu_notifier_),
  e1_cu_up_notifier(e1_cu_up_notifier_)
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
e1_cu_up_impl::~e1_cu_up_impl() {}

void e1_cu_up_impl::handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response& msg)
{
  // Pack message into PDU
  e1_message e1_msg;
  if (msg.success) {
    logger.info("Transmitting CuCpE1SetupResponse message");

    e1_msg.pdu.set_successful_outcome();
    e1_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);
    e1_msg.pdu.successful_outcome().value.gnb_cu_cp_e1_setup_resp() = msg.response;

    // set values handled by E1
    e1_msg.pdu.successful_outcome().value.gnb_cu_cp_e1_setup_resp()->transaction_id.value = current_transaction_id;

    // send response
    pdu_notifier.on_new_message(e1_msg);
  } else {
    logger.info("Transmitting CuCpE1SetupFailure message");
    e1_msg.pdu.set_unsuccessful_outcome();
    e1_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);
    e1_msg.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail() = msg.failure;
    auto& setup_fail = e1_msg.pdu.unsuccessful_outcome().value.gnb_cu_cp_e1_setup_fail();

    // set values handled by E1
    setup_fail->transaction_id.value = current_transaction_id;
    setup_fail->cause.value.set_radio_network();
    setup_fail->cause.value.radio_network() = asn1::e1ap::cause_radio_network_opts::options::no_radio_res_available;

    // send response
    pdu_notifier.on_new_message(e1_msg);
  }
}

void e1_cu_up_impl::handle_message(const e1_message& msg)
{
  // Run E1AP protocols in CU-UP executor.
  cu_up_exec.execute([this, msg]() {
    logger.info("Handling E1 PDU of type {}", msg.pdu.type().to_string());

    // Log message.
    expected<gnb_cu_up_ue_e1ap_id_t> gnb_cu_up_ue_e1ap_id = get_gnb_cu_up_ue_e1ap_id(msg.pdu);
    expected<uint8_t>                transaction_id       = get_transaction_id(msg.pdu);
    if (transaction_id.has_value()) {
      logger.info("E1AP SDU, \"{}.{}\", transaction id={}",
                  msg.pdu.type().to_string(),
                  get_message_type_str(msg.pdu),
                  transaction_id.value());
    } else if (gnb_cu_up_ue_e1ap_id.has_value()) {
      logger.info("E1AP SDU, \"{}.{}\", GNB-CU-UP-UE-E1AP-ID={}",
                  msg.pdu.type().to_string(),
                  get_message_type_str(msg.pdu),
                  gnb_cu_up_ue_e1ap_id.value());
    } else {
      logger.info("E1AP SDU, \"{}.{}\"", msg.pdu.type().to_string(), get_message_type_str(msg.pdu));
    }

    switch (msg.pdu.type().value) {
      case asn1::e1ap::e1ap_pdu_c::types_opts::init_msg:
        handle_initiating_message(msg.pdu.init_msg());
        break;
      case asn1::e1ap::e1ap_pdu_c::types_opts::successful_outcome:
        handle_successful_outcome(msg.pdu.successful_outcome());
        break;
      case asn1::e1ap::e1ap_pdu_c::types_opts::unsuccessful_outcome:
        handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
        break;
      default:
        logger.error("Invalid E1 PDU type");
        break;
    }
  });
}

void e1_cu_up_impl::handle_initiating_message(const asn1::e1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::gnb_cu_cp_e1_setup_request: {
      current_transaction_id = msg.value.gnb_cu_cp_e1_setup_request()->transaction_id.value;
      handle_cu_cp_e1_setup_request(msg.value.gnb_cu_cp_e1_setup_request());
    } break;
    case asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::bearer_context_setup_request: {
      handle_bearer_context_setup_request(msg.value.bearer_context_setup_request());
    } break;
    case asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::bearer_context_mod_request: {
      handle_bearer_context_modification_request(msg.value.bearer_context_mod_request());
    } break;
    case asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::bearer_context_release_cmd: {
      handle_bearer_context_release_command(msg.value.bearer_context_release_cmd());
    } break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void e1_cu_up_impl::handle_cu_cp_e1_setup_request(const asn1::e1ap::gnb_cu_cp_e1_setup_request_s& msg)
{
  cu_cp_e1_setup_request req_msg = {};
  req_msg.request                = msg;
  e1_cu_up_notifier.on_cu_cp_e1_setup_request_received(req_msg);
}

void e1_cu_up_impl::handle_bearer_context_setup_request(const asn1::e1ap::bearer_context_setup_request_s& msg)
{
  // create failure message for early returns
  e1_message e1_msg;
  e1_msg.pdu.set_unsuccessful_outcome();
  e1_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_setup_fail()->gnb_cu_cp_ue_e1ap_id = msg->gnb_cu_cp_ue_e1ap_id;
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_setup_fail()->cause.value.set_protocol();

  // We only support NG-RAN Bearer
  if (msg->sys_bearer_context_setup_request.value.type() !=
      asn1::e1ap::sys_bearer_context_setup_request_c::types::ng_ran_bearer_context_setup_request) {
    logger.error("Not handling E-UTRAN Bearers");

    // send response
    logger.info("Transmitting BearerContextSetupFailure message");
    pdu_notifier.on_new_message(e1_msg);
    return;
  }

  logger.info("Received Bearer Context Setup Request plmn={}", plmn_bcd_to_string(msg->serving_plmn.value.to_number()));

  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = ue_ctxt_list.next_gnb_cu_up_ue_e1ap_id();
  if (cu_up_ue_e1ap_id == gnb_cu_up_ue_e1ap_id_t::invalid) {
    logger.error("No CU-UP UE E1 ID available.");

    // send response
    logger.info("Transmitting BearerContextSetupFailure message");
    pdu_notifier.on_new_message(e1_msg);
    return;
  }

  // Add gnb_cu_up_ue_e1ap_id to failure message
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_setup_fail()->gnb_cu_up_ue_e1ap_id.value =
      gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);

  // Forward message to CU-UP
  e1ap_bearer_context_setup_request e1_bearer_context_setup = {};
  e1_bearer_context_setup.serving_plmn                      = msg->serving_plmn.value;
  e1_bearer_context_setup.request                           = msg->sys_bearer_context_setup_request.value;
  e1ap_bearer_context_setup_response ue_context_setup_response_msg =
      e1_cu_up_notifier.on_bearer_context_setup_request_received(e1_bearer_context_setup);

  if (ue_context_setup_response_msg.ue_index == INVALID_UE_INDEX) {
    logger.error("Invalid UE index.");

    // send response
    logger.info("Transmitting BearerContextSetupFailure message");
    pdu_notifier.on_new_message(e1_msg);
    return;
  }

  // Create UE context and store it
  ue_ctxt_list.add_ue(ue_context_setup_response_msg.ue_index,
                      cu_up_ue_e1ap_id,
                      int_to_gnb_cu_cp_ue_e1ap_id(msg->gnb_cu_cp_ue_e1ap_id.value));
  e1ap_ue_context& ue_ctxt = ue_ctxt_list[cu_up_ue_e1ap_id];

  logger.debug("Added UE context (gnb_cu_up_ue_e1ap_id={}, gnb_cu_cp_e1ap_ue_id={}, ue_index={}).",
               ue_ctxt.cu_up_ue_e1ap_id,
               ue_ctxt.cu_cp_ue_e1ap_id,
               ue_ctxt.ue_index);

  if (ue_context_setup_response_msg.success) {
    e1_msg.pdu.set_successful_outcome();
    e1_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
    e1_msg.pdu.successful_outcome().value.bearer_context_setup_resp()->gnb_cu_cp_ue_e1ap_id = msg->gnb_cu_cp_ue_e1ap_id;
    e1_msg.pdu.successful_outcome().value.bearer_context_setup_resp()->gnb_cu_up_ue_e1ap_id.value =
        gnb_cu_up_ue_e1ap_id_to_uint(cu_up_ue_e1ap_id);
    e1_msg.pdu.successful_outcome().value.bearer_context_setup_resp()->sys_bearer_context_setup_resp.value =
        ue_context_setup_response_msg.sys_bearer_context_setup_resp;

    // send response
    logger.info("Transmitting BearerContextSetupResponse message");
    pdu_notifier.on_new_message(e1_msg);
  } else {
    e1_msg.pdu.unsuccessful_outcome().value.bearer_context_setup_fail()->cause.value =
        ue_context_setup_response_msg.cause;

    // send response
    logger.info("Transmitting BearerContextSetupFailure message");
    pdu_notifier.on_new_message(e1_msg);
  }
}

void e1_cu_up_impl::handle_bearer_context_modification_request(const asn1::e1ap::bearer_context_mod_request_s& msg)
{
  // create failure message for early returns
  e1_message e1_msg;
  e1_msg.pdu.set_unsuccessful_outcome();
  e1_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_cp_ue_e1ap_id = msg->gnb_cu_cp_ue_e1ap_id;
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->gnb_cu_up_ue_e1ap_id = msg->gnb_cu_up_ue_e1ap_id;
  e1_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause.value.set_protocol();

  e1ap_bearer_context_modification_request e1_bearer_context_mod = {};

  e1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_up_ue_e1ap_id(msg->gnb_cu_up_ue_e1ap_id.value)];
  if (ue_ctxt.cu_up_ue_e1ap_id == gnb_cu_up_ue_e1ap_id_t::invalid) {
    logger.error("No UE context for the received gnb_cu_up_ue_e1ap_id={} available.", msg->gnb_cu_up_ue_e1ap_id.value);

    // send response
    logger.info("Transmitting BearerContextModificationFailure message");
    pdu_notifier.on_new_message(e1_msg);
    return;
  }

  // sys bearer context mod request
  if (msg->sys_bearer_context_mod_request_present) {
    // We only support NG-RAN Bearer
    if (msg->sys_bearer_context_mod_request.value.type() !=
        asn1::e1ap::sys_bearer_context_mod_request_c::types::ng_ran_bearer_context_mod_request) {
      logger.error("Not handling E-UTRAN Bearers");

      // send response
      logger.info("Transmitting BearerContextModificationFailure message");
      pdu_notifier.on_new_message(e1_msg);
      return;
    }

    e1_bearer_context_mod.ue_index = ue_ctxt.ue_index;
    e1_bearer_context_mod.request  = msg->sys_bearer_context_mod_request.value;
  }

  // Forward message to CU-UP
  e1ap_bearer_context_modification_response ue_context_mod_response_msg =
      e1_cu_up_notifier.on_bearer_context_modification_request_received(e1_bearer_context_mod);

  if (ue_context_mod_response_msg.ue_index == INVALID_UE_INDEX) {
    logger.error("Invalid UE index.");

    // send response
    logger.info("Transmitting BearerContextModificationFailure message");
    pdu_notifier.on_new_message(e1_msg);
    return;
  }

  if (ue_context_mod_response_msg.success) {
    e1_msg.pdu.set_successful_outcome();
    e1_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);
    e1_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_cp_ue_e1ap_id = msg->gnb_cu_cp_ue_e1ap_id;
    e1_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->gnb_cu_up_ue_e1ap_id = msg->gnb_cu_up_ue_e1ap_id;
    e1_msg.pdu.successful_outcome().value.bearer_context_mod_resp()->sys_bearer_context_mod_resp.value =
        ue_context_mod_response_msg.sys_bearer_context_modification_resp;

    // send response
    logger.info("Transmitting BearerContextModificationResponse message");
    pdu_notifier.on_new_message(e1_msg);
  } else {
    e1_msg.pdu.unsuccessful_outcome().value.bearer_context_mod_fail()->cause.value = ue_context_mod_response_msg.cause;

    // send response
    logger.info("Transmitting BearerContextModificationFailure message");
    pdu_notifier.on_new_message(e1_msg);
  }
}

void e1_cu_up_impl::handle_bearer_context_release_command(const asn1::e1ap::bearer_context_release_cmd_s& msg)
{
  e1ap_bearer_context_release_command e1_bearer_context_release_cmd = {};

  e1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_up_ue_e1ap_id(msg->gnb_cu_up_ue_e1ap_id.value)];
  if (ue_ctxt.cu_up_ue_e1ap_id == gnb_cu_up_ue_e1ap_id_t::invalid) {
    logger.error("No UE context for the received gnb_cu_up_ue_e1ap_id={} available.", msg->gnb_cu_up_ue_e1ap_id.value);
    return;
  }

  e1_bearer_context_release_cmd.ue_index = ue_ctxt.ue_index;
  e1_bearer_context_release_cmd.cause    = msg->cause.value;

  // Forward message to CU-UP
  e1_cu_up_notifier.on_bearer_context_release_command_received(e1_bearer_context_release_cmd);

  // Remove UE context
  ue_ctxt_list.remove_ue(ue_ctxt.ue_index);

  e1_message e1_msg;
  logger.info("Transmitting BearerContextReleaseComplete message");

  e1_msg.pdu.set_successful_outcome();
  e1_msg.pdu.successful_outcome().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  e1_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_cp_ue_e1ap_id =
      msg->gnb_cu_cp_ue_e1ap_id;
  e1_msg.pdu.successful_outcome().value.bearer_context_release_complete()->gnb_cu_up_ue_e1ap_id =
      msg->gnb_cu_up_ue_e1ap_id;

  // send response
  pdu_notifier.on_new_message(e1_msg);
}

void e1_cu_up_impl::handle_successful_outcome(const asn1::e1ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    default:
      // Handle successful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }
  }
}

void e1_cu_up_impl::handle_unsuccessful_outcome(const asn1::e1ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    default:
      // Handle unsuccessful outcomes with transaction id
      expected<uint8_t> transaction_id = get_transaction_id(outcome);
      if (transaction_id.is_error()) {
        logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
        return;
      }
  }
}
