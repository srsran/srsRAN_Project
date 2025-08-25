/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_reset_procedure.h"
#include "../du_ue/du_ue_manager.h"
#include "ue_deletion_procedure.h"

using namespace srsran;
using namespace srs_du;

du_ue_reset_procedure::du_ue_reset_procedure(const std::vector<du_ue_index_t>& ues_to_reset_,
                                             du_ue_manager&                    ue_mng_,
                                             const du_manager_params&          du_params_,
                                             bool                              trigger_f1_reset_) :
  ues_to_reset(ues_to_reset_), ue_mng(ue_mng_), du_params(du_params_), trigger_f1_reset(trigger_f1_reset_)
{
}

void du_ue_reset_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Launch tasks to remove UEs in their respective schedulers.
  launch_rem_ues_tasks();

  // Wait for all removal tasks to complete.
  CORO_AWAIT(complete_flag);

  if (trigger_f1_reset) {
    // Trigger F1 Reset towards CU.
    CORO_AWAIT(du_params.f1ap.conn_mng.handle_f1_reset_request(f1_reset_request{ues_to_reset}));
  }

  CORO_RETURN();
}

void du_ue_reset_procedure::launch_rem_ues_tasks()
{
  if (ues_to_reset.empty()) {
    // Need to delete all UEs. Update ues_to_reset with current UEs.
    auto& ue_db = ue_mng.get_du_ues();
    ues_to_reset.reserve(ue_db.size());
    for (const auto& u : ue_db) {
      ues_to_reset.push_back(u.ue_index);
    }
  }

  ue_rem = ues_to_reset.size();
  for (unsigned ue_count = 0; ue_count != ues_to_reset.size(); ++ue_count) {
    ue_mng.schedule_async_task(ues_to_reset[ue_count], launch_ue_rem_task(ues_to_reset[ue_count]));
  }
}

async_task<void> du_ue_reset_procedure::launch_ue_rem_task(du_ue_index_t ue_index)
{
  return launch_async([this, ue_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    CORO_AWAIT(launch_async<ue_deletion_procedure>(ue_index, ue_mng, du_params));

    ue_rem--;
    if (ue_rem == 0) {
      // This is the last task to remove UEs. Flag completion.
      complete_flag.set();
    }

    CORO_RETURN();
  });
}
