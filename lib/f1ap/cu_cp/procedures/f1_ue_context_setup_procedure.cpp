/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_ue_context_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

f1_ue_context_setup_procedure::f1_ue_context_setup_procedure(const ue_context_setup_request_s& request_,
                                                             f1ap_ue_context&                  ue_ctx_,
                                                             f1c_message_notifier&             f1c_notif_,
                                                             f1c_event_manager&                ev_mng_,
                                                             srslog::basic_logger&             logger_) :
  request(request_), ue_ctx(ue_ctx_), f1c_notifier(f1c_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void f1_ue_context_setup_procedure::operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to DU.
  send_ue_context_setup_request();

  // Await CU response.
  CORO_AWAIT_VALUE(f1_ue_ctxt_setup_outcome, ev_mng.f1ap_ue_context_setup_outcome);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_setup_result());
}

void f1_ue_context_setup_procedure::send_ue_context_setup_request()
{
  // Pack message into PDU
  f1c_message f1c_ue_ctxt_setup_request_msg;
  f1c_ue_ctxt_setup_request_msg.pdu.set_init_msg();
  f1c_ue_ctxt_setup_request_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& req = f1c_ue_ctxt_setup_request_msg.pdu.init_msg().value.ue_context_setup_request();

  req                           = request;
  req->gnb_cu_ue_f1ap_id->value = gnb_cu_ue_f1ap_id_to_uint(ue_ctx.cu_ue_f1ap_id);
  req->gnb_du_ue_f1ap_id->value = gnb_du_ue_f1ap_id_to_uint(ue_ctx.du_ue_f1ap_id);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1c_ue_ctxt_setup_request_msg.pdu.to_json(js);
    logger.debug("Containerized UE Context Setup Request message: {}", js.to_string());
  }

  // send UE context setup request message
  f1c_notifier.on_new_message(f1c_ue_ctxt_setup_request_msg);
}

f1ap_ue_context_setup_response f1_ue_context_setup_procedure::create_ue_context_setup_result()
{
  f1ap_ue_context_setup_response res{};

  if (f1_ue_ctxt_setup_outcome.has_value()) {
    logger.info("Received F1AP UE Context Setup Response.");
    res.response = *f1_ue_ctxt_setup_outcome.value();
    res.success  = true;
  } else {
    logger.info("Received F1AP UE Context Setup Failure. Cause: {}",
                get_cause_str((*f1_ue_ctxt_setup_outcome.error())->cause.value));
    res.failure = *f1_ue_ctxt_setup_outcome.error();
    res.success = false;
  }
  return res;
}