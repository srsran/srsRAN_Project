/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_modification_procedure.h"
#include "../e1ap_cu_cp_asn1_helpers.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

bearer_context_modification_procedure::bearer_context_modification_procedure(const e1ap_message&    request_,
                                                                             e1ap_ue_context&       ue_ctxt_,
                                                                             e1ap_message_notifier& e1ap_notif_,
                                                                             srslog::basic_logger&  logger_) :
  request(request_), ue_ctxt(ue_ctxt_), e1ap_notifier(e1ap_notif_), logger(logger_)
{
}

void bearer_context_modification_procedure::operator()(
    coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.bearer_ev_mng.context_modification_outcome);

  // Send command to DU.
  send_bearer_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_bearer_context_modification_result());
}

void bearer_context_modification_procedure::send_bearer_context_modification_request()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    request.pdu.to_json(js);
    logger.debug("Containerized BearerContextModificationRequest: {}", js.to_string());
  }

  // send UE context modification request message
  e1ap_notifier.on_new_message(request);
}

e1ap_bearer_context_modification_response
bearer_context_modification_procedure::create_bearer_context_modification_result()
{
  e1ap_bearer_context_modification_response res{};

  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_mod_resp_s& resp = transaction_sink.response();
    logger.debug("Received BearerContextModificationResponse");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized BearerContextModificationResponse: {}", js.to_string());
    }
    fill_e1ap_bearer_context_modification_response(res, resp);
  } else if (transaction_sink.failed()) {
    const asn1::e1ap::bearer_context_mod_fail_s& fail = transaction_sink.failure();
    logger.debug("Received BearerContextModificationFailure cause={}", get_cause_str(fail->cause.value));
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      fail.to_json(js);
      logger.debug("Containerized BearerContextModificationFailure: {}", js.to_string());
    }
    fill_e1ap_bearer_context_modification_response(res, fail);
  } else {
    logger.warning("BearerContextModificationResponse timeout");
    res.success = false;
  }
  return res;
}