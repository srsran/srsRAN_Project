/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_cu_impl.h"
#include "../../ran/gnb_format.h"
#include "../lib/f1_interface/common/asn1_helpers.h"
#include "srsgnb/asn1/f1ap.h"

using namespace srsgnb;
using namespace asn1::f1ap;
using namespace srs_cu_cp;

f1ap_cu_impl::f1ap_cu_impl(f1c_message_notifier&       f1c_pdu_notifier_,
                           f1c_du_processor_notifier&  f1c_du_processor_notifier_,
                           f1c_du_management_notifier& f1c_du_management_notifier_) :
  logger(srslog::fetch_basic_logger("CU-F1AP")),
  pdu_notifier(f1c_pdu_notifier_),
  du_processor_notifier(f1c_du_processor_notifier_),
  du_management_notifier(f1c_du_management_notifier_),
  events(std::make_unique<f1ap_event_manager>())
{
  f1ap_ue_context empty_context = {};
  std::fill(cu_ue_id_to_f1ap_ue_context.begin(), cu_ue_id_to_f1ap_ue_context.end(), empty_context);
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_cu_impl::~f1ap_cu_impl() {}

void f1ap_cu_impl::connect_srb_notifier(ue_index_t ue_index, srb_id_t srb_id, f1c_rrc_message_notifier& notifier)
{
  gnb_cu_ue_f1ap_id_t cu_ue_id = find_cu_ue_id(ue_index);

  f1ap_ue_context& ue_ctxt = cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)];

  ue_ctxt.srbs[srb_id_to_uint(srb_id)] = &notifier;
}

void f1ap_cu_impl::handle_f1ap_setup_response(const f1_setup_response_message& msg)
{
  // Pack message into PDU
  f1c_message f1c_msg;
  if (msg.success) {
    // TODO send response
    logger.info("Transmitting F1SetupResponse message");

    f1c_msg.pdu.set_successful_outcome();
    f1c_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    f1c_msg.pdu.successful_outcome().value.f1_setup_resp() = msg.response;

    // set values handled by F1
    f1c_msg.pdu.successful_outcome().value.f1_setup_resp()->transaction_id.value = 99;

    // send response
    pdu_notifier.on_new_message(f1c_msg);
  } else {
    logger.info("Transmitting F1SetupFailure message");
    f1c_msg.pdu.set_unsuccessful_outcome();
    f1c_msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    f1c_msg.pdu.unsuccessful_outcome().value.f1_setup_fail() = msg.failure;
    auto& setup_fail                                         = f1c_msg.pdu.unsuccessful_outcome().value.f1_setup_fail();

    // set values handled by F1
    setup_fail->transaction_id.value = 99;
    setup_fail->cause.value.set_radio_network();
    setup_fail->cause.value.radio_network() = asn1::f1ap::cause_radio_network_opts::options::no_radio_res_available;

    // send response
    pdu_notifier.on_new_message(f1c_msg);

    // send DU remove request
    du_index_t du_index = du_processor_notifier.get_du_index();
    du_management_notifier.on_du_remove_request_received(du_index);
  }
}

void f1ap_cu_impl::handle_dl_rrc_message_transfer(const f1ap_dl_rrc_message& msg)
{
  gnb_cu_ue_f1ap_id_t cu_ue_id = find_cu_ue_id(msg.ue_index);
  f1ap_ue_context     ue_ctxt  = cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)];

  asn1::f1ap::dlrrc_msg_transfer_s dlrrc_msg = {};
  dlrrc_msg->gnb_cu_ue_f1_ap_id.value        = gnb_cu_ue_f1ap_id_to_uint(cu_ue_id);
  dlrrc_msg->gnb_du_ue_f1_ap_id.value        = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id);
  dlrrc_msg->srbid.value                     = (uint8_t)msg.srb_id;
  dlrrc_msg->rrc_container.value             = msg.rrc_container;

  logger.info("Transmitting DL RRC message");
  // Pack message into PDU
  f1c_message f1c_dl_rrc_msg;
  f1c_dl_rrc_msg.pdu.set_init_msg();
  f1c_dl_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_DLRRC_MSG_TRANSFER);
  f1c_dl_rrc_msg.pdu.init_msg().value.dlrrc_msg_transfer() = dlrrc_msg;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1c_dl_rrc_msg.pdu.to_json(js);
    logger.debug("Containerized DL RRC message: {}", js.to_string());
  }

  // send DL RRC message
  pdu_notifier.on_new_message(f1c_dl_rrc_msg);
}

async_task<f1ap_ue_context_setup_response_message>
f1ap_cu_impl::handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request)
{
  return launch_async<f1ap_ue_context_setup_procedure>(request.msg, pdu_notifier, *events, logger);
}

async_task<ue_index_t>
f1ap_cu_impl::handle_ue_context_release_command(const f1ap_ue_context_release_command_message& msg)
{
  gnb_cu_ue_f1ap_id_t cu_ue_id = find_cu_ue_id(msg.ue_index);

  if (cu_ue_id == gnb_cu_ue_f1ap_id_t::invalid) {
    logger.error("Can't find UE to release (ue_index={})", msg.ue_index);

    return launch_async([](coro_context<async_task<ue_index_t>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(INVALID_UE_INDEX);
    });
  }

  f1ap_ue_context& ue_ctxt = cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)];

  asn1::f1ap::ue_context_release_cmd_s ue_ctxt_rel_cmd = {};
  ue_ctxt_rel_cmd->gnb_cu_ue_f1_ap_id.value            = gnb_cu_ue_f1ap_id_to_uint(cu_ue_id);
  ue_ctxt_rel_cmd->gnb_du_ue_f1_ap_id.value            = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id);
  ue_ctxt_rel_cmd->cause.value                         = msg.cause;

  return launch_async<f1ap_ue_context_release_procedure>(ue_ctxt, ue_ctxt_rel_cmd, pdu_notifier, *events, logger);
}

async_task<f1ap_ue_context_modification_response_message>
f1ap_cu_impl::handle_ue_context_modification_request(const f1ap_ue_context_modification_request_message& request)
{
  return launch_async<f1ap_ue_context_modification_procedure>(request.msg, pdu_notifier, *events, logger);
}

void f1ap_cu_impl::handle_message(const f1c_message& msg)
{
  logger.info("Handling F1AP PDU of type {}", msg.pdu.type().to_string());

  switch (msg.pdu.type().value) {
    case asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    case asn1::f1ap::f1_ap_pdu_c::types_opts::successful_outcome:
      handle_successful_outcome(msg.pdu.successful_outcome());
      break;
    case asn1::f1ap::f1_ap_pdu_c::types_opts::unsuccessful_outcome:
      handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
      break;
    default:
      logger.error("Invalid F1AP PDU type");
      break;
  }
}

int f1ap_cu_impl::get_nof_ues()
{
  int nof_ues = 0;
  for (auto ue_context : cu_ue_id_to_f1ap_ue_context) {
    if (ue_context.ue_index != INVALID_UE_INDEX) {
      nof_ues++;
    }
  }
  return nof_ues;
}

void f1ap_cu_impl::handle_initiating_message(const asn1::f1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::options::f1_setup_request: {
      f1_setup_request_message req_msg = {};
      req_msg.request                  = msg.value.f1_setup_request();
      du_processor_notifier.on_f1_setup_request_received(req_msg);
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::init_ulrrc_msg_transfer: {
      handle_initial_ul_rrc_message(msg.value.init_ulrrc_msg_transfer());
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ulrrc_msg_transfer: {
      handle_ul_rrc_message(msg.value.ulrrc_msg_transfer());
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request: {
      handle_f1_removal_request(msg.value.f1_removal_request());
    } break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_initial_ul_rrc_message(const init_ulrrc_msg_transfer_s& msg)
{
  // Reject request without served cells
  if (not msg->duto_currc_container_present) {
    logger.error("Not handling Initial UL RRC message transfer without DU to CU container");
    /// Assume the DU can't serve the UE. Ignoring the message.
    return;
  }

  nr_cell_global_identity cgi = cgi_from_asn1(msg->nrcgi.value);

  logger.info("Received Initial UL RRC message transfer nr_cgi={}, crnti={}", cgi.nci.packed, msg->c_rnti.value);
  logger.debug("plmn={}", cgi.plmn);

  if (msg->sul_access_ind_present) {
    logger.debug("Ignoring SUL access indicator");
  }

  gnb_cu_ue_f1ap_id_t cu_ue_id = get_next_cu_ue_id();
  if (cu_ue_id == gnb_cu_ue_f1ap_id_t::invalid) {
    logger.error("No CU UE F1AP ID available.");
    return;
  }

  // Request UE creation
  f1ap_initial_ul_rrc_message f1ap_init_ul_rrc_msg      = {};
  f1ap_init_ul_rrc_msg.msg                              = msg;
  ue_creation_complete_message ue_creation_complete_msg = du_processor_notifier.on_create_ue(f1ap_init_ul_rrc_msg);

  if (ue_creation_complete_msg.ue_index == INVALID_UE_INDEX) {
    logger.error("Invalid UE index.");
    return;
  }

  // Create UE context and store it
  f1ap_ue_context ue_ctxt = {};
  ue_ctxt.du_ue_f1ap_id   = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1_ap_id.value);
  ue_ctxt.ue_index        = ue_creation_complete_msg.ue_index;
  ue_ctxt.srbs            = ue_creation_complete_msg.srbs;
  cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)] = ue_ctxt;

  logger.debug(
      "Added UE (cu_ue_f1ap_id={}, du_ue_f1ap_id={}, ue_index={}).", cu_ue_id, ue_ctxt.du_ue_f1ap_id, ue_ctxt.ue_index);

  // Forward RRC container
  if (msg->rrc_container_rrc_setup_complete_present) {
    // RRC setup complete over SRB1
    cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)]
        .srbs[srb_id_to_uint(srb_id_t::srb1)]
        ->on_new_rrc_message(msg->rrc_container_rrc_setup_complete.value);
    return;
  }

  // Pass container to RRC
  cu_ue_id_to_f1ap_ue_context[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)]
      .srbs[srb_id_to_uint(srb_id_t::srb0)]
      ->on_new_rrc_message(msg->rrc_container.value);
}

void f1ap_cu_impl::handle_ul_rrc_message(const ulrrc_msg_transfer_s& msg)
{
  f1ap_ue_context ue_ctxt = cu_ue_id_to_f1ap_ue_context[msg->gnb_cu_ue_f1_ap_id.value];

  // Notify upper layers about reception
  ue_ctxt.srbs[msg->srbid.value]->on_new_rrc_message(msg->rrc_container.value);
}

void f1ap_cu_impl::handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete: {
      events->f1ap_ue_context_release_complete.set(&outcome.value.ue_context_release_complete());
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_setup_resp: {
      events->f1ap_ue_context_setup_response_message.set(&outcome.value.ue_context_setup_resp());
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp: {
      events->f1ap_ue_context_modification_response_message.set(&outcome.value.ue_context_mod_resp());
    } break;
    default:
      logger.error("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::f1ap::f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ue_context_setup_fail: {
      events->f1ap_ue_context_setup_response_message.set(&outcome.value.ue_context_setup_fail());
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ue_context_mod_fail: {
      events->f1ap_ue_context_modification_response_message.set(&outcome.value.ue_context_mod_fail());
    } break;
    default:
      logger.error("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_f1_removal_request(const asn1::f1ap::f1_removal_request_s& msg)
{
  du_index_t du_index = du_processor_notifier.get_du_index();
  du_management_notifier.on_du_remove_request_received(du_index);
}

gnb_cu_ue_f1ap_id_t f1ap_cu_impl::get_next_cu_ue_id()
{
  for (int cu_ue_id = MIN_UE_INDEX; cu_ue_id < MAX_NOF_UES; cu_ue_id++) {
    if (cu_ue_id_to_f1ap_ue_context[cu_ue_id].ue_index == INVALID_UE_INDEX) {
      return int_to_gnb_cu_ue_f1ap_id(cu_ue_id);
    }
  }
  logger.error("No CU UE ID available");
  return gnb_cu_ue_f1ap_id_t::invalid;
}

gnb_cu_ue_f1ap_id_t f1ap_cu_impl::find_cu_ue_id(ue_index_t ue_index)
{
  for (int cu_ue_id = MIN_UE_INDEX; cu_ue_id < MAX_NOF_UES; cu_ue_id++) {
    if (cu_ue_id_to_f1ap_ue_context[cu_ue_id].ue_index == ue_index) {
      return int_to_gnb_cu_ue_f1ap_id(cu_ue_id);
    }
  }
  logger.error("CU UE ID for ue_index={} not found", ue_index);
  return gnb_cu_ue_f1ap_id_t::invalid;
}
