/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_context_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::f1ap;

ue_context_setup_procedure::ue_context_setup_procedure(const ue_context_setup_request_s& request_,
                                                       f1ap_ue_context&                  ue_ctx_,
                                                       f1ap_message_notifier&            f1ap_notif_,
                                                       srslog::basic_logger&             logger_) :
  request(request_), ue_ctx(ue_ctx_), f1ap_notifier(f1ap_notif_), logger(logger_)
{
}

void ue_context_setup_procedure::operator()(coro_context<async_task<f1ap_ue_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive UE CONTEXT SETUP RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctx.ev_mng.context_setup_outcome);

  // Send command to DU.
  send_ue_context_setup_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_setup_result());
}

void ue_context_setup_procedure::send_ue_context_setup_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_setup_request_msg;
  f1ap_ue_ctxt_setup_request_msg.pdu.set_init_msg();
  f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& req = f1ap_ue_ctxt_setup_request_msg.pdu.init_msg().value.ue_context_setup_request();

  req                           = request;
  req->gnb_cu_ue_f1ap_id->value = gnb_cu_ue_f1ap_id_to_uint(ue_ctx.cu_ue_f1ap_id);
  req->gnb_du_ue_f1ap_id->value = gnb_du_ue_f1ap_id_to_uint(ue_ctx.du_ue_f1ap_id);

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

  if (transaction_sink.successful()) {
    logger.debug("Received UeContextSetupResponse");
    res.response = transaction_sink.response();
    res.success  = true;
  } else if (transaction_sink.failed()) {
    logger.debug("Received UeContextSetupFailure cause={}", get_cause_str(transaction_sink.failure()->cause.value));
    res.failure = transaction_sink.failure();
    res.success = false;
  } else {
    logger.warning("UeContextSetup timeout");
    res.success = false;
  }
  return res;
}