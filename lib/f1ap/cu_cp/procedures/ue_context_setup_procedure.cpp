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

#include "ue_context_setup_procedure.h"
#include "../f1ap_asn1_helpers.h"
#include "srsran/f1ap/common/f1ap_message.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

ue_context_setup_procedure::ue_context_setup_procedure(const f1ap_ue_context_setup_request& request_,
                                                       f1ap_ue_context_list&                ue_ctxt_list_,
                                                       f1ap_du_processor_notifier&          du_processor_notifier_,
                                                       f1ap_message_notifier&               f1ap_notif_,
                                                       srslog::basic_logger&                logger_) :
  request(request_),
  ue_ctxt_list(ue_ctxt_list_),
  du_processor_notifier(du_processor_notifier_),
  f1ap_notifier(f1ap_notif_),
  logger(logger_)
{
}

void ue_context_setup_procedure::operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized.", name());

  // Create UE context in CU-CP.
  if (not allocate_ue_ids()) {
    logger.error("Failed to create UE context");
    CORO_EARLY_RETURN(create_ue_context_setup_result());
  }

  srsran_assert(new_cu_ue_f1ap_id != gnb_cu_ue_f1ap_id_t::invalid, "CU F1AP UE ID must not be invalid");
  srsran_assert(ue_ctxt_list.contains(new_cu_ue_f1ap_id), "UE context must exist at this point.");

  // Subscribe to respective publisher to receive UE CONTEXT SETUP RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt_list[new_cu_ue_f1ap_id].ev_mng.context_setup_outcome);

  // Send command to DU.
  send_ue_context_setup_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_setup_result());
}

bool ue_context_setup_procedure::allocate_ue_ids()
{
  gnb_cu_ue_f1ap_id_t tmp_cu_ue_f1ap_id = ue_ctxt_list.next_gnb_cu_ue_f1ap_id();
  if (tmp_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid) {
    logger.error("No CU UE F1AP ID available");
    return false;
  }

  // Request allocation of UE index in target cell
  ue_index_t ue_index = du_processor_notifier.on_new_ue_index_required();
  if (ue_index == ue_index_t::invalid) {
    logger.error("Couldn't create UE in target cell");
    return false;
  }

  // Create UE context and store it.
  f1ap_ue_context& ue_ctxt = ue_ctxt_list.add_ue(ue_index, tmp_cu_ue_f1ap_id);
  logger.debug("ue={} Added UE (cu_ue_f1ap_id={}, du_ue_f1ap_id=<n/a>)", ue_ctxt.ue_index, tmp_cu_ue_f1ap_id);

  // Store identifiers.
  new_cu_ue_f1ap_id = tmp_cu_ue_f1ap_id;
  new_ue_index      = ue_ctxt.ue_index;

  return true;
}

bool ue_context_setup_procedure::create_ue_context(f1ap_ue_context_setup_response& response)
{
  // Request UE creation in target cell.

  cu_cp_ue_creation_message ue_creation_msg;
  ue_creation_msg.ue_index               = response.ue_index;
  ue_creation_msg.c_rnti                 = response.c_rnti.value();
  ue_creation_msg.cgi                    = request.sp_cell_id;
  ue_creation_msg.du_to_cu_rrc_container = response.du_to_cu_rrc_info.cell_group_cfg.copy();

  ue_creation_complete_message ue_creation_complete_msg = du_processor_notifier.on_create_ue(ue_creation_msg);
  if (ue_creation_complete_msg.ue_index == ue_index_t::invalid) {
    logger.error("Couldn't create UE in target cell");
    return false;
  }

  // Add SRBs to ue context.
  f1ap_ue_context& ue_ctxt = ue_ctxt_list[ue_creation_complete_msg.ue_index];
  ue_ctxt.srbs             = ue_creation_complete_msg.srbs;

  logger.debug("ue={} Added SRBs", ue_ctxt.ue_index);

  return true;
}

void ue_context_setup_procedure::send_ue_context_setup_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_setup_request_msg;
  f1ap_ue_ctxt_setup_request_msg.pdu.set_init_msg();
  f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& req = f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().value.ue_context_setup_request();

  // Convert common type to asn1
  fill_asn1_ue_context_setup_request(req, request);

  req->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(new_cu_ue_f1ap_id);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1ap_ue_ctxt_setup_request_msg.pdu.to_json(js);
    logger.debug("Containerized UeContextSetupRequest: {}", js.to_string());
  }

  // send UE context setup request message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_setup_request_msg);
}

f1ap_ue_context_setup_response ue_context_setup_procedure::create_ue_context_setup_result()
{
  f1ap_ue_context_setup_response res{};

  if (new_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::invalid || new_ue_index == ue_index_t::invalid) {
    res.success = false;
    logger.error("\"{}\" failed.", name());
    return res;
  }

  srsran_assert(ue_ctxt_list.contains(new_cu_ue_f1ap_id), "UE context must exist at this point.");

  if (transaction_sink.successful()) {
    logger.debug("Received UeContextSetupResponse");
    fill_f1ap_ue_context_setup_response(res, new_ue_index, transaction_sink.response());
    res.success = create_ue_context(res);
  } else if (transaction_sink.failed()) {
    logger.debug("Received UeContextSetupFailure cause={}", get_cause_str(transaction_sink.failure()->cause));
    fill_f1ap_ue_context_setup_response(res, transaction_sink.failure());
    res.success = false;
  } else {
    logger.warning("UeContextSetup timeout");
    res.success = false;
  }

  if (res.success) {
    logger.debug("ue={}: \"{}\" finalized.", ue_ctxt_list[new_cu_ue_f1ap_id].ue_index, name());
  } else {
    logger.error("ue={}: \"{}\" failed.", ue_ctxt_list[new_cu_ue_f1ap_id].ue_index, name());
    delete_ue_context(new_cu_ue_f1ap_id);
  }

  return res;
}

void ue_context_setup_procedure::delete_ue_context(gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id)
{
  // Delete UE in DU processor.
  du_processor_notifier.on_delete_ue(ue_ctxt_list[new_cu_ue_f1ap_id].ue_index);

  // Remove F1AP context.
  ue_ctxt_list.remove_ue(cu_ue_f1ap_id);
}