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
  execution_context_description ctxt = {
      "WORKER",
      8,
      os_thread_realtime_priority::no_realtime(),
      {},
      execution_context_description::single{concurrent_queue_policy::lockfree_spsc, std::chrono::microseconds{100}},
      {{"EXEC"}}};
  const task_execution_manager_config cfg{{ctxt}};

  auto mng = task_execution_manager::create(cfg);
  ASSERT_NE(mng, nullptr);

  ASSERT_EQ(mng->executors().size(), 1);
  ASSERT_EQ(mng->executors().count("EXEC"), 1);

  // Run single task in created execution environment.
  std::promise<std::string> p;
  std::future<std::string>  f = p.get_future();
  mng->executors().at("EXEC")->execute([&p]() { p.set_value(this_thread_name()); });
  std::string thread_name = f.get();
  ASSERT_EQ(thread_name, "WORKER");
}

TEST_F(task_execution_manager_test, creation_of_task_worker_pool)
{
  execution_context_description       ctxt = {"WORKER_POOL",
                                              8,
                                              os_thread_realtime_priority::no_realtime(),
                                              {},
                                              execution_context_description::pool{4},
                                              {{"EXEC"}}};
  const task_execution_manager_config cfg{{ctxt}};

  auto mng = task_execution_manager::create(cfg);
  ASSERT_NE(mng, nullptr);

  ASSERT_EQ(mng->executors().size(), 1);
  ASSERT_EQ(mng->executors().count("EXEC"), 1);

  // Run single task in created execution environment.
  std::promise<std::string> p;
  std::future<std::string>  f = p.get_future();
  mng->executors().at("EXEC")->execute([&p]() { p.set_value(this_thread_name()); });
  std::string thread_name = f.get();
  ASSERT_EQ(thread_name.find("WORKER_POOL#"), 0);
}
