/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "amf_reconnection_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/async_timer.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

async_task<bool> srsran::srs_cu_cp::start_amf_reconnection(ngap_interface&           ngap,
                                                           timer_factory             timers,
                                                           std::chrono::milliseconds reconnection_retry_time)
{
  return launch_async<amf_reconnection_routine>(ngap, timers, reconnection_retry_time);
}

amf_reconnection_routine::amf_reconnection_routine(ngap_interface&           ngap_,
                                                   timer_factory             timers,
                                                   std::chrono::milliseconds reconnection_retry_time_) :
  ngap(ngap_),
  logger(srslog::fetch_basic_logger("CU-CP")),
  amf_tnl_connection_retry_timer(timers.create_timer()),
  reconnection_retry_time(reconnection_retry_time_)
{
}

void amf_reconnection_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
  CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
  while (not ngap.handle_amf_tnl_connection_request()) {
    logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
    CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, ngap.handle_ng_setup_request(/*max_setup_retries*/ 1));

  success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

  if (success) {
    if (logger.debug.enabled()) {
      std::string plmn_list;
      for (const auto& plmn : ngap.get_ngap_context().get_supported_plmns()) {
        plmn_list += plmn.to_string() + " ";
      }

      logger.debug("Reconnected to AMF. Supported PLMNs: {}", plmn_list);
    }

    logger.info("\"{}\" finished successfully", name());

  } else {
    logger.info("\"{}\" failed", name());

    CORO_EARLY_RETURN(false);
  }

  CORO_RETURN(success);
}
