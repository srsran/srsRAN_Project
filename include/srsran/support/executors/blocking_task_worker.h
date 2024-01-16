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

#pragma once

#include "task_executor.h"
#include "srsran/adt/blocking_queue.h"

namespace srsran {

/// \brief Contrarily to other type of workers, this worker runs in the same thread where run() is called.
/// run() is blocking.
class blocking_task_worker final : public task_executor
{
public:
  blocking_task_worker(size_t q_size) : pending_tasks(q_size) {}

  bool execute(unique_task task) override
  {
    pending_tasks.push_blocking(std::move(task));
    return true;
  }

  bool defer(unique_task task) override
  {
    execute(std::move(task));
    return true;
  }

  void request_stop()
  {
    execute([this]() {
      if (not pending_tasks.is_stopped()) {
        pending_tasks.stop();
      }
    });
  }

  void run()
  {
    while (true) {
      bool        success;
      unique_task t = pending_tasks.pop_blocking(&success);
      if (not success) {
        break;
      }
      t();
    }
  }

private:
  blocking_queue<unique_task> pending_tasks;
};

} // namespace srsran
