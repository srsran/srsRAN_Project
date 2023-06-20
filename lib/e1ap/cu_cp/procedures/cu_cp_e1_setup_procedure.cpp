/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "cu_cp_e1_setup_procedure.h"
#include "../../common/e1ap_asn1_helpers.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

cu_cp_e1_setup_procedure::cu_cp_e1_setup_procedure(const cu_cp_e1_setup_request& request_,
                                                   e1ap_message_notifier&        cu_up_notif_,
                                                   e1ap_transaction_manager&     ev_mng_,
                                                   timer_factory                 timers,
                                                   srslog::basic_logger&         logger_) :
  request(request_),
  cu_up_notifier(cu_up_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  e1_setup_wait_timer(timers.create_timer())
{
}

void cu_cp_e1_setup_procedure::operator()(coro_context<async_task<cu_cp_e1_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized.", name());

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
    logger.debug("Received E1SetupFailure with Time to Wait IE - reinitiating E1 setup in {}s (retry={}/{})",
                 time_to_wait.count(),
                 e1_setup_retry_no,
                 request.max_setup_retries);
    CORO_AWAIT(
        async_wait_for(e1_setup_wait_timer, std::chrono::duration_cast<std::chrono::milliseconds>(time_to_wait)));
  }

  // Forward procedure result
  CORO_RETURN(create_e1_setup_result());
}

void cu_cp_e1_setup_procedure::send_e1_setup_request()
{
  e1ap_message msg = {};
  // set E1AP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_GNB_CU_CP_E1_SETUP);
  auto& setup_req = msg.pdu.init_msg().value.gnb_cu_cp_e1_setup_request();

  if (request.gnb_cu_cp_name.has_value()) {
    setup_req->gnb_cu_cp_name_present = true;
    setup_req->gnb_cu_cp_name.from_string(request.gnb_cu_cp_name.value());
  }

  // set values handled by E1
  setup_req->transaction_id = transaction.id();

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
    logger.error("CU-UP did not set any retry waiting time");
    return false;
  }
  if (e1_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of CU-CP E1 Setup connection retries ({})", request.max_setup_retries);
    return false;
  }

  time_to_wait = std::chrono::seconds{e1_setup_fail.time_to_wait.to_number()};
  return true;
}

cu_cp_e1_setup_response cu_cp_e1_setup_procedure::create_e1_setup_result()
{
  const e1ap_outcome&     cu_cp_e1_setup_outcome = transaction.result();
  cu_cp_e1_setup_response res{};

  if (cu_cp_e1_setup_outcome.has_value()) {
    logger.debug("Received PDU with successful outcome");

    fill_e1ap_cu_cp_e1_setup_response(res, cu_cp_e1_setup_outcome.value().value.gnb_cu_cp_e1_setup_resp());

    logger.debug("\"{}\" finalized.", name());
  } else if (cu_cp_e1_setup_outcome.has_value() or
             cu_cp_e1_setup_outcome.error().value.type().value !=
                 e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail) {
    logger.error("Received PDU with unexpected type {}", cu_cp_e1_setup_outcome.value().value.type().to_string());
    res.success = false;

    logger.error("\"{}\" failed.", name());
  } else {
    logger.debug("Received PDU with unsuccessful outcome cause={}",
                 get_cause_str(cu_cp_e1_setup_outcome.error().value.gnb_cu_cp_e1_setup_fail()->cause));
    fill_e1ap_cu_cp_e1_setup_response(res, cu_cp_e1_setup_outcome.error().value.gnb_cu_cp_e1_setup_fail());

    logger.error("\"{}\" failed.", name());
  }
  return res;
}
