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
#include "srsgnb/support/async/event_signal.h"

using namespace srsgnb;
using namespace asn1::f1ap;
using namespace srs_cu_cp;

class srsgnb::srs_cu_cp::f1ap_cu_impl::f1ap_event_manager
{
public:
  /// F1 Context Release Complete
  // FIXME handle static return value (only returns f1ap_ue_context_release_complete)
  using f1ap_ue_context_release_outcome_t =
      expected<const ue_context_release_complete_s*, const ue_context_setup_fail_s*>;
  event_signal<f1ap_ue_context_release_outcome_t> f1ap_ue_context_release_complete;

  /// F1 UE context setup procedure outcome.
  using f1ap_ue_context_setup_outcome_t = expected<const ue_context_setup_resp_s*, const ue_context_setup_fail_s*>;
  event_signal<f1ap_ue_context_setup_outcome_t> f1ap_ue_context_setup_response;

  /// F1 UE Context Modification procedure outcome.
  using f1ap_ue_context_modification_outcome_t = expected<const ue_context_mod_resp_s*, const ue_context_mod_fail_s*>;
  event_signal<f1ap_ue_context_modification_outcome_t> f1ap_ue_context_modification_response_message;
};

f1ap_cu_impl::f1ap_cu_impl(f1c_message_notifier&              f1c_pdu_notifier_,
                           f1c_du_processor_message_notifier& du_processor_notifier_,
                           f1c_ue_manager_message_notifier&   ue_manager_notifier_,
                           f1c_du_management_notifier&        f1c_du_management_notifier_) :
  logger(srslog::fetch_basic_logger("CU-F1AP")),
  pdu_notifier(f1c_pdu_notifier_),
  du_processor_notifier(du_processor_notifier_),
  ue_manager_notifier(ue_manager_notifier_),
  du_management_notifier(f1c_du_management_notifier_)
{
  f1ap_ue_context empty_context = {};
  empty_context.du_ue_f1ap_id   = INVALID_F1AP_UE_ID;
  empty_context.ue_index        = INVALID_UE_INDEX;
  std::fill(cu_ue_id_to_f1ap_ue_context.begin(), cu_ue_id_to_f1ap_ue_context.end(), empty_context);
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_cu_impl::~f1ap_cu_impl() {}

void f1ap_cu_impl::handle_f1ap_setup_response(const f1_setup_response_message& msg)
{
  // Pack message into PDU
  f1c_msg f1c_msg;
  if (msg.success) {
    // TODO send response
    logger.info("Transmitting F1SetupResponse message");

    f1c_msg.pdu.set_successful_outcome();
    f1c_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
    f1c_msg.pdu.successful_outcome().value.f1_setup_resp() = msg.response;

    // set values handled by F1
    f1c_msg.pdu.successful_outcome().value.f1_setup_resp()->transaction_id.value = 99;
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
  }

  // send response
  pdu_notifier.on_new_message(f1c_msg);
}

void f1ap_cu_impl::handle_dl_rrc_message_transfer(const f1ap_dl_rrc_msg& msg)
{
  logger.info("Transmitting DL RRC message");
  // Pack message into PDU
  f1c_msg f1c_msg;
  f1c_msg.pdu.set_init_msg();
  f1c_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_DLRRC_MSG_TRANSFER);
  f1c_msg.pdu.init_msg().value.dlrrc_msg_transfer() = msg.msg;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1c_msg.pdu.to_json(js);
    logger.debug("Containerized DL RRC message: {}", js.to_string());
  }

  // send DL RRC message
  pdu_notifier.on_new_message(f1c_msg);
}

async_task<f1ap_ue_context_setup_response_message>
f1ap_cu_impl::handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request)
{
  f1ap_event_manager::f1ap_ue_context_setup_outcome_t f1_ue_ctx_setup_resp;

  // TODO: send msg

  return launch_async([this, f1_ue_ctx_setup_resp, res = f1ap_ue_context_setup_response_message{}, request](
                          coro_context<async_task<f1ap_ue_context_setup_response_message>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(f1_ue_ctx_setup_resp, events->f1ap_ue_context_setup_response);

    if (f1_ue_ctx_setup_resp.has_value()) {
      logger.info("Received F1AP PDU with successful outcome.");
      res.msg     = *f1_ue_ctx_setup_resp.value();
      res.success = true;
    } else {
      logger.info("Received F1AP PDU with unsuccessful outcome.");
      res.success = false;
    }

    CORO_RETURN(res);
  });
}

async_task<f1ap_ue_context_release_complete_message>
f1ap_cu_impl::handle_ue_context_release(const f1ap_ue_context_release_command_message& msg)
{
  // TODO: send msg

  f1ap_event_manager::f1ap_ue_context_release_outcome_t f1ap_ue_ctxt_rel_complete;

  return launch_async([this, f1ap_ue_ctxt_rel_complete, res = f1ap_ue_context_release_complete_message{}, msg](
                          coro_context<async_task<f1ap_ue_context_release_complete_message>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(f1ap_ue_ctxt_rel_complete, events->f1ap_ue_context_release_complete);
    res.msg = *f1ap_ue_ctxt_rel_complete.value();

    CORO_RETURN(res);
  });
}

async_task<f1ap_ue_context_modification_response_message>
f1ap_cu_impl::handle_ue_context_modification(const f1ap_ue_context_modification_request_message& request)
{
  f1ap_event_manager::f1ap_ue_context_modification_outcome_t f1ap_ue_ctx_mod_resp;

  // TODO: send msg

  return launch_async([this, f1ap_ue_ctx_mod_resp, res = f1ap_ue_context_modification_response_message{}, request](
                          coro_context<async_task<f1ap_ue_context_modification_response_message>>& ctx) mutable {
    CORO_BEGIN(ctx);

    CORO_AWAIT_VALUE(f1ap_ue_ctx_mod_resp, events->f1ap_ue_context_modification_response_message);

    if (f1ap_ue_ctx_mod_resp.has_value()) {
      logger.info("Received F1AP PDU with successful outcome.");
      res.response = *f1ap_ue_ctx_mod_resp.value();
      res.success  = true;
    } else {
      logger.info("Received F1AP PDU with unsuccessful outcome.");
      res.failure = *f1ap_ue_ctx_mod_resp.error();
      res.success = false;
    }

    CORO_RETURN(res);
  });
}

void f1ap_cu_impl::handle_message(const f1c_msg& msg)
{
  logger.info("Handling F1AP PDU of type {}", msg.pdu.type().to_string());

  switch (msg.pdu.type().value) {
    case asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg:
      handle_initiating_message(msg.pdu.init_msg());
      break;
    default:
      logger.error("Invalid PDU type");
      break;
  }
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
      f1ap_initial_ul_rrc_msg ul_transfer = {};
      ul_transfer.msg                     = msg.value.init_ulrrc_msg_transfer();
      handle_initial_ul_rrc_message(ul_transfer);
    } break;
    case asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ulrrc_msg_transfer: {
      f1ap_ul_rrc_msg ul_transfer = {};
      ul_transfer.msg             = msg.value.ulrrc_msg_transfer();
      handle_ul_rrc_message(ul_transfer);
    } break;
    default:
      logger.error("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_initial_ul_rrc_message(const f1ap_initial_ul_rrc_msg& msg)
{
  // Reject request without served cells
  if (not msg.msg->duto_currc_container_present) {
    logger.error("Not handling Initial UL RRC message transfer without DU to CU container");
    /// Assume the DU can't serve the UE. Ignoring the message.
    return;
  }

  nr_cell_global_identity cgi = cgi_from_asn1(msg.msg->nrcgi.value);

  logger.info("Received Initial UL RRC message transfer nr_cgi={}, crnti={}", cgi.nci.packed, msg.msg->c_rnti.value);
  logger.debug("plmn={}", cgi.plmn);

  if (msg.msg->sul_access_ind_present) {
    logger.debug("Ignoring SUL access indicator");
  }

  du_cell_index_t pcell_index = du_processor_notifier.find_cell(cgi.nci.packed);
  if (pcell_index == INVALID_DU_CELL_INDEX) {
    logger.error("Could not find cell with cell_id={}", cgi.nci.packed);
    return;
  }

  ue_index_t ue_index = ue_manager_notifier.on_initial_ul_rrc_message_transfer_received(pcell_index, msg);
  if (ue_index == INVALID_UE_INDEX) {
    logger.error("No free UE index found, maximum number of connected UEs reached.");
    return;
  }

  f1ap_ue_id_t cu_ue_id = get_next_cu_ue_id();
  if (cu_ue_id == INVALID_F1AP_UE_ID) {
    logger.error("No CU UE F1AP ID available.");
    return;
  }

  f1ap_ue_context ue_ctx = {};
  ue_ctx.du_ue_f1ap_id   = int_to_f1ap_ue_id(msg.msg->gnb_du_ue_f1_ap_id.value);
  ue_ctx.ue_index        = ue_index;

  logger.debug(
      "Added UE (cu_ue_f1ap_id={}, du_ue_f1ap_id={}, ue_index={}.", cu_ue_id, ue_ctx.du_ue_f1ap_id, ue_ctx.ue_index);

  cu_ue_id_to_f1ap_ue_context[cu_ue_id] = ue_ctx;
}

void f1ap_cu_impl::handle_ul_rrc_message(const f1ap_ul_rrc_msg& msg)
{
  f1ap_ue_context ue_ctx = cu_ue_id_to_f1ap_ue_context[msg.msg->gnb_cu_ue_f1_ap_id.value];

  ue_manager_notifier.on_ul_rrc_message_transfer_received(ue_ctx.ue_index, msg);
}

void f1ap_cu_impl::handle_f1_removal_resquest(const f1_removal_request_message& msg)
{
  // TODO: get real du_index
  du_index_t du_index = int_to_du_index(0);
  du_management_notifier.on_du_remove_request_received(du_index);
}

f1ap_ue_id_t f1ap_cu_impl::get_next_cu_ue_id()
{
  for (int cu_ue_id = 0; cu_ue_id < MAX_NOF_UES; cu_ue_id++) {
    if (cu_ue_id_to_f1ap_ue_context[int_to_f1ap_ue_id(cu_ue_id)].ue_index == INVALID_UE_INDEX) {
      return int_to_f1ap_ue_id(cu_ue_id);
    }
  }
  logger.error("No CU UE ID available");
  return INVALID_F1AP_UE_ID;
}

f1ap_ue_id_t f1ap_cu_impl::find_cu_ue_id(ue_index_t ue_index)
{
  for (int cu_ue_id = 0; cu_ue_id < MAX_NOF_UES; cu_ue_id++) {
    if (cu_ue_id_to_f1ap_ue_context[int_to_f1ap_ue_id(cu_ue_id)].ue_index == ue_index) {
      return int_to_f1ap_ue_id(cu_ue_id);
    }
  }
  logger.error("CU UE ID for ue_index={} not found", ue_index);
  return INVALID_F1AP_UE_ID;
}