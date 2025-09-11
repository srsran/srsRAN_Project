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

#pragma once

#include "../du_ue/du_ue_manager.h"
#include "srsran/adt/scope_exit.h"
#include "srsran/ran/du_types.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/manual_event.h"
#include <vector>

namespace srsran {
namespace srs_du {

namespace detail {

/// Helper task to run a coroutine for different UEs and wait for all of them to complete.
template <typename TaskFactory>
class await_all_ues_impl
{
public:
  await_all_ues_impl(du_ue_manager& ue_mng_, span<const du_ue_index_t> ues_to_update_, TaskFactory ue_task_factory_) :
    ue_mng(ue_mng_),
    ue_task_factory(std::move(ue_task_factory_)),
    ues_to_update(ues_to_update_.begin(), ues_to_update_.end())
  {
  }

  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);

    // Launch tasks in the respective UEs task schedulers.
    launch_ue_tasks();

    // Await all tasks to complete.
    CORO_AWAIT(all_ue_tasks_completed);

    CORO_RETURN();
  }

private:
  void launch_ue_tasks()
  {
    // Check how many UEs need to be updated.
    remaining_ues = ues_to_update.size();
    if (remaining_ues == 0) {
      // Fast path: No UEs to update. Flag completion.
      all_ue_tasks_completed.set();
      return;
    }

    // Launch UE tasks in their respective task scheduler.
    for (unsigned ue_count = 0; ue_count != remaining_ues; ++ue_count) {
      ue_mng.schedule_async_task(ues_to_update[ue_count], launch_ue_task(ues_to_update[ue_count]));
    }
  }

  async_task<void> launch_ue_task(du_ue_index_t ue_index)
  {
    // RAII object to decrement remaining UEs and flag completion if this is the last one.
    auto dec_remaining_ues = make_scope_exit([this]() {
      remaining_ues--;
      if (remaining_ues == 0) {
        // This is the last task to remove UEs. Flag completion.
        all_ue_tasks_completed.set();
      }
    });

    return launch_async(
        [this, ue_index, on_finish = std::move(dec_remaining_ues)](coro_context<async_task<void>>& ctx) {
          CORO_BEGIN(ctx);

          if (cur_ue = ue_mng.find_ue(ue_index); cur_ue != nullptr) {
            CORO_AWAIT(ue_task_factory(*cur_ue));
          }

          CORO_RETURN();
        });
  }

  du_ue_manager& ue_mng;
  TaskFactory    ue_task_factory;

  std::vector<du_ue_index_t> ues_to_update;

  unsigned          remaining_ues = 0;
  manual_event_flag all_ue_tasks_completed;
  du_ue*            cur_ue = nullptr;
};

} // namespace detail

/// \brief Runs an async_task in the respective task scheduler of a list of UEs.
template <typename AsyncTaskFactory>
auto await_all_ues(du_ue_manager&            ue_mng,
                   span<const du_ue_index_t> ues_to_update,
                   AsyncTaskFactory          ue_task_factory) -> async_task<void>
{
  return launch_async<detail::await_all_ues_impl<AsyncTaskFactory>>(ue_mng, ues_to_update, std::move(ue_task_factory));
}

/// \brief Runs an async_task in the respective task scheduler of all UEs.
template <typename AsyncTaskFactory>
auto await_all_ues(du_ue_manager& ue_mng, AsyncTaskFactory ue_task_factory) -> async_task<void>
{
  // Fill list of UEs to update.
  const auto&                ue_db = ue_mng.get_du_ues();
  std::vector<du_ue_index_t> ues_to_update;
  ues_to_update.reserve(ue_db.size());
  for (const auto& u : ue_db) {
    ues_to_update.push_back(u.ue_index);
  }
  return await_all_ues(ue_mng, ues_to_update, std::forward<AsyncTaskFactory>(ue_task_factory));
}

} // namespace srs_du
} // namespace srsran
