/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/support/rate_limiting/token_bucket.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

static void tick_all(timer_manager& timers, manual_task_worker& worker, uint32_t nof_ticks)
{
  for (uint32_t i = 0; i < nof_ticks; i++) {
    timers.tick();
    worker.run_pending_tasks();
  }
}

/// \brief Test token bucket consume and re-fill.
TEST(token_bucket_test, consume_and_refill_test)
{
  timer_manager      timers;
  manual_task_worker worker(1024);
  auto               timers_f = timer_factory{timers, worker};
  auto               bucket   = token_bucket(5, std::chrono::milliseconds(5), 50, timers_f);

  ASSERT_TRUE(bucket.consume(20));
  ASSERT_FALSE(bucket.consume(60));
  ASSERT_TRUE(bucket.consume(30));

  // Re-fill bucket.
  tick_all(timers, worker, 5);

  ASSERT_FALSE(bucket.consume(20));
  ASSERT_TRUE(bucket.consume(5));
  ASSERT_FALSE(bucket.consume(1));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
