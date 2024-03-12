/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "f1ap_cu_impl.h"
#include "../common/asn1_helpers.h"
#include "f1ap_asn1_helpers.h"
#include "procedures/f1_setup_procedure.h"
#include "procedures/ue_context_modification_procedure.h"
#include "procedures/ue_context_release_procedure.h"
#include "procedures/ue_context_setup_procedure.h"
#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/nr_cgi_helpers.h"

using namespace srsran;
using namespace asn1::f1ap;
using namespace srs_cu_cp;

f1ap_cu_impl::f1ap_cu_impl(f1ap_message_notifier&       f1ap_pdu_notifier_,
                           f1ap_du_processor_notifier&  f1ap_du_processor_notifier_,
                           f1ap_du_management_notifier& f1ap_du_management_notifier_,
                           f1ap_ue_removal_notifier&    f1ap_cu_cp_notifier_,
                           timer_manager&               timers_,
                           task_executor&               ctrl_exec_) :
  logger(srslog::fetch_basic_logger("CU-CP-F1")),
  ue_ctxt_list(timer_factory{timers_, ctrl_exec_}, logger),
  pdu_notifier(f1ap_pdu_notifier_),
  du_processor_notifier(f1ap_du_processor_notifier_),
  du_management_notifier(f1ap_du_management_notifier_),
  ctrl_exec(ctrl_exec_)
{
}

// Note: For fwd declaration of member types, dtor cannot be trivial.
f1ap_cu_impl::~f1ap_cu_impl() {}

void f1ap_cu_impl::handle_dl_rrc_message_transfer(const f1ap_dl_rrc_message& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping DlRrcMessageTransfer. UE context does not exist", msg.ue_index);
    return;
  }

  f1ap_ue_context& ue_ctxt = ue_ctxt_list[msg.ue_index];

  asn1::f1ap::dl_rrc_msg_transfer_s dl_rrc_msg = {};
  dl_rrc_msg->gnb_cu_ue_f1ap_id                = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.cu_ue_f1ap_id);
  dl_rrc_msg->gnb_du_ue_f1ap_id                = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.du_ue_f1ap_id);
  dl_rrc_msg->srb_id                           = (uint8_t)msg.srb_id;
  dl_rrc_msg->rrc_container                    = msg.rrc_container.copy();

  if (ue_ctxt.pending_old_ue_id.has_value()) {
    // if the UE requests to reestablish RRC connection in the last serving gNB-DU, the DL RRC MESSAGE TRANSFER message
    // shall include old gNB-DU UE F1AP ID, see TS 38.401 section 8.7.
    dl_rrc_msg->old_gnb_du_ue_f1ap_id_present = true;
    dl_rrc_msg->old_gnb_du_ue_f1ap_id         = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.pending_old_ue_id.value());

    ue_ctxt.pending_old_ue_id.reset();
  }

  // Pack message into PDU
  f1ap_message f1ap_dl_rrc_msg;
  f1ap_dl_rrc_msg.pdu.set_init_msg();
  f1ap_dl_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  f1ap_dl_rrc_msg.pdu.init_msg().value.dl_rrc_msg_transfer() = dl_rrc_msg;

  // send DL RRC message
  ue_ctxt.logger.log_debug("Sending DlRrcMessageTransfer");
  if (ue_ctxt.logger.get_basic_logger().debug.enabled()) {
    asn1::json_writer js;
    f1ap_dl_rrc_msg.pdu.to_json(js);
    ue_ctxt.logger.log_debug("Containerized DlRrcMessageTransfer: {}", js.to_string());
  }
  pdu_notifier.on_new_message(f1ap_dl_rrc_msg);
}

async_task<f1ap_ue_context_setup_response>
f1ap_cu_impl::handle_ue_context_setup_request(const f1ap_ue_context_setup_request& request,
                                              optional<rrc_ue_transfer_context>    rrc_context)
{
  return launch_async<ue_context_setup_procedure>(
      request, ue_ctxt_list, du_processor_notifier, pdu_notifier, logger, rrc_context);
}

async_task<ue_index_t> f1ap_cu_impl::handle_ue_context_release_command(const f1ap_ue_context_release_command& msg)
{
  if (!ue_ctxt_list.contains(msg.ue_index)) {
    logger.warning("ue={}: Dropping UeContextReleaseCommand. UE context does not exist", msg.ue_index);

    return launch_async([](coro_context<async_task<ue_index_t>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(ue_index_t::invalid);
    });
  }

  return launch_async<ue_context_release_procedure>(msg, ue_ctxt_list[msg.ue_index], pdu_notifier);
}

async_task<f1ap_ue_context_modification_response>
f1ap_cu_impl::handle_ue_context_modification_request(const f1ap_ue_context_modification_request& request)
{
  if (!ue_ctxt_list.contains(request.ue_index)) {
    logger.warning("ue={}: Dropping UeContextModificationRequest. UE context does not exist", request.ue_index);

    return launch_async([](coro_context<async_task<f1ap_ue_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(f1ap_ue_context_modification_response{});
    });
  }

  return launch_async<ue_context_modification_procedure>(request, ue_ctxt_list[request.ue_index], pdu_notifier);
}

bool f1ap_cu_impl::handle_ue_id_update(ue_index_t ue_index, ue_index_t old_ue_index)
{
  if (!ue_ctxt_list.contains(ue_index) or !ue_ctxt_list.contains(old_ue_index)) {
    return false;
  }

  // Mark that an old gNB-DU UE F1AP ID needs to be sent to the DU in the next DL RRC Message Transfer.
  ue_ctxt_list[ue_index].pending_old_ue_id = ue_ctxt_list[old_ue_index].ue_ids.du_ue_f1ap_id;
  return true;
}

void f1ap_cu_impl::handle_paging(const cu_cp_paging_message& msg)
{
  asn1::f1ap::paging_s paging = {};

  // Pack message into PDU
  f1ap_message paging_msg;
  paging_msg.pdu.set_init_msg();
  paging_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_PAGING);

  fill_asn1_paging_message(paging_msg.pdu.init_msg().value.paging(), msg);

  // send DL RRC message
  logger.debug("Sending Paging");
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    paging_msg.pdu.to_json(js);
    logger.debug("Containerized Paging: {}", js.to_string());
  }
  pdu_notifier.on_new_message(paging_msg);
}

void f1ap_cu_impl::handle_message(const f1ap_message& msg)
{
  logger.debug("Handling PDU of type {}", msg.pdu.type().to_string());
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    msg.pdu.to_json(js);
    logger.debug("Containerized PDU: {}", js.to_string());
  }

  // Run F1AP protocols in Control executor.
  if (not ctrl_exec.execute([this, msg]() {
        switch (msg.pdu.type().value) {
          case asn1::f1ap::f1ap_pdu_c::types_opts::init_msg:
            handle_initiating_message(msg.pdu.init_msg());
            break;
          case asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome:
            handle_successful_outcome(msg.pdu.successful_outcome());
            break;
          case asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome:
            handle_unsuccessful_outcome(msg.pdu.unsuccessful_outcome());
            break;
          default:
            logger.warning("Invalid PDU type");
            break;
        }
      })) {
    logger.warning("Discarding F1AP PDU. Cause: CU-CP task queue is full");
  }
}

void f1ap_cu_impl::remove_ue_context(ue_index_t ue_index)
{
  if (!ue_ctxt_list.contains(ue_index)) {
    logger.debug("ue={}: UE context not found", ue_index);
    return;
  }

  ue_ctxt_list.remove_ue(ue_index);
}

void f1ap_cu_impl::handle_initiating_message(const asn1::f1ap::init_msg_s& msg)
{
  switch (msg.value.type().value) {
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::options::f1_setup_request: {
      handle_f1_setup_request(msg.value.f1_setup_request());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::init_ul_rrc_msg_transfer: {
      handle_initial_ul_rrc_message(msg.value.init_ul_rrc_msg_transfer());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ul_rrc_msg_transfer: {
      handle_ul_rrc_message(msg.value.ul_rrc_msg_transfer());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request: {
      handle_f1_removal_request(msg.value.f1_removal_request());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::options::ue_context_release_request: {
      handle_ue_context_release_request(msg.value.ue_context_release_request());
    } break;
    default:
      logger.warning("Initiating message of type {} is not supported", msg.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_f1_setup_request(const f1_setup_request_s& request)
{
  current_transaction_id = request->transaction_id;

  handle_f1_setup_procedure(request, pdu_notifier, du_processor_notifier, logger);
}

void f1ap_cu_impl::handle_initial_ul_rrc_message(const init_ul_rrc_msg_transfer_s& msg)
{
  const gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(msg->gnb_du_ue_f1ap_id);

  nr_cell_global_id_t cgi = cgi_from_asn1(msg->nr_cgi);
  if (not config_helpers::is_valid(cgi)) {
    logger.warning("du_ue_f1ap_id={}: Dropping InitialULRRCMessageTransfer. Invalid CGI", du_ue_id);
    return;
  }

  rnti_t crnti = to_rnti(msg->c_rnti);
  if (crnti == rnti_t::INVALID_RNTI) {
    logger.warning("du_ue_f1ap_id={}: Dropping InitialULRRCMessageTransfer. Cause: Invalid C-RNTI", du_ue_id);
    return;
  }

  logger.debug("du_ue_f1ap_id={} nci={} crnti={} plmn={}: Received InitialULRRCMessageTransfer",
               du_ue_id,
               cgi.nci,
               crnti,
               cgi.plmn);

  if (msg->sul_access_ind_present) {
    logger.debug("du_ue_f1ap_id={}: Ignoring SUL access indicator", du_ue_id);
  }

  const gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
  if (cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid) {
    logger.warning("du_ue_f1ap_id={}: Dropping InitialULRRCMessageTransfer. Cause: Failed to allocate CU-UE-F1AP-ID",
                   du_ue_id);
    return;
  }

  // Create CU-CP UE instance.
  const ue_index_t ue_index = du_processor_notifier.on_new_cu_cp_ue_required();
  if (ue_index == ue_index_t::invalid) {
    logger.warning("du_ue_f1ap_id={}: Dropping InitialULRRCMessageTransfer. Cause: CU-CP UE creation failed",
                   msg->gnb_du_ue_f1ap_id);
    return;
  }

  // Update the UE RRC context (e.g. C-RNTI, PCell) in the CU-CP.
  ue_rrc_context_creation_request req;
  req.ue_index = ue_index;
  req.c_rnti   = crnti;
  req.cgi      = cgi;
  if (msg->du_to_cu_rrc_container_present) {
    req.du_to_cu_rrc_container = byte_buffer(msg->du_to_cu_rrc_container);
  } else {
    // Assume the DU can't serve the UE, so the CU-CP should reject the UE, see TS 38.473 section 8.4.1.2.
    // We will forward an empty container to the RRC UE, that will trigger an RRC Reject
    logger.debug("du_ue_f1ap_id={}: Forwarding InitialULRRCMessageTransfer to RRC to reject the UE. Cause: Missing DU "
                 "to CU container",
                 du_ue_id);
    req.du_to_cu_rrc_container = byte_buffer{};
  }
  const ue_rrc_context_creation_response resp = du_processor_notifier.on_ue_rrc_context_creation_request(req);

  // Remove the UE if the creation was not successful
  if (resp.f1ap_rrc_notifier == nullptr) {
    logger.warning("du_ue_f1ap_id={}: Dropping InitialULRRCMessageTransfer. Cause: UE RRC context creation failed",
                   msg->gnb_du_ue_f1ap_id);
    return;
  }

  // Create UE context and store it
  ue_ctxt_list.add_ue(ue_index, cu_ue_f1ap_id);
  ue_ctxt_list.add_du_ue_f1ap_id(cu_ue_f1ap_id, du_ue_id);
  ue_ctxt_list.add_rrc_notifier(ue_index, resp.f1ap_rrc_notifier);
  f1ap_ue_context& ue_ctxt = ue_ctxt_list[cu_ue_f1ap_id];

  ue_ctxt.logger.log_info("Added UE context");

  // Forward RRC container
  if (msg->rrc_container_rrc_setup_complete_present) {
    // RRC setup complete over SRB1
    ue_ctxt_list[cu_ue_f1ap_id].rrc_notifier->on_ul_dcch_pdu(srb_id_t::srb1,
                                                             msg->rrc_container_rrc_setup_complete.copy());
    return;
  }

  // Pass RRC container to RRC
  ue_ctxt_list[cu_ue_f1ap_id].rrc_notifier->on_ul_ccch_pdu(msg->rrc_container.copy());
}

void f1ap_cu_impl::handle_ul_rrc_message(const ul_rrc_msg_transfer_s& msg)
{
  if (!ue_ctxt_list.contains(int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id))) {
    logger.warning("cu_ue_f1ap_id={} du_ue_f1ap_id={}: Dropping UlRrcMessageTransfer. UE context does not exist",
                   msg->gnb_cu_ue_f1ap_id,
                   msg->gnb_du_ue_f1ap_id);
    return;
  }

  f1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id)];
  ue_ctxt.logger.log_debug("Received UlRrcMessageTransfer");

  // Notify upper layers about reception
  ue_ctxt.rrc_notifier->on_ul_dcch_pdu(int_to_srb_id(msg->srb_id), msg->rrc_container.copy());
}

void f1ap_cu_impl::handle_f1_removal_request(const asn1::f1ap::f1_removal_request_s& msg)
{
  logger.debug("Received F1 Removal Request");

  du_index_t du_index = du_processor_notifier.get_du_index();
  du_management_notifier.on_du_remove_request_received(du_index);
}

void f1ap_cu_impl::handle_ue_context_release_request(const asn1::f1ap::ue_context_release_request_s& msg)
{
  if (!ue_ctxt_list.contains(int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id))) {
    logger.warning("cu_ue_f1ap_id={} du_ue_f1ap_id={}: Dropping UeContextReleaseRequest. UE context does not exist",
                   msg->gnb_cu_ue_f1ap_id,
                   msg->gnb_du_ue_f1ap_id);
    return;
  }

  f1ap_ue_context& ue_ctxt = ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(msg->gnb_cu_ue_f1ap_id)];

  if (ue_ctxt.marked_for_release) {
    // UE context is already being released. Ignore the request.
    ue_ctxt.logger.log_debug("UeContextReleaseRequest ignored. UE context release procedure has already started");
    return;
  }

  ue_ctxt.logger.log_debug("Received UeContextReleaseRequest");

  f1ap_ue_context_release_request req;
  req.ue_index = ue_ctxt.ue_ids.ue_index;
  req.cause    = asn1_to_cause(msg->cause);

  du_processor_notifier.on_du_initiated_ue_context_release_request(req);
}

void f1ap_cu_impl::handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_release_complete: {
      ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(outcome.value.ue_context_release_complete()->gnb_cu_ue_f1ap_id)]
          .ev_mng.context_release_complete.set(outcome.value.ue_context_release_complete());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_setup_resp: {
      ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(outcome.value.ue_context_setup_resp()->gnb_cu_ue_f1ap_id)]
          .ev_mng.context_setup_outcome.set(outcome.value.ue_context_setup_resp());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_mod_resp: {
      ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(outcome.value.ue_context_mod_resp()->gnb_cu_ue_f1ap_id)]
          .ev_mng.context_modification_outcome.set(outcome.value.ue_context_mod_resp());
    } break;
    default:
      logger.warning("Successful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}

void f1ap_cu_impl::handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome)
{
  switch (outcome.value.type().value) {
    case asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ue_context_setup_fail: {
      ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(outcome.value.ue_context_setup_fail()->gnb_cu_ue_f1ap_id)]
          .ev_mng.context_setup_outcome.set(outcome.value.ue_context_setup_fail());
    } break;
    case asn1::f1ap::f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ue_context_mod_fail: {
      ue_ctxt_list[int_to_gnb_cu_ue_f1ap_id(outcome.value.ue_context_mod_fail()->gnb_cu_ue_f1ap_id)]
          .ev_mng.context_modification_outcome.set(outcome.value.ue_context_mod_fail());
    } break;
    default:
      logger.warning("Unsuccessful outcome of type {} is not supported", outcome.value.type().to_string());
  }
}
