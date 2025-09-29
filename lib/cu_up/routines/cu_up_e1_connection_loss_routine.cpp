/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_e1_connection_loss_routine.h"
#include "initial_cu_up_setup_routine.h"
#include "srsran/support/async/async_timer.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_up;

cu_up_e1_connection_loss_routine::cu_up_e1_connection_loss_routine(gnb_cu_up_id_t           cu_up_id_,
                                                                   std::string              cu_up_name_,
                                                                   std::string              plmn_,
                                                                   std::atomic<bool>&       stop_command_,
                                                                   e1ap_connection_manager& e1ap_conn_mng_,
                                                                   ue_manager&              ue_mng_,
                                                                   timer_manager&           timers,
                                                                   task_executor&           ctrl_exec) :
  cu_up_id(cu_up_id_),
  cu_up_name(std::move(cu_up_name_)),
  plmn(std::move(plmn_)),
  stop_command(stop_command_),
  retry_timer(timers.create_unique_timer(ctrl_exec)),
  e1ap_conn_mng(e1ap_conn_mng_),
  ue_mng(ue_mng_),
  logger(srslog::fetch_basic_logger("CU-UP"))
{
}

void cu_up_e1_connection_loss_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("\"{}\" initialized.", name());

  CORO_AWAIT(ue_mng.remove_all_ues());

  // Attempt a new E1 setup connection.
  for (;;) {
    CORO_AWAIT_VALUE(reconnected, launch_async<initial_cu_up_setup_routine>(cu_up_id, cu_up_name, plmn, e1ap_conn_mng));
    if (reconnected || stop_command) {
      break;
    }
    CORO_AWAIT(async_wait_for(retry_timer, std::chrono::milliseconds{1000}));
  }
  CORO_RETURN();
}
