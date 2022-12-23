/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1_bearer_context_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::e1ap;

e1_bearer_context_setup_procedure::e1_bearer_context_setup_procedure(const bearer_context_setup_request_s& request_,
                                                                     e1_message_notifier&                  e1_notif_,
                                                                     e1_event_manager&                     ev_mng_,
                                                                     srslog::basic_logger&                 logger_) :
  request(request_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_setup_procedure::operator()(coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to CU-UP.
  send_bearer_context_setup_request();

  // Await response.
  CORO_AWAIT_VALUE(e1_bearer_ctxt_setup_outcome, ev_mng.e1ap_bearer_context_setup_outcome);

  // Handle response from CU-UP and return bearer index
  CORO_RETURN(create_bearer_context_setup_result());
}

void e1_bearer_context_setup_procedure::send_bearer_context_setup_request()
{
  // Pack message into PDU
  e1_message e1_bearer_ctxt_setup_request_msg;
  e1_bearer_ctxt_setup_request_msg.pdu.set_init_msg();
  e1_bearer_ctxt_setup_request_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);
  e1_bearer_ctxt_setup_request_msg.pdu.init_msg().value.bearer_context_setup_request() = request;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    e1_bearer_ctxt_setup_request_msg.pdu.to_json(js);
    logger.debug("Containerized Bearer Context Setup Request message: {}", js.to_string());
  }

  // send Bearer context setup request message
  e1_notifier.on_new_message(e1_bearer_ctxt_setup_request_msg);
}

e1ap_bearer_context_setup_response e1_bearer_context_setup_procedure::create_bearer_context_setup_result()
{
  e1ap_bearer_context_setup_response res{};

  if (e1_bearer_ctxt_setup_outcome.has_value()) {
    logger.info("Received E1AP Bearer Context Setup Response.");
    res.response = *e1_bearer_ctxt_setup_outcome.value();
    res.success  = true;
  } else {
    logger.info("Received E1AP Bearer Context Setup Failure. Cause: {}",
                get_cause_str((*e1_bearer_ctxt_setup_outcome.error())->cause.value));
    res.failure = *e1_bearer_ctxt_setup_outcome.error();
    res.success = false;
  }
  return res;
}