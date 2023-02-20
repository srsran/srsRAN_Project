/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
