/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_loss_routine.h"
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
  return launch_async<amf_connection_loss_routine>(ngap, timers, reconnection_retry_time);
}

amf_connection_loss_routine::amf_connection_loss_routine(ngap_interface&           ngap_,
                                                         timer_factory             timers,
                                                         std::chrono::milliseconds reconnection_retry_time_) :
  ngap(ngap_),
  logger(srslog::fetch_basic_logger("CU-CP")),
  amf_tnl_connection_retry_timer(timers.create_timer()),
  reconnection_retry_time(reconnection_retry_time_)
{
}

void amf_connection_loss_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  while (not ngap.handle_amf_tnl_connection_request()) {
    logger.info("TNL connection establishment to AMF failed. Retrying in {}...", reconnection_retry_time);
    CORO_AWAIT(async_wait_for(amf_tnl_connection_retry_timer, reconnection_retry_time));
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, ngap.handle_ng_setup_request(/*max_setup_retries*/ 1));

  success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

  if (success) {
    std::string plmn_list;
    for (const auto& plmn : ngap.get_ngap_context().get_supported_plmns()) {
      plmn_list += plmn.to_string() + " ";
    }

    logger.info("Reconnected to AMF. Supported PLMNs: {}", plmn_list);
  } else {
    logger.error("Failed to reconnect to AMF");
    CORO_EARLY_RETURN(false);
  }

  CORO_RETURN(success);
}
