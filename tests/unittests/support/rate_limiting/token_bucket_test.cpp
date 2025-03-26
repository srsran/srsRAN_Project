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
  timer_manager       timers;
  manual_task_worker  worker(1024);
  timer_factory       timers_f{timers, worker};
  token_bucket_config cfg{5, std::chrono::milliseconds(5), 50, timers_f};
  token_bucket        bucket{cfg};

  ASSERT_TRUE(bucket.consume(20));
  ASSERT_FALSE(bucket.consume(60));
  ASSERT_TRUE(bucket.consume(30));

  // Advance time just before the bucket is refilled and confirm bucket is still empty.
  tick_all(timers, worker, 4);
  ASSERT_FALSE(bucket.consume(1));

  // Advance time until bucket is refilled
  tick_all(timers, worker, 1);

  ASSERT_FALSE(bucket.consume(20));
  ASSERT_TRUE(bucket.consume(5));
  ASSERT_FALSE(bucket.consume(1));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
