/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ng_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::ngap;

ng_setup_procedure::ng_setup_procedure(const ng_setup_request&   request_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       timer_manager&            timers,
                                       srslog::basic_logger&     logger_) :
  request(request_),
  amf_notifier(amf_notif_),
  ev_mng(ev_mng_),
  logger(logger_),
  ng_setup_wait_timer(timers.create_unique_timer())
{
}

void ng_setup_procedure::operator()(coro_context<async_task<ng_setup_response>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    // Subscribe to respective publisher to receive NG SETUP RESPONSE/FAILURE message.
    transaction_sink.subscribe_to(ev_mng.ng_setup_outcome);

    // Send request to AMF.
    send_ng_setup_request();

    // Await AMF response.
    CORO_AWAIT(transaction_sink);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Received NGSetupFailure with Time to Wait IE - Reinitiating NG setup in {}s (retry={}/{})",
                time_to_wait,
                ng_setup_retry_no,
                request.max_setup_retries);
    CORO_AWAIT(async_wait_for(ng_setup_wait_timer, time_to_wait * 1000));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_ng_setup_result());
}

void ng_setup_procedure::send_ng_setup_request()
{
  ngap_message msg = {};
  // set NGAP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  msg.pdu.init_msg().value.ng_setup_request() = request.msg;

  // TODO: fill message

  // send request
  amf_notifier.on_new_message(msg);
}

bool ng_setup_procedure::retry_required()
{
  if (transaction_sink.successful()) {
    // Success case.
    return false;
  }

  const asn1::ngap::ng_setup_fail_s& ng_fail = transaction_sink.failure();
  if (not ng_fail->time_to_wait_present) {
    // AMF didn't command a waiting time.
    logger.error("AMF did not set any retry waiting time");
    return false;
  }
  if (ng_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of NG Setup connection retries ({})", request.max_setup_retries);
    return false;
  }

  time_to_wait = ng_fail->time_to_wait->to_number();
  return true;
}

ng_setup_response ng_setup_procedure::create_ng_setup_result()
{
  ng_setup_response res{};

  if (transaction_sink.successful()) {
    logger.debug("Received PDU with successful outcome");
    res.msg     = transaction_sink.response();
    res.success = true;
  } else {
    const asn1::ngap::ng_setup_fail_s& ng_fail = transaction_sink.failure();
    logger.error("Received PDU with unsuccessful outcome cause={}", get_cause_str(ng_fail->cause.value));
    res.success = false;
  }
  return res;
}
