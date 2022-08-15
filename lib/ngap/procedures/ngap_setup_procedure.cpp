/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_setup_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::ngap;

ngap_setup_procedure::ngap_setup_procedure(const ng_setup_request_message& request_,
                                           ng_message_notifier&            amf_notif_,
                                           ngap_event_manager&             ev_mng_,
                                           srslog::basic_logger&           logger_) :
  request(request_), amf_notifier(amf_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void ngap_setup_procedure::operator()(coro_context<async_task<ng_setup_response_message>>& ctx)
{
  CORO_BEGIN(ctx);

  while (true) {
    // Send request to AMF.
    send_ng_setup_request();

    // Await AMF response.
    CORO_AWAIT_VALUE(ngap_setup_outcome, ev_mng.ngap_setup_response);

    if (not retry_required()) {
      // No more attempts. Exit loop.
      break;
    }

    // Await timer.
    logger.info("Received NGSetupFailure with Time to Wait IE. Reinitiating NG setup in {}s (retry={}/{}).",
                time_to_wait,
                ng_setup_retry_no,
                request.max_setup_retries);
    // TODO: CORO_AWAIT(timer.wait(time_to_wait));
  }

  // Forward procedure result to DU manager.
  CORO_RETURN(create_ng_setup_result());
}

void ngap_setup_procedure::send_ng_setup_request()
{
  ngap_msg msg = {};
  // set NGAP PDU contents
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_NG_SETUP);
  msg.pdu.init_msg().value.ng_setup_request() = request.msg;

  // TODO: fill message

  // send request
  amf_notifier.on_new_message(msg);
}

bool ngap_setup_procedure::retry_required()
{
  if (ngap_setup_outcome.has_value()) {
    // Success case.
    return false;
  }

  const asn1::ngap::ng_setup_fail_s& ng_fail = *ngap_setup_outcome.error();
  if (not ng_fail->time_to_wait_present) {
    // AMF didn't command a waiting time.
    logger.error("AMF did not set any retry waiting time.");
    return false;
  }
  if (ng_setup_retry_no++ >= request.max_setup_retries) {
    // Number of retries exceeded, or there is no time to wait.
    logger.error("Reached maximum number of NG Setup connection retries ({}).", request.max_setup_retries);
    return false;
  }

  time_to_wait = ng_fail->time_to_wait->to_number();
  return true;
}

ng_setup_response_message ngap_setup_procedure::create_ng_setup_result()
{
  ng_setup_response_message res{};

  if (ngap_setup_outcome.has_value()) {
    logger.info("Received NGAP PDU with successful outcome.");
    res.msg     = *ngap_setup_outcome.value();
    res.success = true;
  } else {
    const asn1::ngap::ng_setup_fail_s& ng_fail = *ngap_setup_outcome.error();
    logger.info("Received NGAP PDU with unsuccessful outcome. Cause: {}", get_cause_str(ng_fail->cause.value));
    res.success = false;
  }
  return res;
}
