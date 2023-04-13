/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_disconnect_procedure.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

du_disconnect_procedure::du_disconnect_procedure(du_manager_params& params_, du_ue_manager& ue_mng_) :
  params(params_),
  ue_mng(ue_mng_),
  disconnect_timer(params.services.timers.create_unique_timer(params.services.du_mng_exec))
{
}

void du_disconnect_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  static constexpr std::chrono::milliseconds wait_step{100};

  CORO_BEGIN(ctx);

  // TODO: Call F1 Removal Request.

  // Schedule removal of the context of all UEs.
  for (auto& ue : ue_mng.get_ues()) {
    ue_mng.schedule_async_task(ue->ue_index, ue_mng.handle_ue_delete_request(f1ap_ue_delete_request{ue->ue_index}));
  }

  // Wait for all UEs to be removed.
  for (; wait_time.count() > 0; wait_time -= wait_step) {
    CORO_AWAIT(async_wait_for(disconnect_timer, wait_step));
  }

  CORO_RETURN();
}
