/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/test_utils.h"
#include <chrono>

using namespace srsran;

void test_async_loop_empty_tasks()
{
  fifo_async_task_scheduler loop{128};
  size_t                    nof_tasks = 10000;

  auto tp = std::chrono::high_resolution_clock::now();

  // Action: Run a lot of empty async tasks.
  for (size_t i = 0; i < nof_tasks; ++i) {
    loop.schedule([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  // Status: The stack shouldn't overflow with so many immediately resumable tasks.

  auto tp2       = std::chrono::high_resolution_clock::now();
  auto diff_usec = std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp);

  fmt::print("Computes {} empty tasks in {} msec.\n", nof_tasks, diff_usec.count() / (float)1000);
}

int main()
{
  test_async_loop_empty_tasks();
  return 0;
}