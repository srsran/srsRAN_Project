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

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

f1_ue_context_modification_procedure::f1_ue_context_modification_procedure(
    const f1ap_ue_context_modification_request& request_,
    f1ap_ue_context&                            ue_ctx_,
    f1c_message_notifier&                       f1c_notif_,
    f1c_event_manager&                          ev_mng_,
    srslog::basic_logger&                       logger_) :
  request(request_), ue_ctx(ue_ctx_), f1c_notifier(f1c_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void f1_ue_context_modification_procedure::operator()(
    coro_context<async_task<f1ap_ue_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to DU.
  send_ue_context_modification_request();

  // Await CU response.
  CORO_AWAIT_VALUE(ue_ctxt_mod_outcome, ev_mng.f1ap_ue_context_modification_outcome);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_modification_result());
}

void f1_ue_context_modification_procedure::send_ue_context_modification_request()
{
  // Pack message into PDU
  f1c_message f1c_ue_ctxt_mod_request_msg;
  f1c_ue_ctxt_mod_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& ctx_mod = f1c_ue_ctxt_mod_request_msg.pdu.init_msg().value.ue_context_mod_request();
  ctx_mod                           = request.msg;

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

f1ap_ue_context_modification_response f1_ue_context_modification_procedure::create_ue_context_modification_result()
{
  f1ap_ue_context_modification_response res{};

  if (ue_ctxt_mod_outcome.has_value()) {
    logger.info("Received F1AP UE Context Modification Response.");
    res.response = *ue_ctxt_mod_outcome.value();
    res.success  = true;
  } else {
    logger.info("Received F1AP UE Context Modification Failure. Cause: {}",
                get_cause_str((*ue_ctxt_mod_outcome.error())->cause.value));
    res.failure = *ue_ctxt_mod_outcome.error();
    res.success = false;
  }
  return res;
}
