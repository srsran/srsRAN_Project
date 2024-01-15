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

#include "ue_context_modification_procedure.h"
#include "../../common/asn1_helpers.h"
#include "../f1ap_asn1_helpers.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

ue_context_modification_procedure::ue_context_modification_procedure(
    const f1ap_ue_context_modification_request& request_,
    f1ap_ue_context&                            ue_ctxt_,
    f1ap_message_notifier&                      f1ap_notif_) :
  request(request_), ue_ctxt(ue_ctxt_), f1ap_notifier(f1ap_notif_)
{
}

void ue_context_modification_procedure::operator()(coro_context<async_task<f1ap_ue_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt.logger.log_debug("\"{}\" initialized", name());

  // Subscribe to respective publisher to receive UE CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.ev_mng.context_modification_outcome);

  // Send command to DU.
  send_ue_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_modification_result());
}

void ue_context_modification_procedure::send_ue_context_modification_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_mod_request_msg;
  f1ap_ue_ctxt_mod_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& ctx_mod = f1ap_ue_ctxt_mod_request_msg.pdu.init_msg().value.ue_context_mod_request();

  fill_asn1_ue_context_modification_request(ctx_mod, request);

  ctx_mod->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.du_ue_f1ap_id);
  ctx_mod->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.ue_ids.cu_ue_f1ap_id);

  if (ue_ctxt.logger.get_basic_logger().debug.enabled()) {
    asn1::json_writer js;
    f1ap_ue_ctxt_mod_request_msg.pdu.to_json(js);
    ue_ctxt.logger.log_debug("Containerized UeContextModificationRequest: {}", js.to_string());
  }

  // send UE context modification request message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_mod_request_msg);
}

f1ap_ue_context_modification_response ue_context_modification_procedure::create_ue_context_modification_result()
{
  f1ap_ue_context_modification_response res{};

  if (transaction_sink.successful()) {
    const asn1::f1ap::ue_context_mod_resp_s& resp = transaction_sink.response();
    ue_ctxt.logger.log_debug("Received UeContextModificationResponse");
    if (ue_ctxt.logger.get_basic_logger().debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      ue_ctxt.logger.log_debug("Containerized UeContextModificationResponse: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response(res, resp);

    ue_ctxt.logger.log_debug("\"{}\" finalized", name());
  } else if (transaction_sink.failed()) {
    const asn1::f1ap::ue_context_mod_fail_s& fail = transaction_sink.failure();
    ue_ctxt.logger.log_debug("Received UeContextModificationFailure cause={}", get_cause_str(fail->cause));
    if (ue_ctxt.logger.get_basic_logger().debug.enabled()) {
      asn1::json_writer js;
      (*transaction_sink.failure()).to_json(js);
      ue_ctxt.logger.log_debug("Containerized UeContextModificationFailure: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response(res, fail);

    ue_ctxt.logger.log_error("\"{}\" failed", name());
  } else {
    ue_ctxt.logger.log_warning("UeContextModificationResponse timeout");
    res.success = false;
    res.cause   = cause_misc_t::unspecified;

    ue_ctxt.logger.log_error("\"{}\" failed", name());
  }

  return res;
}
