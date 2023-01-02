/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_e1_setup_procedure.h"
#include "srsgnb/support/async/async_timer.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::e1ap;

cu_cp_e1_setup_procedure::cu_cp_e1_setup_procedure(const cu_cp_e1_setup_request& request_,
                                                   e1_message_notifier&          cu_up_notif_,
                                                   e1_event_manager&             ev_mng_,
                                                   timer_manager&                timers,
                                                   srslog::basic_logger&         logger_) :
  request(request_),
  cu_up_notifier(cu_up_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  e1_setup_wait_timer(timers.create_unique_timer())
{
}

void cu_cp_e1_setup_procedure::operator()(coro_context<async_task<cu_cp_e1_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    transaction = ev_mng.transactions.create_transaction();

    // Send request to CU-UP.
    send_e1_setup_request();

    // Await CU response.
    CORO_AWAIT(transaction);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Received E1SetupFailure with Time to Wait IE. Reinitiating E1 setup in {}s (retry={}/{}).",
                time_to_wait,
                e1_setup_retry_no,
                request.max_setup_retries);
    CORO_AWAIT(async_wait_for(e1_setup_wait_timer, time_to_wait * 1000));
  }

  // Forward procedure result
  CORO_RETURN(create_e1_setup_result());
}

void cu_cp_e1_setup_procedure::send_e1_setup_request()
{
  e1_message msg = {};
  // set E1AP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);
  msg.pdu.init_msg().value.gnb_cu_cp_e1_setup_request() = request.request;

  // set values handled by E1
  auto& setup_req                 = msg.pdu.init_msg().value.gnb_cu_cp_e1_setup_request();
  setup_req->transaction_id.value = transaction.id();

  // send request
  cu_up_notifier.on_new_message(msg);
}

bool cu_cp_e1_setup_procedure::retry_required()
{
  const e1ap_outcome& cu_cp_e1_setup_outcome = transaction.result();
  if (cu_cp_e1_setup_outcome.has_value()) {
    // Success case.
    return false;
  }

  if (cu_cp_e1_setup_outcome.error().value.type().value !=
      e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail) {
    // Invalid response type.
    return false;
  }

  const asn1::e1ap::gnb_cu_cp_e1_setup_fail_ies_container& e1_setup_fail =
      *cu_cp_e1_setup_outcome.error().value.gnb_cu_cp_e1_setup_fail();
  if (not e1_setup_fail.time_to_wait_present) {
    // CU-UP didn't command a waiting time.
    logger.error("CU-UP did not set any retry waiting time.");
    return false;
  }
  if (e1_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of E1 Setup connection retries ({}).", request.max_setup_retries);
    return false;
  }

  time_to_wait = e1_setup_fail.time_to_wait->to_number();
  return true;
}

cu_cp_e1_setup_response cu_cp_e1_setup_procedure::create_e1_setup_result()
{
  const e1ap_outcome&     cu_cp_e1_setup_outcome = transaction.result();
  cu_cp_e1_setup_response res{};

  if (cu_cp_e1_setup_outcome.has_value()) {
    logger.info("Received E1AP PDU with successful outcome.");
    res.response = cu_cp_e1_setup_outcome.value().value.gnb_cu_cp_e1_setup_resp();
    res.success  = true;
  } else if (cu_cp_e1_setup_outcome.has_value() or
             cu_cp_e1_setup_outcome.error().value.type().value !=
                 e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail) {
    logger.error("Received E1AP PDU with unexpected E1AP PDU type {}",
                 cu_cp_e1_setup_outcome.value().value.type().to_string());
    res.success = false;
  } else {
    logger.info("Received E1AP PDU with unsuccessful outcome. Cause: {}",
                get_cause_str(cu_cp_e1_setup_outcome.error().value.gnb_cu_cp_e1_setup_fail()->cause.value));
    res.success = false;
  }
  return res;
}
