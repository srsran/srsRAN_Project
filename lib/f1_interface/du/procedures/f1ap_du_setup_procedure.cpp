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
using namespace asn1::f1ap;

f1ap_du_setup_procedure::f1ap_du_setup_procedure(const f1_setup_request_message& request_,
                                                 f1c_message_notifier&           cu_notif_,
                                                 f1ap_event_manager&             ev_mng_,
                                                 srslog::basic_logger&           logger_) :
  request(request_), cu_notifier(cu_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void f1ap_du_setup_procedure::operator()(coro_context<async_task<f1_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    transaction = ev_mng.transactions.create_transaction();

    // Send request to CU.
    send_f1_setup_request();

    // Await CU response.
    CORO_AWAIT(transaction);

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
  f1_ap_pdu_c pdu;
  pdu.set_init_msg();
  pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  pdu.init_msg().value.f1_setup_request() = request.msg;

  // set values handled by F1
  auto& setup_req                 = pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id.value = transaction.id();

  // send request
  cu_notifier.on_new_message(pdu);
}

bool f1ap_du_setup_procedure::retry_required()
{
  const f1ap_outcome& cu_pdu_response = transaction.result();
  if (cu_pdu_response.has_value()) {
    // Success case.
    return false;
  }

  if (cu_pdu_response.error().value.type().value !=
      f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail) {
    // Invalid response type.
    return false;
  }

  const f1_setup_fail_ies_container& f1_fail = *cu_pdu_response.error().value.f1_setup_fail();
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
  const f1ap_outcome&       cu_pdu_response = transaction.result();
  f1_setup_response_message res{};

  if (cu_pdu_response.has_value() and cu_pdu_response.value().value.type().value ==
                                          f1_ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp) {
    logger.info("Received F1AP PDU with successful outcome.");
    res.msg     = cu_pdu_response.value().value.f1_setup_resp();
    res.success = true;
  } else if (cu_pdu_response.has_value() or cu_pdu_response.error().value.type().value !=
                                                f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail) {
    logger.error("Received F1AP PDU with unexpected F1AP PDU type {}",
                 cu_pdu_response.value().value.type().to_string());
    res.success = false;
  } else {
    logger.info("Received F1AP PDU with unsuccessful outcome. Cause: {}",
                get_cause_str(cu_pdu_response.error().value.f1_setup_fail()->cause.value));
    res.success = false;
  }
  return res;
}
