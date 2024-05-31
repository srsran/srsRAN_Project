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

#include "ue_transaction_info_release_routine.h"
#include "srsran/adt/scope_exit.h"

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
  ues_remaining_count = ues_to_release.size();
  for (ue_index_t ue_idx : ues_to_release) {
    launch_ue_removal(ue_idx);
  }

  // Wait for all UEs to be removed.
  if (ues_remaining_count != 0) {
    CORO_AWAIT(all_ues_reset);
  }

  CORO_RETURN();
}

void ue_transaction_info_release_routine::launch_ue_removal(ue_index_t ue_idx)
{
  // Create task to be called after a UE removal.
  auto on_ue_removal = make_scope_exit([this]() {
    // If it is the last UE to be removed, notify the completion.
    if (--ues_remaining_count == 0) {
      all_ues_reset.set();
    }
  });

  auto* ue_it = ue_mng.find_ue(ue_idx);
  if (ue_it == nullptr) {
    // UE not found. The "on_ue_removal" will still decrement the counter.
    return;
  }

  // Cancel any pending transactions.
  ue_rem_handler.handle_pending_ue_task_cancellation(ue_idx);

  async_task<void> ue_rem_task =
      launch_async([this, ue_idx, on_ue_removal = std::move(on_ue_removal)](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);

        // Launch UE removal routine and wait for its completion.
        CORO_AWAIT(ue_rem_handler.handle_ue_removal_request(ue_idx));

        CORO_RETURN();
      });
  ue_mng.get_task_sched().handle_ue_async_task(ue_idx, std::move(ue_rem_task));
}