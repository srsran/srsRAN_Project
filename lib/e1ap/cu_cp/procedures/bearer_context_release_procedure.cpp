/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "bearer_context_release_procedure.h"
#include "cu_cp/e1ap_cu_cp_impl.h"
#include "cu_cp/ue_context/e1ap_bearer_transaction_manager.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::e1ap;

constexpr std::chrono::milliseconds bearer_context_release_response_timeout{5000};

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
  transaction_sink.subscribe_to(ev_mng.context_release_complete, bearer_context_release_response_timeout);

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
  // send DL RRC message
  e1ap_notifier.on_new_message(command);
}

void bearer_context_release_procedure::handle_bearer_context_release_complete()
{
  if (transaction_sink.successful()) {
    logger.log_debug("\"{}\" finalized", name());

  } else {
    logger.log_warning("BearerContextReleaseComplete timeout");
    logger.log_error("\"{}\" failed", name());
  }
}