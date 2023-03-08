/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers2.h"
#include <gtest/gtest.h>

using namespace srsran;

class unique_timer_manual_tester : public ::testing::Test
{
protected:
  unique_timer create_timer() { return timers.create_unique_timer(worker); }

  void tick()
  {
    timers.tick_all();
    worker.run_pending_tasks();
  }

  timer_manager2     timers;
  manual_task_worker worker{64};
};

TEST(unique_timer_test, default_ctor)
{
  unique_timer timer;
  ASSERT_FALSE(timer.is_valid());
  ASSERT_EQ(timer.id(), INVALID_TIMER_ID);
  ASSERT_FALSE(timer.has_expired());
  ASSERT_FALSE(timer.is_running());
  ASSERT_FALSE(timer.is_set());
  timer.stop(); // no-op.
}

TEST_F(unique_timer_manual_tester, creation)
{
  unique_timer t = this->create_timer();

  ASSERT_TRUE(t.is_valid());
  ASSERT_NE(t.id(), INVALID_TIMER_ID);
  ASSERT_FALSE(t.is_set());
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.stop(); // no-op.
}

TEST_F(unique_timer_manual_tester, set_duration)
{
  unique_timer t = this->create_timer();

  unsigned dur = test_rgen::uniform_int<unsigned>(0, 100);
  t.set(dur);
  ASSERT_TRUE(t.is_set());
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  ASSERT_EQ(t.duration(), dur);
  t.stop(); // no-op.
}

TEST_F(unique_timer_manual_tester, single_run)
{
  unique_timer t = this->create_timer();

  unsigned dur = test_rgen::uniform_int<unsigned>(0, 100);
  t.set(dur);
  t.run();
  ASSERT_TRUE(t.is_set());
  ASSERT_EQ(t.duration(), dur);

  for (unsigned i = 0; i != dur; ++i) {
    ASSERT_TRUE(t.is_running());
    ASSERT_FALSE(t.has_expired());

    timers.tick_all();
    worker.run_pending_tasks();
  }

  ASSERT_FALSE(t.is_running());
  ASSERT_TRUE(t.has_expired());
  ASSERT_TRUE(t.is_set());
  ASSERT_EQ(t.duration(), dur);
}
