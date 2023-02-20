/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_release_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

bearer_context_release_procedure::bearer_context_release_procedure(const ue_index_t       ue_index_,
                                                                   const e1ap_message&    command_,
                                                                   e1ap_ue_context_list&  ue_ctxt_list_,
                                                                   e1ap_message_notifier& e1ap_notif_,
                                                                   srslog::basic_logger&  logger_) :
  ue_index(ue_index_), command(command_), ue_ctxt_list(ue_ctxt_list_), e1ap_notifier(e1ap_notif_), logger(logger_)
{
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Subscribe to respective publisher to receive BEARER CONTEXT RELEASE COMPLETE message.
  transaction_sink.subscribe_to(ue_ctxt_list[ue_index].bearer_ev_mng.context_release_complete);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  handle_bearer_context_release_complete();

  // Handle response from CU-UP and return bearer index
  CORO_RETURN();
}

void bearer_context_release_procedure::send_bearer_context_release_command()
{
  if (logger.debug.enabled()) {
    asn1::json_writer js;
    command.pdu.to_json(js);
    logger.debug("Containerized BearerContextReleaseCommand: {}", js.to_string());
  }

  // send DL RRC message
  e1ap_notifier.on_new_message(command);
}

void bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_release_complete_s& resp = transaction_sink.response();
    logger.debug("Received BearerContextReleaseComplete");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized BearerContextReleaseComplete: {}", js.to_string());
    }
    if (command.pdu.init_msg().value.bearer_context_release_cmd()->gnb_cu_cp_ue_e1ap_id.value ==
        resp->gnb_cu_cp_ue_e1ap_id.value) {
      ue_ctxt_list.remove_ue(ue_index);
    }

  } else {
    logger.warning("BearerContextReleaseComplete timeout");
  }
}