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

e1_bearer_context_release_procedure::e1_bearer_context_release_procedure(const e1_message&                command_,
                                                                         e1_message_notifier&             e1_notif_,
                                                                         e1ap_bearer_transaction_manager& ev_mng_,
                                                                         srslog::basic_logger&            logger_) :
  command(command_), e1_notifier(e1_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void e1_bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT RELEASE COMPLETE message.
  transaction_sink.subscribe_to(ev_mng.context_release_complete);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  handle_bearer_context_release_complete();

  // Handle response from CU-UP and return bearer index
  CORO_RETURN();
}

void e1_bearer_context_release_procedure::send_bearer_context_release_command()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    command.pdu.to_json(js);
    logger.debug("Containerized Bearer Context Release Command message: {}", js.to_string());
  }

  // send DL RRC message
  e1_notifier.on_new_message(command);
}

void e1_bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_release_complete_s& resp = transaction_sink.response();
    logger.info("Received E1AP Bearer Context Release Complete.");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized Bearer Context Release Complete message: {}", js.to_string());
    }
  } else {
    logger.warning("E1AP Bearer Context Release Complete timeout.");
  }
}