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
    const e1ap_bearer_context_release_command_message& command_,
    e1_message_notifier&                               e1_notif_,
    e1_event_manager&                                  ev_mng_,
    srslog::basic_logger&                              logger_) :
  command(command_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_release_procedure::operator()(
    coro_context<async_task<e1ap_bearer_context_release_complete_message>>& ctx)
{
  CORO_BEGIN(ctx);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT_VALUE(e1_bearer_ctxt_rel_outcome, ev_mng.e1ap_bearer_context_release_complete);

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

e1ap_bearer_context_release_complete_message
e1_bearer_context_release_procedure::create_bearer_context_release_complete()
{
  e1ap_bearer_context_release_complete_message res{};

  logger.info("Received E1AP Bearer Context Release Complete.");
  res.msg = *e1_bearer_ctxt_rel_outcome;

  return res;
}