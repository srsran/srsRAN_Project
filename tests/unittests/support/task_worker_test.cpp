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

#include "srsran/support/executors/task_worker_pool.h"
#include <future>
#include <gtest/gtest.h>

using namespace srsran;

TEST(task_worker_pool, correct_initialization)
{
  task_worker_pool pool{4, 128, "POOL"};
  ASSERT_EQ(pool.nof_workers(), 4);
  ASSERT_EQ(pool.nof_pending_tasks(), 0);
}

TEST(task_worker_pool, worker_pool_runs_single_task)
{
  task_worker_pool pool{4, 128, "POOL"};

  std::promise<void> p;
  std::future<void>  f = p.get_future();
  pool.push_task([&p]() {
    p.set_value();
    fmt::print("Finished in {}\n", this_thread_name());
  });
  f.get();
}
