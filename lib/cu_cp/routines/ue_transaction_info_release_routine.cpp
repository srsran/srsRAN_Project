/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_transaction_info_release_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_transaction_info_release_routine::ue_transaction_info_release_routine(const std::vector<ue_index_t>& ues_to_release_,
                                                                         ue_manager&                    ue_mng_,
                                                                         cu_cp_ue_removal_handler& ue_rem_handler_) :
  ues_to_release(ues_to_release_), ue_mng(ue_mng_), ue_rem_handler(ue_rem_handler_)
{
}

void ue_transaction_info_release_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Run NG Reset to signal to the AMF the reset UEs.
  // TODO

  // Launch removal procedure for the provided UEs.
  for (ue_index_t ue_idx : ues_to_release) {
    launch_ue_removal(ue_idx);
  }

  // Wait for all UEs to be removed.
  CORO_AWAIT(all_ues_reset);

  CORO_RETURN();
}

void ue_transaction_info_release_routine::launch_ue_removal(ue_index_t ue_idx)
{
  async_task<void> ue_rem_task = launch_async([this, ue_idx](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Launch UE removal routine and wait for its completion.
    CORO_AWAIT(ue_rem_handler.handle_ue_removal_request(ue_idx));

    // If it is the last UE to be removed, notify the completion.
    if (--ues_remaining_count == 0) {
      all_ues_reset.set();
    }

    CORO_RETURN();
  });
  ue_mng.get_task_sched().handle_ue_async_task(ue_idx, std::move(ue_rem_task));
}