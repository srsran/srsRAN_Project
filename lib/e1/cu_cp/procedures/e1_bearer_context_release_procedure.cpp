/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1_bearer_context_release_procedure.h"

using namespace srsgnb;
using namespace srsgnb::srs_cu_cp;
using namespace asn1::e1ap;

e1_bearer_context_release_procedure::e1_bearer_context_release_procedure(
    const e1ap_bearer_context_release_command& command_,
    e1_message_notifier&                       e1_notif_,
    e1ap_bearer_transaction_manager&           ev_mng_,
    srslog::basic_logger&                      logger_) :
  command(command_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_release_procedure::operator()(
    coro_context<async_task<e1ap_bearer_context_release_complete>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT RELEASE COMPLETE message.
  transaction_sink.subscribe_to(ev_mng.context_release_complete);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from CU-UP and return bearer index
  CORO_RETURN(create_bearer_context_release_complete());
}

void e1_bearer_context_release_procedure::send_bearer_context_release_command()
{
  // Pack message into PDU
  e1_message e1_bearer_ctxt_rel_msg;
  e1_bearer_ctxt_rel_msg.pdu.set_init_msg();
  e1_bearer_ctxt_rel_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_RELEASE);
  e1_bearer_ctxt_rel_msg.pdu.init_msg().value.bearer_context_release_cmd() = command.msg;

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    e1_bearer_ctxt_rel_msg.pdu.to_json(js);
    logger.debug("Containerized Bearer Context Release Command message: {}", js.to_string());
  }

  // send DL RRC message
  e1_notifier.on_new_message(e1_bearer_ctxt_rel_msg);
}

e1ap_bearer_context_release_complete e1_bearer_context_release_procedure::create_bearer_context_release_complete()
{
  e1ap_bearer_context_release_complete res{};

  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_release_complete_s& resp = transaction_sink.response();
    logger.info("Received E1AP Bearer Context Release Complete.");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized Bearer Context Release Complete message: {}", js.to_string());
    }
    res.msg = resp;
  } else {
    logger.warning("E1AP Bearer Context Release Complete timeout.");
  }

  return res;
}