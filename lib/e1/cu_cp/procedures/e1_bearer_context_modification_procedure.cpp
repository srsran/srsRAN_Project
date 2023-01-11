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

e1_bearer_context_modification_procedure::e1_bearer_context_modification_procedure(
    const e1_message&                request_,
    e1_message_notifier&             e1_notif_,
    e1ap_bearer_transaction_manager& ev_mng_,
    srslog::basic_logger&            logger_) :
  request(request_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_modification_procedure::operator()(
    coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ev_mng.context_modification_outcome);

  // Send command to DU.
  send_bearer_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_bearer_context_modification_result());
}

void e1_bearer_context_modification_procedure::send_bearer_context_modification_request()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    request.pdu.to_json(js);
    logger.debug("Containerized Bearer Context Modification Request message: {}", js.to_string());
  }

  // send UE context modification request message
  e1_notifier.on_new_message(request);
}

e1ap_bearer_context_modification_response
e1_bearer_context_modification_procedure::create_bearer_context_modification_result()
{
  e1ap_bearer_context_modification_response res{};

  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_mod_resp_s& resp = transaction_sink.response();
    logger.info("Received E1AP Bearer Context Modification Response.");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized Bearer Context Modification Response message: {}", js.to_string());
    }
    res.response = resp;
    res.success  = true;
  } else if (transaction_sink.failed()) {
    const asn1::e1ap::bearer_context_mod_fail_s& fail = transaction_sink.failure();
    logger.info("Received E1AP Bearer Context Modification Failure. Cause: {}", get_cause_str(fail->cause.value));
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      fail.to_json(js);
      logger.debug("Containerized Bearer Context Modification Failure message: {}", js.to_string());
    }
    res.failure = fail;
    res.success = false;
  } else {
    logger.warning("E1AP Bearer Context Modification Response timeout.");
    res.success = false;
  }
  return res;
}