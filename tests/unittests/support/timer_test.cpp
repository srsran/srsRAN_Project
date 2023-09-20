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

#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
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
  timer_id_t last_timer_id = timer_id_t::invalid;
};

class unique_timer_manual_tester : public ::testing::Test
{
protected:
  unique_timer_manual_tester(bool blocking_worker = true) : worker(64, blocking_worker)
  {
    logger.set_level(srslog::basic_levels::debug);

    srslog::init();

    // randomize time wheel start.
    unsigned nof_ticks = test_rgen::uniform_int<unsigned>(0, 100);
    for (unsigned i = 0; i != nof_ticks; ++i) {
      tick();
    }
  }

  unique_timer create_timer() { return timer_mng.create_unique_timer(worker); }

  void tick()
  {
    timer_mng.tick();
    worker.run_pending_tasks();
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
  timer_manager         timer_mng;
  manual_task_worker    worker;
};

TEST(unique_timer_test, default_ctor)
{
  unique_timer timer;
  ASSERT_FALSE(timer.is_valid());
  ASSERT_EQ(timer.id(), timer_id_t::invalid);
  ASSERT_FALSE(timer.has_expired());
  ASSERT_FALSE(timer.is_running());
  ASSERT_FALSE(timer.is_set());
  timer.stop(); // no-op.
}

TEST_F(unique_timer_manual_tester, creation)
{
  unique_timer t = this->create_timer();

  ASSERT_TRUE(t.is_valid());
  ASSERT_NE(t.id(), timer_id_t::invalid);
  ASSERT_FALSE(t.is_set());
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.stop(); // no-op.

  ASSERT_EQ(this->timer_mng.nof_timers(), 1);
  ASSERT_EQ(this->timer_mng.nof_running_timers(), 0);
}

TEST_F(unique_timer_manual_tester, destruction)
{
  {
    ASSERT_EQ(this->timer_mng.nof_timers(), 0);
    unique_timer t = this->create_timer();
    ASSERT_EQ(this->timer_mng.nof_timers(), 1);
  }
  // Note: dtor command sent to backend but not yet processed.
  ASSERT_EQ(this->timer_mng.nof_timers(), 1);
  this->tick();
  ASSERT_EQ(this->timer_mng.nof_timers(), 0);
  ASSERT_EQ(this->timer_mng.nof_running_timers(), 0);
}

TEST_F(unique_timer_manual_tester, set_duration)
{
  unique_timer t = this->create_timer();

  timer_duration dur{test_rgen::uniform_int<unsigned>(1, 100)};
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

  timer_duration dur{test_rgen::uniform_int<unsigned>(1, 100)};
  bool           expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();
  ASSERT_TRUE(t.is_set());
  ASSERT_EQ(t.duration(), dur);

  for (unsigned i = 0; i != std::max((unsigned)dur.count(), 1U); ++i) {
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

TEST_F(unique_timer_manual_tester, single_run_and_move)
{
  unique_timer t = this->create_timer();

  timer_duration dur{test_rgen::uniform_int<unsigned>(1, 100)};
  bool           expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();

  unique_timer t2 = std::move(t);
  ASSERT_TRUE(t2.is_running());
  ASSERT_FALSE(t.is_valid());

  for (unsigned i = 0; i != std::max((unsigned)dur.count(), 1U); ++i) {
    ASSERT_TRUE(t2.is_running());
    ASSERT_FALSE(t2.has_expired());

    this->tick();
  }

  ASSERT_FALSE(t2.is_running());
  ASSERT_TRUE(t2.has_expired());
  ASSERT_TRUE(t2.is_set());
  ASSERT_EQ(t2.duration(), dur);
  ASSERT_TRUE(expiry_callback_triggered);
}

TEST_F(unique_timer_manual_tester, move_to_running_timer)
{
  unique_timer t1                        = this->create_timer();
  bool         expiry_callback_triggered = false;
  t1.set(std::chrono::milliseconds{10}, callback_flag_setter(expiry_callback_triggered));
  t1.run();

  unique_timer t2                         = this->create_timer();
  bool         expiry_callback_triggered2 = false;
  t2.set(std::chrono::milliseconds{10}, callback_flag_setter(expiry_callback_triggered2));
  t2.run();

  ASSERT_EQ(timer_mng.nof_timers(), 2);
  ASSERT_EQ(timer_mng.nof_running_timers(), 0);
  timer_mng.tick();
  ASSERT_EQ(timer_mng.nof_running_timers(), 2);

  t1 = std::move(t2);
  ASSERT_EQ(timer_mng.nof_timers(), 2);
  ASSERT_EQ(timer_mng.nof_running_timers(), 2);
  timer_mng.tick();
  ASSERT_EQ(timer_mng.nof_timers(), 1);
  ASSERT_EQ(timer_mng.nof_running_timers(), 1);
}

TEST_F(unique_timer_manual_tester, multiple_timers_with_same_duration_and_timeout)
{
  timer_duration            dur{test_rgen::uniform_int<unsigned>(1, 100)};
  std::vector<unique_timer> timers(test_rgen::uniform_int<unsigned>(1, 10));
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i] = this->create_timer();
    timers[i].set(dur);
    timers[i].run();
  }

  for (unsigned i = 0; i != std::max((unsigned)dur.count(), 1U); ++i) {
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
  timer_duration            dur{timers.size() + test_rgen::uniform_int<unsigned>(1, 100)};
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i] = this->create_timer();
    timers[i].set(dur - timer_duration{i});
  }

  // Each timer starts at a different tick.
  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i].run();

    this->tick();
  }

  for (unsigned i = 0; i != dur.count() - timers.size(); ++i) {
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

  timer_duration dur{test_rgen::uniform_int<unsigned>(1, 100)};
  unsigned       stop_tick                 = test_rgen::uniform_int<unsigned>(0, dur.count() - 1);
  bool           expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();

  // Stop timer before expiry.
  for (unsigned i = 0; i < stop_tick; ++i) {
    ASSERT_TRUE(t.is_running());
    this->tick();
  }
  t.stop();

  // Expiry never triggers.
  for (unsigned i = stop_tick; i != dur.count() + 1; ++i) {
    this->tick();
    ASSERT_FALSE(t.is_running());
    ASSERT_FALSE(t.has_expired());
    ASSERT_FALSE(expiry_callback_triggered);
  }
}

/// \brief This tester verifies that if the status of the timer is updated after the timer has expired in the backend,
/// but before the expiry callback gets processed in the frontend (e.g. due to queue latency), the expiry callback is
/// ignored.
class unique_timer_cancel_already_launched_expiry_callback_tester : public unique_timer_manual_tester
{
protected:
  unique_timer_cancel_already_launched_expiry_callback_tester()
  {
    t   = this->create_timer();
    dur = timer_duration{test_rgen::uniform_int<unsigned>(1, 100)};

    t.set(dur, callback_flag_setter(expiry_callback_triggered));
    t.run();

    // Run until one tick before expiry.
    for (unsigned i = 0; i != dur.count() - 1; ++i) {
      EXPECT_TRUE(t.is_running());
      this->tick();
    }

    // Tick backend timer manager, but do not process frontend executor callbacks yet.
    this->timer_mng.tick();
  }

  void process_pending_expiry_callbacks() { this->worker.run_pending_tasks(); }

  unique_timer   t;
  timer_duration dur;
  bool           expiry_callback_triggered = false;
};

TEST_F(unique_timer_cancel_already_launched_expiry_callback_tester, stop_intercepts_callback)
{
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.stop();
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  process_pending_expiry_callbacks();
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  ASSERT_FALSE(expiry_callback_triggered);
}

TEST_F(unique_timer_cancel_already_launched_expiry_callback_tester, timer_destroy_intercepts_callback)
{
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.reset();
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  process_pending_expiry_callbacks();
  ASSERT_FALSE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  ASSERT_FALSE(expiry_callback_triggered);
}

TEST_F(unique_timer_cancel_already_launched_expiry_callback_tester, timer_run_intercepts_callback)
{
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  t.run();
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  process_pending_expiry_callbacks();
  ASSERT_TRUE(t.is_running());
  ASSERT_FALSE(t.has_expired());
  ASSERT_FALSE(expiry_callback_triggered);
}

TEST_F(unique_timer_manual_tester, calling_run_on_running_timer_restarts_timer)
{
  unique_timer   t = this->create_timer();
  timer_duration dur{1000};
  bool           expiry_callback_triggered = false;
  t.set(dur, callback_flag_setter(expiry_callback_triggered));
  t.run();

  // Restart the timer several times without letting the timer expire.
  unsigned nof_restarts = test_rgen::uniform_int<unsigned>(0, 10);
  for (unsigned i = 0; i != nof_restarts; ++i) {
    unsigned rerun_tick = test_rgen::uniform_int<unsigned>(1, dur.count() - 1);
    for (unsigned n = 0; n != rerun_tick; ++n) {
      this->tick();
    }
    ASSERT_TRUE(t.is_running());
    ASSERT_FALSE(t.has_expired());
    ASSERT_FALSE(expiry_callback_triggered);
    t.run();
  }

  // Let the timer expire by ticking its full duration.
  for (unsigned n = 0; n != dur.count(); ++n) {
    ASSERT_TRUE(t.is_running());
    ASSERT_FALSE(t.has_expired());
    this->tick();
  }
  ASSERT_FALSE(t.is_running());
  ASSERT_TRUE(t.has_expired());
  ASSERT_TRUE(expiry_callback_triggered);
}

class unique_timer_timeout_dispatch_fail_tester : public unique_timer_manual_tester
{
protected:
  unique_timer_timeout_dispatch_fail_tester() : unique_timer_manual_tester(false) {}
};

TEST_F(unique_timer_timeout_dispatch_fail_tester,
       when_timeout_handling_fails_to_be_dispatched_then_postpone_it_to_next_slot)
{
  unsigned                  q_size = worker.max_pending_tasks();
  std::vector<unique_timer> timers(q_size + 1);
  unsigned                  timeout_counter = 0;
  timer_duration            dur{100};

  for (unsigned i = 0; i != timers.size(); ++i) {
    timers[i] = this->create_timer();
    timers[i].set(dur, [&timeout_counter](timer_id_t tid) mutable { timeout_counter++; });
    timers[i].run();
  }

  // Run until completion.
  for (unsigned t = 0; t != dur.count(); ++t) {
    tick();
  }

  // All but one timer should be successfully dispatched
  ASSERT_EQ(timeout_counter, timers.size() - 1);
  ASSERT_EQ(std::count_if(timers.begin(), timers.end(), [](const unique_timer& t) { return t.has_expired(); }),
            timeout_counter);
  auto missing_timer = std::find_if(timers.begin(), timers.end(), [](const unique_timer& t) { return t.is_running(); });
  ASSERT_NE(missing_timer, timers.end());

  // Running one extra slot, schedules the dispatch of the last timer.
  tick();
  ASSERT_TRUE(missing_timer->has_expired());
}

class unique_timer_multithread_tester : public ::testing::Test
{
protected:
  enum class timer_event { start, stop, create, rem };

  void stop()
  {
    frontend_worker.stop();
    backend_worker.stop();
  }

  void run_tick()
  {
    // Generate random event in frontend.
    frontend_worker.push_task_blocking([this]() { run_event(); });

    // Tick backworker
    backend_worker.push_task_blocking([this]() { timer_mng.tick(); });
  }

  void run_timer_creation()
  {
    unique_timer t = timer_mng.create_unique_timer(frontend_exec);
    t.set(timer_duration{100}, [th_id = std::this_thread::get_id(), this](timer_id_t tid) {
      expiry_counter++;
      EXPECT_EQ(std::this_thread::get_id(), th_id);
    });
    timers.push_back(std::move(t));
  }

  void run_event()
  {
    if (timers.empty()) {
      run_timer_creation();
      nof_events_processed++;
      return;
    }

    timer_event event    = (timer_event)test_rgen::uniform_int<unsigned>(0, (unsigned)timer_event::rem);
    unsigned    timer_id = test_rgen::uniform_int<unsigned>(0, timers.size() - 1);
    switch (event) {
      case timer_event::create: {
        run_timer_creation();
      } break;
      case timer_event::rem:
        timers.erase(timers.begin() + timer_id);
        break;
      case timer_event::start:
        timers[timer_id].run();
        break;
      case timer_event::stop:
        timers[timer_id].stop();
        break;
      default:
        report_fatal_error("Unrecognized event");
    }
    nof_events_processed++;
  }

  timer_manager        timer_mng;
  task_worker          backend_worker{"backend", 2048};
  task_worker          frontend_worker{"frontend", 2048};
  task_worker_executor frontend_exec{frontend_worker};

  std::vector<unique_timer> timers;
  std::atomic<unsigned>     nof_events_processed{0};
  unsigned                  expiry_counter = 0;
};

TEST_F(unique_timer_multithread_tester, randomize_timer_operations)
{
  unsigned nof_events = 10000;

  while (nof_events_processed < nof_events) {
    run_tick();
  }

  stop();
  fmt::print("event_counter={} expiry_counter={}\n", nof_events_processed, expiry_counter);
}
