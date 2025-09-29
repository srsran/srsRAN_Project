/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_up_reset_procedure.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_up;

e1ap_cu_up_reset_procedure::e1ap_cu_up_reset_procedure(asn1::e1ap::reset_s          reset_msg_,
                                                       e1ap_cu_up_manager_notifier& cu_up_notifier_,
                                                       e1ap_message_notifier&       tx_pdu_notifier_,
                                                       srslog::basic_logger&        logger_) :
  reset_msg(std::move(reset_msg_)), cu_up_notifier(cu_up_notifier_), tx_pdu_notifier(tx_pdu_notifier_), logger(logger_)
{
}

void e1ap_cu_up_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  fmt::println("TEESTTTT");
  // Release CU-UP Bearer contexts.
  // CORO_AWAIT(cu_up_notifier.on_bearer_context_release_command_received(bearer_context_release_cmd));

  CORO_RETURN();
}
