/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1_bearer_context_modification_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::e1ap;

e1_bearer_context_modification_procedure::e1_bearer_context_modification_procedure(const e1_message&     request_,
                                                                                   e1_message_notifier&  e1_notif_,
                                                                                   e1_event_manager&     ev_mng_,
                                                                                   srslog::basic_logger& logger_) :
  request(request_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_modification_procedure::operator()(
    coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to DU.
  send_bearer_context_modification_request();

  // Await CU response.
  CORO_AWAIT_VALUE(bearer_ctxt_mod_outcome, ev_mng.e1ap_bearer_context_modification_outcome);

  // Handle response from DU and return UE index
  CORO_RETURN(create_bearer_context_modification_result());
}

void e1_bearer_context_modification_procedure::send_bearer_context_modification_request()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    request.pdu.to_json(js);
    logger.debug("Containerized UE Context Modification Request message: {}", js.to_string());
  }

  // send UE context modification request message
  e1_notifier.on_new_message(request);
}

e1ap_bearer_context_modification_response
e1_bearer_context_modification_procedure::create_bearer_context_modification_result()
{
  e1ap_bearer_context_modification_response res{};

  if (bearer_ctxt_mod_outcome.has_value()) {
    logger.info("Received E1AP Bearer Context Modification Response.");
    res.response = *bearer_ctxt_mod_outcome.value();
    res.success  = true;
  } else {
    logger.info("Received E1AP Bearer Context Modification Failure. Cause: {}",
                get_cause_str((*bearer_ctxt_mod_outcome.error())->cause.value));
    res.failure = *bearer_ctxt_mod_outcome.error();
    res.success = false;
  }
  return res;
}