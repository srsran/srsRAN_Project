/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

TEST_F(task_execution_manager_test, decorate_executor_as_synchronous)
{
  using namespace execution_config_helper;
  priority_multiqueue cfg{"WORKER",
                          {task_queue{concurrent_queue_policy::locking_mpmc, 8}},
                          std::chrono::microseconds{10},
                          {priority_multiqueue::executor{"EXEC", 0, true, nullptr}}};

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
