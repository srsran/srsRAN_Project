/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ng_reset_procedure.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::ngap;

constexpr std::chrono::milliseconds ng_reset_response_timeout{5000};

ng_reset_procedure::ng_reset_procedure(const ngap_message&       msg_,
                                       ngap_message_notifier&    amf_notif_,
                                       ngap_transaction_manager& ev_mng_,
                                       srslog::basic_logger&     logger_) :
  msg(msg_), amf_notifier(amf_notif_), ev_mng(ev_mng_), logger(logger_)
{
}

void ng_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" started...", name());

  // Subscribe to respective publisher to receive NG RESET ACKNOWLEDGE message.
  transaction_sink.subscribe_to(ev_mng.ng_reset_outcome, ng_reset_response_timeout);

  // Forward message to AMF.
  if (!amf_notifier.on_new_message(msg)) {
    logger.error("AMF notifier is not set. Cannot send NGReset");
    CORO_EARLY_RETURN();
  }

  // Await AMF response.
  CORO_AWAIT(transaction_sink);

  // TODO: Handle NG RESET ACKNOWLEDGE message.

  CORO_RETURN();
}
