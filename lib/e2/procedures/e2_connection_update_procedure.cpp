/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_connection_update_procedure.h"
using namespace srsran;
using namespace asn1::e2ap;

e2_connection_update_procedure::e2_connection_update_procedure(const e2conn_upd_s&   request_,
                                                               e2_message_notifier&  ric_notif_,
                                                               timer_factory         timers_,
                                                               srslog::basic_logger& logger_) :
  request(request_), logger(logger_), ric_notif(ric_notif_), timers(timers_)
{
}

void e2_connection_update_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  logger.info("E2AP: Received E2 Connection Update");
  CORO_BEGIN(ctx);
  logger.debug("\"{}\" initialized", name());

  // E2 Connection Update not implemented -> always send failure.
  send_e2_connection_update_failure();

  logger.debug("\"{}\" finalized", name());
  CORO_RETURN();
}

void e2_connection_update_procedure::send_e2_connection_update_ack()
{
  logger.info("E2AP: Sending E2 Connection Update Ack");
  e2_message msg;
  msg.pdu.set_successful_outcome().load_info_obj(ASN1_E2AP_ID_E2CONN_UPD);
  msg.pdu.successful_outcome().value.e2conn_upd_ack()->transaction_id = request->transaction_id;
  ric_notif.on_new_message(msg);
}

void e2_connection_update_procedure::send_e2_connection_update_failure()
{
  logger.info("E2AP: Sending E2 Connection Update Failure");
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_E2CONN_UPD);
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->transaction_id = request->transaction_id;
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->cause_present  = true;
  msg.pdu.unsuccessful_outcome().value.e2conn_upd_fail()->cause.set_transport() =
      cause_transport_e::transport_res_unavailable;
  ric_notif.on_new_message(msg);
}
