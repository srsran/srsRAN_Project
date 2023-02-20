/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"
#include <chrono>

using namespace srsran;

void test_async_loop_empty_tasks()
{
  async_task_sequencer loop{128};
  size_t               nof_tasks = 10000;

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