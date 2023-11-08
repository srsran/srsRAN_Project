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

#include "srsran/support/executors/task_execution_manager.h"
#include <future>
#include <gtest/gtest.h>

using namespace srsran;

class task_execution_manager_test : public ::testing::Test
{
public:
  task_execution_manager_test() { srslog::init(); }
};

TEST_F(task_execution_manager_test, creation_of_single_task_worker)
{
  using namespace execution_config_helper;
  single_worker cfg{"WORKER", {concurrent_queue_policy::lockfree_spsc, 8}, {{"EXEC"}}, std::chrono::microseconds{100}};

  task_execution_manager mng;
  ASSERT_TRUE(mng.add_execution_context(create_execution_context(cfg)));

  ASSERT_EQ(mng.executors().size(), 1);
  ASSERT_EQ(mng.executors().count("EXEC"), 1);

  // Run single task in created execution environment.
  std::promise<std::string> p;
  std::future<std::string>  f = p.get_future();
  mng.executors().at("EXEC")->execute([&p]() { p.set_value(this_thread_name()); });
  std::string thread_name = f.get();
  ASSERT_EQ(thread_name, "WORKER");
}

TEST_F(task_execution_manager_test, creation_of_task_worker_pool)
{
  using namespace execution_config_helper;
  worker_pool cfg{"WORKER_POOL", 4, {concurrent_queue_policy::locking_mpmc, 8}, {{"EXEC"}}};

  task_execution_manager mng;
  ASSERT_TRUE(mng.add_execution_context(create_execution_context(cfg)));

  ASSERT_EQ(mng.executors().size(), 1);
  ASSERT_EQ(mng.executors().count("EXEC"), 1);

  // Run single task in created execution environment.
  std::promise<std::string> p;
  std::future<std::string>  f = p.get_future();
  mng.executors().at("EXEC")->execute([&p]() { p.set_value(this_thread_name()); });
  std::string thread_name = f.get();
  ASSERT_EQ(thread_name.find("WORKER_POOL#"), 0);
}

TEST_F(task_execution_manager_test, worker_with_queues_of_different_priorities)
{
  using namespace execution_config_helper;
  priority_multiqueue_worker cfg{
      "WORKER",
      {task_queue{concurrent_queue_policy::lockfree_spsc, 8}, task_queue{concurrent_queue_policy::locking_mpsc, 8}},
      std::chrono::microseconds{10},
      {priority_multiqueue_worker::executor{"EXEC1", enqueue_priority::max, true, false},
       priority_multiqueue_worker::executor{"EXEC2", enqueue_priority::min, true, false}}};

  task_execution_manager mng;
  ASSERT_TRUE(mng.add_execution_context(create_execution_context(cfg)));

  ASSERT_EQ(mng.executors().size(), 2);
  ASSERT_EQ(mng.executors().count("EXEC1"), 1);
  ASSERT_EQ(mng.executors().count("EXEC2"), 1);
  ASSERT_NE(mng.executors().at("EXEC1"), mng.executors().at("EXEC2"));

  std::atomic<int> counter{0};
  std::vector<int> execs_called;
  mng.executors().at("EXEC1")->execute([&mng, &execs_called, &counter]() {
    mng.executors().at("EXEC2")->defer([&execs_called, &counter]() {
      execs_called.push_back(2);
      counter++;
    });
    mng.executors().at("EXEC1")->defer([&execs_called, &counter]() {
      execs_called.push_back(1);
      counter++;
    });
  });

  while (counter != 2) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::vector<int> expected{1, 2};
  ASSERT_EQ(execs_called, expected) << "The highest priority executed should have been called first";
}

TEST_F(task_execution_manager_test, decorate_executor_as_synchronous)
{
  using namespace execution_config_helper;
  worker_pool cfg{
      "WORKER", 2, {task_queue{concurrent_queue_policy::locking_mpmc, 8}}, {worker_pool::executor{"EXEC", true, true}}};

  task_execution_manager mng;
  ASSERT_TRUE(mng.add_execution_context(create_execution_context(cfg)));

  ASSERT_EQ(mng.executors().size(), 1);
  ASSERT_EQ(mng.executors().count("EXEC"), 1);

  // Run single task in created execution environment.
  // Note: Given that the executor was decorated as synchronous, the "execute" call will only return once the task
  // has completed.
  bool done = false;
  mng.executors().at("EXEC")->execute([&done]() { done = true; });
  ASSERT_TRUE(done);
}
