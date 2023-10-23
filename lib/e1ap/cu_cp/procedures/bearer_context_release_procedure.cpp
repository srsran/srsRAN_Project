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

  logger.debug("ue={}: \"{}\" initialized.", ue_index, name());

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

    logger.debug("ue={}: \"{}\" finalized.", ue_index, name());

  } else {
    logger.warning("BearerContextReleaseComplete timeout");
    logger.error("ue={}: \"{}\" failed.", ue_index, name());
  }
}