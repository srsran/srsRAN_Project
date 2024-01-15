/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "cu_cp/e1ap_cu_cp_impl.h"
#include "cu_cp/ue_context/e1ap_bearer_transaction_manager.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

bearer_context_release_procedure::bearer_context_release_procedure(const e1ap_message&              command_,
                                                                   e1ap_bearer_transaction_manager& ev_mng_,
                                                                   e1ap_message_notifier&           e1ap_notif_,
                                                                   e1ap_ue_logger&                  logger_) :
  command(command_), ev_mng(ev_mng_), e1ap_notifier(e1ap_notif_), logger(logger_)
{
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.log_debug("\"{}\" initialized", name());

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

void bearer_context_release_procedure::send_bearer_context_release_command()
{
  if (logger.get_basic_logger().debug.enabled()) {
    asn1::json_writer js;
    command.pdu.to_json(js);
    logger.log_debug("Containerized BearerContextReleaseCommand: {}", js.to_string());
  }

  // send DL RRC message
  e1ap_notifier.on_new_message(command);
}

void bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    const asn1::e1ap::bearer_context_release_complete_s& resp = transaction_sink.response();
    logger.log_debug("Received BearerContextReleaseComplete");
    if (logger.get_basic_logger().debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.log_debug("Containerized BearerContextReleaseComplete: {}", js.to_string());
    }

    logger.log_debug("\"{}\" finalized", name());

  } else {
    logger.log_warning("BearerContextReleaseComplete timeout");
    logger.log_error("\"{}\" failed", name());
  }
}