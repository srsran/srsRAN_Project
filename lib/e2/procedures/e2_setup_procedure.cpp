/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_setup_procedure.h"
#include "srsgnb/support/async/async_timer.h"

using namespace srsgnb;
using namespace asn1::e2ap;

e2_setup_procedure::e2_setup_procedure(const e2_setup_request_message& request_,
                                       e2_message_notifier&            notif_,
                                       e2_event_manager&               ev_mng_,
                                       timer_manager&                  timers,
                                       srslog::basic_logger&           logger_) :
  request(request_),
  notifier(notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  e2_setup_wait_timer(timers.create_unique_timer())
{
}

void e2_setup_procedure::operator()(coro_context<async_task<e2_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    transaction = ev_mng.transactions.create_transaction();

    // Send request to CU-UP.
    send_e2_setup_request();

    // Await CU response.
    CORO_AWAIT(transaction);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Received E2SetupFailure with Time to Wait IE. Reinitiating E2 setup in {}s (retry={}/{}).",
                time_to_wait,
                e2_setup_retry_no,
                request.max_setup_retries);
    CORO_AWAIT(async_wait_for(e2_setup_wait_timer, time_to_wait * 1000));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_e2_setup_result());
}

void e2_setup_procedure::send_e2_setup_request()
{
  e2_message msg = {};
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_E2AP_ID_E2SETUP);
  msg.pdu.init_msg().value.e2setup_request() = request.request;
  auto& setup_req                            = msg.pdu.init_msg().value.e2setup_request();
  setup_req->transaction_id.value            = transaction.id();
  notifier.on_new_message(msg);
}

bool e2_setup_procedure::retry_required()
{
  const e2ap_outcome& e2_setup_outcome = transaction.result();
  if (e2_setup_outcome.has_value()) {
    // Success case
    return false;
  }

  if (e2_setup_outcome.error().value.type() == e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::e2setup_fail) {
    // Radio network
    return false;
  }
  if (e2_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of E2 Setup connection retries ({}).", request.max_setup_retries);
    return false;
  }

  return true;
}

e2_setup_response_message e2_setup_procedure::create_e2_setup_result()
{
  const e2ap_outcome&       e2_setup_outcome = transaction.result();
  e2_setup_response_message res{};

  if (e2_setup_outcome.has_value()) {
    res.success  = true;
    res.response = e2_setup_outcome.value().value.e2setup_resp();
    logger.info("E2 Setup procedure successful.");
  } else {
    res.success = false;
    logger.error("E2 Setup procedure failed.");
  }
  return res;
}
