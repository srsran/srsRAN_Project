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

  // Send request to AMF.
  amf_notifier.on_new_message(msg);

  // Await AMF response.
  CORO_AWAIT(transaction_sink);

  // TODO: Handle NG RESET ACKNOWLEDGE message.

  CORO_RETURN();
}
