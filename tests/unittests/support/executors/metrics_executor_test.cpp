/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/metrics_executor.h"
#include <gtest/gtest.h>

using namespace srsran;

struct test_logger {
  template <typename... Args>
  void info(const char* fmtstr, Args&&... args)
  {
    reports.emplace_back(fmt::format(fmtstr, std::forward<Args>(args)...));
  }

  std::vector<std::string> reports;
};

TEST(metrics_executor_test, report_logged_within_given_period)
{
  test_logger               logger;
  manual_task_worker        worker{256};
  std::chrono::milliseconds period{100};
  auto                      metrics_exec  = make_metrics_executor("test", worker, logger, period);
  auto                      test_start_tp = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() - test_start_tp < period) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ASSERT_TRUE(metrics_exec->execute([]() {}));
  worker.run_pending_tasks();

  ASSERT_EQ(logger.reports.size(), 1);
  std::string result = logger.reports[0];
  fmt::print("Result: {}\n", result);

  ASSERT_EQ(result.find("\"test\" executor metrics: "), 0);
  ASSERT_GT(result.find("nof_executes=1 "), 0);
  ASSERT_GT(result.find("nof_defers=0 "), 0);
}
