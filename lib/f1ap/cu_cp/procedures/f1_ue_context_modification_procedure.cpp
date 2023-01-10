/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_ue_context_modification_procedure.h"
#include "../../common/asn1_helpers.h"
#include "../f1ap_asn1_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

f1_ue_context_modification_procedure::f1_ue_context_modification_procedure(
    const cu_cp_ue_context_modification_request& request_,
    f1ap_ue_context&                             ue_ctx_,
    f1c_message_notifier&                        f1c_notif_,
    srslog::basic_logger&                        logger_) :
  request(request_), ue_ctx(ue_ctx_), f1c_notifier(f1c_notif_), logger(logger_)
{
}

void f1_ue_context_modification_procedure::operator()(
    coro_context<async_task<cu_cp_ue_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive UE CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctx.ev_mng.context_modification_outcome);

  // Send command to DU.
  send_ue_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_modification_result());
}

void f1_ue_context_modification_procedure::send_ue_context_modification_request()
{
  // Pack message into PDU
  f1c_message f1c_ue_ctxt_mod_request_msg;
  f1c_ue_ctxt_mod_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& ctx_mod = f1c_ue_ctxt_mod_request_msg.pdu.init_msg().value.ue_context_mod_request();

  fill_f1ap_ue_context_modification_request(ctx_mod, request);

  ctx_mod->gnb_du_ue_f1ap_id->value = gnb_du_ue_f1ap_id_to_uint(ue_ctx.du_ue_f1ap_id);
  ctx_mod->gnb_cu_ue_f1ap_id->value = gnb_cu_ue_f1ap_id_to_uint(ue_ctx.cu_ue_f1ap_id);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1c_ue_ctxt_mod_request_msg.pdu.to_json(js);
    logger.debug("Containerized UE Context Modification Request message: {}", js.to_string());
  }

  // send UE context modification request message
  f1c_notifier.on_new_message(f1c_ue_ctxt_mod_request_msg);
}

cu_cp_ue_context_modification_response f1_ue_context_modification_procedure::create_ue_context_modification_result()
{
  cu_cp_ue_context_modification_response res{};

  if (transaction_sink.successful()) {
    const asn1::f1ap::ue_context_mod_resp_s& resp = transaction_sink.response();
    logger.info("Received F1AP UE Context Modification Response.");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized UE Context Modification Response message: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response_message(res, resp);
  } else if (transaction_sink.failed()) {
    const asn1::f1ap::ue_context_mod_fail_s& fail = transaction_sink.failure();
    logger.info("Received F1AP UE Context Modification Failure. Cause: {}", get_cause_str(fail->cause.value));
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      (*transaction_sink.failure()).to_json(js);
      logger.debug("Containerized UE Context Modification Failure message: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response_message(res, fail);
  } else {
    logger.warning("F1AP UE Context Modification Response timeout.");
    res.success = false;
    res.cause   = cu_cp_cause_t::misc;
  }

  return res;
}
