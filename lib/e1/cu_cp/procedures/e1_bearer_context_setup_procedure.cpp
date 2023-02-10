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
#include "../e1ap_cu_cp_asn1_helpers.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::e1ap;

e1_bearer_context_setup_procedure::e1_bearer_context_setup_procedure(const e1_message&     request_,
                                                                     e1ap_ue_context&      ue_ctxt_,
                                                                     e1_message_notifier&  e1_notif_,
                                                                     srslog::basic_logger& logger_) :
  request(request_), ue_ctxt(ue_ctxt_), e1_notifier(e1_notif_), logger(logger_)
{
}

void e1_bearer_context_setup_procedure::operator()(coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT SETUP RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.bearer_ev_mng.context_setup_outcome);

  // Send command to CU-UP.
  send_bearer_context_setup_request();

  // Await response.
  CORO_AWAIT(transaction_sink);

  // Handle response from CU-UP and return bearer index
  CORO_RETURN(create_bearer_context_setup_result());
}

void e1_bearer_context_setup_procedure::send_bearer_context_setup_request()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    request.pdu.to_json(js);
    logger.debug("Containerized BearerContextSetupRequest: {}", js.to_string());
  }

  // send Bearer context setup request message
  e1_notifier.on_new_message(request);
}

e1ap_bearer_context_setup_response e1_bearer_context_setup_procedure::create_bearer_context_setup_result()
{
  e1ap_bearer_context_setup_response res{};

  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_setup_resp_s& resp = transaction_sink.response();
    logger.debug("Received BearerContextSetupResponse");

    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized BearerContextSetupResponse: {}", js.to_string());
    }

    // Add CU-UP UE E1AP ID to UE context
    ue_ctxt.cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(resp->gnb_cu_up_ue_e1ap_id.value);
    fill_e1ap_bearer_context_setup_response(res, resp);
  } else if (transaction_sink.failed()) {
    const asn1::e1ap::bearer_context_setup_fail_s& fail = transaction_sink.failure();
    logger.debug("Received BearerContextSetupFailure cause={}", get_cause_str(fail->cause.value));

    // Add CU-UP UE E1AP ID to UE context
    if (fail->gnb_cu_up_ue_e1ap_id_present) {
      ue_ctxt.cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(fail->gnb_cu_up_ue_e1ap_id.value);
    }
    fill_e1ap_bearer_context_setup_response(res, fail);
  } else {
    logger.warning("BearerContextSetupResponse timeout");
    res.success = false;
  }

  return res;
}