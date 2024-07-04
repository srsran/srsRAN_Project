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
#include "cu_cp/ue_context/e1ap_cu_cp_ue_context.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

constexpr std::chrono::milliseconds bearer_context_release_response_timeout{5000};

bearer_context_release_procedure::bearer_context_release_procedure(const e1ap_message&    command_,
                                                                   ue_index_t             ue_index_,
                                                                   e1ap_ue_context_list&  ue_ctxt_list_,
                                                                   e1ap_message_notifier& e1ap_notif_) :
  command(command_), ue_index(ue_index_), ue_ctxt_list(ue_ctxt_list_), e1ap_notifier(e1ap_notif_)
{
  srsran_assert(ue_ctxt_list.contains(ue_index), "Bearer context does not exist in UE context list.");
}

void bearer_context_release_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  ue_ctxt_list[ue_index].logger.log_debug("\"{}\" initialized", name());

  // Subscribe to respective publisher to receive BEARER CONTEXT RELEASE COMPLETE message.
  transaction_sink.subscribe_to(ue_ctxt_list[ue_index].bearer_ev_mng.context_release_complete,
                                bearer_context_release_response_timeout);

  // Send command to CU-UP.
  send_bearer_context_release_command();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  handle_bearer_context_release_complete();

  /// NOTE: From this point on the UE context is removed and only locally stored variables can be used.

  // Handle response from CU-UP and return bearer index
  CORO_RETURN();
}

void bearer_context_release_procedure::send_bearer_context_release_command()
{
  // send DL RRC message
  e1ap_notifier.on_new_message(command);
}

void bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    ue_ctxt_list[ue_index].logger.log_debug("\"{}\" finalized", name());

  } else {
    ue_ctxt_list[ue_index].logger.log_warning("BearerContextReleaseComplete timeout");
    ue_ctxt_list[ue_index].logger.log_error("\"{}\" failed", name());
  }

  // Remove UE context
  ue_ctxt_list.remove_ue(ue_index);
}