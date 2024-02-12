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

#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(sync_task_worker, task_is_synced)
{
  task_worker w{"WORKER", 1};
  auto        exec      = make_task_executor(w);
  auto        sync_exec = make_sync_executor(exec);

  std::atomic<unsigned> counter{0};
  for (unsigned i = 0; i != 10; ++i) {
    ASSERT_TRUE(sync_exec->execute([&counter]() { ++counter; }));
    ASSERT_EQ(counter, i + 1);
  }

  w.stop();
}

TEST(sync_task_worker, multiple_producers_sync_their_tasks)
{
  task_worker w{"WORKER", 10};
  auto        exec      = make_task_executor(w);
  auto        sync_exec = make_sync_executor(exec);

  std::atomic<unsigned> counter{0};
  auto                  f = [&counter, &sync_exec]() {
    for (unsigned i = 0; i != 10; ++i) {
      unsigned prev_count = ++counter;
      unsigned result;
      ASSERT_TRUE(sync_exec->execute([prev_count, &result]() { result = prev_count + 1; }));
      ASSERT_EQ(result, prev_count + 1);
    }
  };

  std::thread t1(f);
  std::thread t2(f);
  t1.join();
  t2.join();

  w.stop();
}
