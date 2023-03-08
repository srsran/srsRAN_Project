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

struct callback_flag_setter {
  callback_flag_setter(bool& flag_) : flag(flag_) { flag = false; }

  void operator()(timer_id_t tid)
  {
    flag          = true;
    last_timer_id = tid;
  }

  bool&      flag;
  timer_id_t last_timer_id = INVALID_TIMER_ID;
};

class unique_timer_manual_tester : public ::testing::Test
{
protected:
  unique_timer_manual_tester()
  {
    // randomize time wheel start.
    unsigned nof_ticks = test_rgen::uniform_int<unsigned>(0, 100);
    for (unsigned i = 0; i != nof_ticks; ++i) {
      tick();
    }
  }

  unique_timer create_timer() { return timer_mng.create_unique_timer(worker); }

  void tick()
  {
    timer_mng.tick_all();
    worker.run_pending_tasks();
  }

  timer_manager2     timer_mng;
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

  unsigned dur                       = test_rgen::uniform_int<unsigned>(0, 100);
  bool     expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();
  ASSERT_TRUE(t.is_set());
  ASSERT_EQ(t.duration(), dur);

  for (unsigned i = 0; i != std::max(dur, 1U); ++i) {
    ASSERT_TRUE(t.is_running());
    ASSERT_FALSE(t.has_expired());

    this->tick();
  }

  ASSERT_FALSE(t.is_running());
  ASSERT_TRUE(t.has_expired());
  ASSERT_TRUE(t.is_set());
  ASSERT_EQ(t.duration(), dur);
  ASSERT_TRUE(expiry_callback_triggered);
}

TEST_F(unique_timer_manual_tester, multiple_timers_with_same_duration_and_timeout)
{
  unsigned                  dur = test_rgen::uniform_int<unsigned>(0, 100);
  std::vector<unique_timer> timers(test_rgen::uniform_int<unsigned>(1, 10));
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i] = this->create_timer();
    timers[i].set(dur);
    timers[i].run();
  }

  for (unsigned i = 0; i != std::max(dur, 1U); ++i) {
    ASSERT_TRUE(std::all_of(timers.begin(), timers.end(), [](const auto& t) { return t.is_running(); }));
    ASSERT_TRUE(std::none_of(timers.begin(), timers.end(), [](const auto& t) { return t.has_expired(); }));

    this->tick();
  }

  // All timers finish at the same time.
  ASSERT_TRUE(std::none_of(timers.begin(), timers.end(), [](const auto& t) { return t.is_running(); }));
  ASSERT_TRUE(std::all_of(timers.begin(), timers.end(), [](const auto& t) { return t.has_expired(); }));
}

TEST_F(unique_timer_manual_tester, multiple_timers_with_same_timeout_but_different_durations)
{
  std::vector<unique_timer> timers(test_rgen::uniform_int<unsigned>(1, 10));
  unsigned                  dur = timers.size() + test_rgen::uniform_int<unsigned>(1, 100);
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i] = this->create_timer();
    timers[i].set(dur - i);
  }

  // Each timer starts at a different tick.
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i].run();

    this->tick();
  }

  for (unsigned i = 0; i != dur - timers.size(); ++i) {
    ASSERT_TRUE(std::all_of(timers.begin(), timers.end(), [](const auto& t) { return t.is_running(); }));
    ASSERT_TRUE(std::none_of(timers.begin(), timers.end(), [](const auto& t) { return t.has_expired(); }));

    this->tick();
  }

  // All timers finish at the same tick.
  ASSERT_TRUE(std::none_of(timers.begin(), timers.end(), [](const auto& t) { return t.is_running(); }));
  ASSERT_TRUE(std::all_of(timers.begin(), timers.end(), [](const auto& t) { return t.has_expired(); }));
}

TEST_F(unique_timer_manual_tester, single_run_and_stop_does_not_trigger_expiry)
{
  unique_timer t = this->create_timer();

  unsigned dur = test_rgen::uniform_int<unsigned>(1, 100), stop_tick = test_rgen::uniform_int<unsigned>(0, dur - 1);
  bool     expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();

  // Stop timer before expiry.
  for (unsigned i = 0; i < stop_tick; ++i) {
    ASSERT_TRUE(t.is_running());
    this->tick();
  }
  t.stop();

  // Expiry never triggers.
  for (unsigned i = stop_tick; i != dur + 1; ++i) {
    this->tick();
    ASSERT_FALSE(t.is_running());
    ASSERT_FALSE(t.has_expired());
    ASSERT_FALSE(expiry_callback_triggered);
  }
}

TEST_F(unique_timer_manual_tester, single_run_and_stop_after_backend_expiry_but_before_callback_run)
{
  unique_timer t = this->create_timer();

  unsigned dur                       = test_rgen::uniform_int<unsigned>(1, 100);
  bool     expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();

  // Run until one tick before expiry.
  for (unsigned i = 0; i != dur - 1; ++i) {
    ASSERT_TRUE(t.is_running());
    this->tick();
  }

  // Tick timers, but do not run callback before timer stop.
  this->timer_mng.tick_all();
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.stop();
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  this->worker.run_pending_tasks(); // the pending expiry callback dispatch should be ignored.

  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  ASSERT_FALSE(expiry_callback_triggered);
}
