/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "e1ap_cu_up_setup_procedure.h"
#include "../e1ap_cu_up_asn1_helpers.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_cu_up;
using namespace asn1::e1ap;

e1ap_cu_up_setup_procedure::e1ap_cu_up_setup_procedure(const cu_up_e1_setup_request& request_,
                                                       e1ap_message_notifier&        cu_cp_notif_,
                                                       e1ap_event_manager&           ev_mng_,
                                                       timer_factory                 timers_,
                                                       srslog::basic_logger&         logger_) :
  request(request_),
  cu_cp_notifier(cu_cp_notif_),
  ev_mng(ev_mng_),
  timers(timers_),
  logger(logger_),
  e1_setup_wait_timer(timers_.create_timer())
{
}

void e1ap_cu_up_setup_procedure::operator()(coro_context<async_task<cu_up_e1_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized", name());

  while (true) {
    transaction = ev_mng.transactions.create_transaction();

    // Send request to CU.
    send_cu_up_e1_setup_request();

    // Await CU response.
    CORO_AWAIT(transaction);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.debug("Reinitiating E1 setup in {}s (retry={}/{}). Received E1SetupFailure with Time to Wait IE",
                 time_to_wait.count(),
                 e1_setup_retry_no,
                 request.max_setup_retries);
    CORO_AWAIT(
        async_wait_for(e1_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_e1_setup_result());
}

void e1ap_cu_up_setup_procedure::send_cu_up_e1_setup_request()
{
  e1ap_message msg = {};
  // set E1AP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_GNB_CU_UP_E1_SETUP);
  gnb_cu_up_e1_setup_request_s& setup_req = msg.pdu.init_msg().value.gnb_cu_up_e1_setup_request();

  setup_req = cu_up_e1_setup_request_to_asn1(request);

  setup_req->transaction_id = transaction.id();

  // send request
  logger.info("Sending E1SetupRequest");
  cu_cp_notifier.on_new_message(msg);
}

bool e1ap_cu_up_setup_procedure::retry_required()
{
  if (transaction.aborted()) {
    // Timeout or cancellation case.
    return false;
  }
  const e1ap_transaction_response& cu_cp_pdu_response = transaction.response();
  if (cu_cp_pdu_response.has_value()) {
    // Success case.
    return false;
  }

  if (cu_cp_pdu_response.error().value.type().value !=
      e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail) {
    // Invalid response type.
    return false;
  }

  const gnb_cu_up_e1_setup_fail_ies_container& e1_setup_fail =
      *cu_cp_pdu_response.error().value.gnb_cu_up_e1_setup_fail();
  if (not e1_setup_fail.time_to_wait_present) {
    // CU didn't command a waiting time.
    logger.error("CU-CP did not set any retry waiting time");
    return false;
  }
  if (e1_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of E1 Setup connection retries ({})", request.max_setup_retries);
    return false;
  }

  time_to_wait = std::chrono::seconds{e1_setup_fail.time_to_wait.to_number()};
  return true;
}

cu_up_e1_setup_response e1ap_cu_up_setup_procedure::create_e1_setup_result()
{
  cu_up_e1_setup_response res{};

  if (transaction.aborted()) {
    // Abortion/timeout case.
    logger.error("\"{}\" failed (timeout reached)");
    res.success = false;
    return res;
  }
  const e1ap_transaction_response& cu_cp_pdu_response = transaction.response();

  if (cu_cp_pdu_response.has_value() and
      cu_cp_pdu_response.value().value.type().value ==
          e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp) {
    res.success = true;

    if (cu_cp_pdu_response.value().value.gnb_cu_up_e1_setup_resp()->gnb_cu_cp_name_present) {
      res.gnb_cu_cp_name = cu_cp_pdu_response.value().value.gnb_cu_up_e1_setup_resp()->gnb_cu_cp_name.to_string();
    }

    logger.info("\"{}\" finalized", name());

  } else if (cu_cp_pdu_response.has_value() and
             cu_cp_pdu_response.error().value.type().value !=
                 e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail) {
    logger.error("\"{}\" failed with cause={}", name(), cu_cp_pdu_response.value().value.type().to_string());
    res.success = false;
  } else {
    logger.error("\"{}\" failed with cause={}",
                 name(),
                 get_cause_str(cu_cp_pdu_response.error().value.gnb_cu_up_e1_setup_fail()->cause));
    res.success = false;
  }
  return res;
}
