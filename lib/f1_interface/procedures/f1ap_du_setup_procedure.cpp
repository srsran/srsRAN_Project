/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_du_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

void f1ap_du_setup_procedure::operator()(coro_context<async_task<f1_setup_response_message> >& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    // Send request to CU.
    send_f1_setup_request();

    // Await CU response.
    CORO_AWAIT_VALUE(f1_resp, ev_mng.f1ap_setup_response);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Received F1SetupFailure with Time to Wait IE. Reinitiating F1 setup in {}s (retry={}/{}).",
                time_to_wait,
                f1_setup_retry_no,
                request.max_setup_retries);
    // TODO: CORO_AWAIT(timer.wait(time_to_wait));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_f1_setup_result());
}

void f1ap_du_setup_procedure::send_f1_setup_request()
{
  // set F1AP PDU contents
  asn1::f1ap::f1_ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  pdu.init_msg().value.f1_setup_request() = request.msg;

  // set values handled by F1
  auto& setup_req                 = pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id.value = 99;

  // send request
  cu_notifier.on_new_message(pdu);
}

bool f1ap_du_setup_procedure::retry_required()
{
  if (f1_resp.has_value()) {
    // Success case.
    return false;
  }

  auto& f1_fail = **f1_resp.error();
  if (not f1_fail.time_to_wait_present) {
    // CU didn't command a waiting time.
    logger.error("CU did not set any retry waiting time.");
    return false;
  }
  if (f1_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of F1 Setup connection retries ({}).", request.max_setup_retries);
    return false;
  }

  time_to_wait = f1_fail.time_to_wait->to_number();
  return true;
}

f1_setup_response_message f1ap_du_setup_procedure::create_f1_setup_result()
{
  f1_setup_response_message res{};

  if (f1_resp.has_value()) {
    logger.info("Received F1AP PDU with successful outcome.");
    res.msg     = *f1_resp.value();
    res.success = true;
  } else {
    logger.info("Received F1AP PDU with unsuccessful outcome. Cause: {}",
                get_cause_str((*f1_resp.error())->cause.value));
    res.success = false;
  }
  return res;
}
