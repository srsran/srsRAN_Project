/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/test_utils.h"
#include "srsran/support/timers.h"
#include <condition_variable>
#include <random>
#include <thread>

using namespace srsran;

static void timers_test1()
{
  test_delimit_logger logger{"Test unique_timer interface"};

  timer_manager           timers;
  timer_tick_difference_t dur = 5;

  {
    // TEST: Default ctor places unique_timer in correct state.
    unique_timer t = timers.create_unique_timer();
    TESTASSERT(!t.is_running() && !t.has_expired());
    TESTASSERT(t.id() == 0);
    unique_timer t2 = timers.create_unique_timer();
    TESTASSERT(!t2.is_running() && !t2.has_expired());
    TESTASSERT(t2.id() == 1);
    TESTASSERT(timers.nof_timers() == 2);

    // TEST: Run multiple times with the same duration.
    bool callback_called = false;
    t.set(dur, [&callback_called](timer_id_t tid) { callback_called = true; });
    for (unsigned runs = 0; runs != 3; ++runs) {
      callback_called = false;
      TESTASSERT(!t.is_running());
      t.run();
      TESTASSERT(t.is_running() && !t.has_expired());
      for (timer_tick_difference_t i = 0; i != (dur - 1); ++i) {
        timers.tick_all();
        TESTASSERT(t.is_running() && !t.has_expired());
      }
      timers.tick_all();
      TESTASSERT(!t.is_running() && t.has_expired());
      TESTASSERT(callback_called);
    }

    // TEST: Interrupt a timer. check if callback was called.
    callback_called = false;
    t.run();
    timers.tick_all();
    TESTASSERT(t.is_running());
    t.stop();
    TESTASSERT(!t.is_running());
    for (timer_tick_difference_t i = 0; i != dur; ++i) {
      timers.tick_all();
      TESTASSERT(!t.is_running());
    }
    TESTASSERT(!callback_called);

    // TEST: Call timer::run() when it is already running. Check if duration gets extended.
    callback_called = false;
    t.run();
    timers.tick_all();
    TESTASSERT(t.is_running());
    t.run(); // re-run
    for (timer_tick_difference_t i = 0; i != (dur - 1); ++i) {
      timers.tick_all();
      TESTASSERT(t.is_running());
    }
    timers.tick_all();
    TESTASSERT(!t.is_running());
    TESTASSERT(callback_called);

    // TEST: Ordering of timers is respected.
    unique_timer t3 = timers.create_unique_timer();
    TESTASSERT(t3.id() == 2);
    int  first_id = -1, last_id = -1;
    auto callback = [&first_id, &last_id](timer_id_t id) {
      if (first_id < 0) {
        printf("First timer id=%d\n", id);
        first_id = id;
      }
      last_id = id;
    };
    t.set(4, callback);
    t2.set(2, callback);
    t3.set(6, callback);
    t.run();
    t2.run();
    t3.run();
    for (unsigned i = 0; i != 5; ++i) {
      timers.tick_all();
      TESTASSERT(i >= 3 or t.is_running());
      TESTASSERT(i >= 1 or t2.is_running());
      TESTASSERT(t3.is_running());
    }
    timers.tick_all();
    TESTASSERT(t.has_expired() && t2.has_expired() && t3.has_expired());
    TESTASSERT(first_id == 1);
    TESTASSERT(last_id == 2);
  }
  // TEST: Timer dtor is called and removes "timer" from "timers".
  TESTASSERT(timers.nof_timers() == 0);
}

/// Description:
/// - calling stop() early, forbids the timer from getting expired.
/// - calling stop() after timer has expired should be a noop.
static void timers_test2()
{
  test_delimit_logger     logger{"Test unique_timer stop() method"};
  timer_manager           timers;
  timer_tick_difference_t duration = 2;

  auto utimer  = timers.create_unique_timer();
  auto utimer2 = timers.create_unique_timer();
  utimer.set(duration);
  utimer2.set(duration);

  // TEST 1: Call utimer.stop() early and check if timer expires.
  utimer.run();
  utimer2.run();
  TESTASSERT(utimer.is_running() && !utimer.has_expired());
  utimer.stop();
  TESTASSERT(!utimer.is_running() && !utimer.has_expired());

  for (unsigned i = 0; i != 5; ++i) {
    timers.tick_all();
  }
  TESTASSERT(!utimer.has_expired());
  TESTASSERT(utimer2.has_expired());

  // TEST 2: Call utimer.stop() after it expires and assert it is still expired.
  utimer2.stop();
  TESTASSERT(utimer2.has_expired());
}

/// Description:
/// - Setting a new duration while the timer is already running should not stop timer, && should extend timeout.
static void timers_test3()
{
  test_delimit_logger     logger{"Test running unique_timer duration extension"};
  timer_manager           timers;
  timer_tick_difference_t duration = 5;

  auto utimer = timers.create_unique_timer();
  utimer.set(duration);
  utimer.run();

  for (unsigned i = 0; i != (2 * duration + 1); ++i) {
    timers.tick_all();
    if ((i % 2) == 0) {
      // Extends lifetime.
      utimer.set(duration);
    }
    TESTASSERT(utimer.is_running());
  }
  for (unsigned i = 0; i != (duration - 1); ++i) {
    timers.tick_all();
    TESTASSERT(utimer.is_running());
  }
  timers.tick_all();
  TESTASSERT(!utimer.is_running() && utimer.has_expired());
}

namespace {
struct timers_test4_ctxt {
  std::vector<unique_timer>     timers;
  bool                          finished{false};
  std::mutex                    mutex;
  std::condition_variable       cvar1, cvar2;
  const timer_tick_difference_t duration = 1000;
};
} // namespace

static void timers2_test4_thread(timers_test4_ctxt* ctx)
{
  std::random_device                    rd;
  std::mt19937                          mt19937(rd());
  std::uniform_real_distribution<float> real_dist(0.0f, 1.0f);
  for (timer_tick_difference_t d = 0; d != ctx->duration; ++d) {
    // Make random events.
    for (size_t i = 1; i != ctx->timers.size(); ++i) {
      // Ensure the getters always return reasonable values.
      TESTASSERT(ctx->timers[i].time_elapsed() <= ctx->duration);

      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].run();
      }
      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].stop();
      }
      if (0.1f > real_dist(mt19937)) {
        ctx->timers[i].set(static_cast<uint32_t>(ctx->duration * real_dist(mt19937)));
        ctx->timers[i].run();
      }
    }

    // Send finished to main thread and wait for it to check results
    std::unique_lock<std::mutex> lock(ctx->mutex);
    ctx->finished = true;
    ctx->cvar1.notify_one();
    ctx->cvar2.wait(lock, [&]() { return !ctx->finished; });
  }
}

static void timers_test4()
{
  test_delimit_logger logger{"Test unique_test multithreading safety"};

  timer_manager                         timers;
  timers_test4_ctxt                     ctx;
  unsigned                              nof_timers = 32;
  std::mt19937                          mt19937(4);
  std::uniform_real_distribution<float> real_dist(0.0f, 1.0f);

  // Generate all timers and start them.
  for (unsigned i = 0; i != nof_timers; ++i) {
    ctx.timers.push_back(timers.create_unique_timer());
    ctx.timers[i].set(ctx.duration);
    ctx.timers[i].run();
  }

  /* ========== multithreaded region begin =========== */

  // Create side thread.
  std::thread thread(timers2_test4_thread, &ctx);

  for (timer_tick_difference_t d = 0; d != ctx.duration; ++d) {
    // Make random events.
    for (unsigned i = 1; i != nof_timers; ++i) {
      // Ensure the getters always return reasonable values.
      TESTASSERT(ctx.timers[i].time_elapsed() <= ctx.duration);

      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].run(); // restart run
      }
      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].stop(); // stop run
      }
      if (0.1f > real_dist(mt19937)) {
        ctx.timers[i].set(static_cast<uint32_t>(ctx.duration * real_dist(mt19937)));
        ctx.timers[i].run(); // start run with new duration
      }
    }

    // First timer does not get updated, so it shall keep running.
    TESTASSERT(ctx.timers[0].is_running());

    // Increment time.
    timers.tick_all();

    // Wait second thread to finish events.
    std::unique_lock<std::mutex> lock(ctx.mutex);
    ctx.cvar1.wait(lock, [&ctx]() { return ctx.finished; });

    // Assert no timer got wrong values.
    for (unsigned i = 0; i != nof_timers; ++i) {
      if (ctx.timers[i].is_running()) {
        TESTASSERT(ctx.timers[i].time_elapsed() <= ctx.timers[i].duration());
        TESTASSERT(ctx.timers[i].duration() <= ctx.duration);
      }
    }

    // Start new TTI.
    ctx.finished = false;
    ctx.cvar2.notify_one();
  }

  // Finish asynchronous thread.
  thread.join();

  /* ========== multithreaded region end =========== */

  // First timer should have expired.
  TESTASSERT(ctx.timers[0].has_expired());
  TESTASSERT(!ctx.timers[0].is_running());

  // Run for the maximum period.
  for (timer_tick_difference_t d = 0; d != ctx.duration; ++d) {
    timers.tick_all();
  }

  // No timer should be running.
  for (unsigned i = 0; i != nof_timers; ++i) {
    TESTASSERT(!ctx.timers[i].is_running());
  }
}

/// Description: Delaying a callback using the timer_handler.
static void timers_test5()
{
  timer_manager timers;
  TESTASSERT(timers.nof_timers() == 0);
  TESTASSERT(timers.nof_running_timers() == 0);

  std::vector<int> vals;

  // TTI 0: Add a unique_timer of duration=5
  unique_timer t = timers.create_unique_timer();
  TESTASSERT(timers.nof_timers() == 1);
  t.set(5, [&vals](timer_id_t tid) { vals.push_back(1); });
  t.run();
  TESTASSERT(timers.nof_running_timers() == 1);
  timers.tick_all();

  // TTI 1: Add two delayed callbacks, with duration=2 and 6.
  {
    // Ensure captures by value are ok.
    std::string string = "test string";
    timers.defer_callback(2, [&vals, string]() {
      vals.push_back(2);
      srsran_assert(string == "test string", "string was not captured correctly");
    });
  }
  timers.defer_callback(6, [&vals]() { vals.push_back(3); });
  TESTASSERT(timers.nof_timers() == 3);
  TESTASSERT(timers.nof_running_timers() == 3);
  timers.tick_all();
  timers.tick_all();

  // TTI 3: First callback should have been triggered by now.
  TESTASSERT(timers.nof_running_timers() == 2);
  TESTASSERT(timers.nof_timers() == 2);
  TESTASSERT(vals.size() == 1);
  TESTASSERT(vals[0] == 2);
  timers.tick_all();
  timers.tick_all();

  // TTI 5: Unique timer should have been triggered by now.
  TESTASSERT(timers.nof_running_timers() == 1);
  TESTASSERT(timers.nof_timers() == 2);
  TESTASSERT(vals.size() == 2);
  TESTASSERT(vals[1] == 1);
  timers.tick_all();
  timers.tick_all();

  // TTI 7: Second callback should have been triggered by now.
  TESTASSERT(timers.nof_running_timers() == 0);
  TESTASSERT(timers.nof_timers() == 1);
  TESTASSERT(vals.size() == 3);
  TESTASSERT(vals[2] == 3);
}

/// Description: Check if erasure of a running timer is safe.
static void timers_test6()
{
  timer_manager timers;

  std::vector<int> vals;

  // Event: Add a timer that gets erased 1 tti after, and before expiring.
  {
    unique_timer t = timers.create_unique_timer();
    t.set(2, [&vals](timer_id_t tid) { vals.push_back(1); });
    t.run();
    TESTASSERT(timers.nof_running_timers() == 1 && t.duration() == 2 && t.is_running());
    timers.tick_all();
  }
  TESTASSERT(timers.nof_running_timers() == 0);
  TESTASSERT(timers.nof_timers() == 0);

  // TEST: The timer callback should not have been called.
  timers.tick_all();
  TESTASSERT(vals.empty());

  // Event: Add a timer that gets erased right after, and add another timer with same timeout.
  {
    unique_timer t = timers.create_unique_timer();
    t.set(2, [&vals](timer_id_t tid) { vals.push_back(2); });
    t.run();
    TESTASSERT(timers.nof_running_timers() == 1 && t.is_running());
    timers.tick_all();
    TESTASSERT(t.time_elapsed() == 1);
  }
  unique_timer t = timers.create_unique_timer();
  t.set(1, [&vals](timer_id_t tid) { vals.push_back(3); });
  t.run();
  TESTASSERT(timers.nof_running_timers() == 1);

  // TEST: The second timer's callback should be the one being called, and should be called only once.
  timers.tick_all();
  TESTASSERT(vals.size() == 1 && vals[0] == 3);
}

/// Tests specific to timer_handler wheel-based implementation:
/// - check if timer update is safe when its new updated wheel position matches the previous wheel position.
/// - multiple timers can exist in the same wheel position.
static void timers_test7()
{
  timer_manager timers;
  size_t        wheel_size = timer_manager::get_wheel_size();

  unique_timer t = timers.create_unique_timer();
  t.set(2);
  t.run();

  timers.tick_all();
  TESTASSERT(!t.has_expired() && t.is_running());

  // Should fall in same wheel position as previous timer run.
  t.set(1 + wheel_size);
  for (size_t i = 0; i != wheel_size; ++i) {
    timers.tick_all();
    TESTASSERT(!t.has_expired() && t.is_running());
  }
  timers.tick_all();
  TESTASSERT(t.has_expired() && !t.is_running());

  // The three timers will all fall in the same wheel position. However, only t and t3 should trigger.
  unique_timer t2 = timers.create_unique_timer();
  unique_timer t3 = timers.create_unique_timer();
  t.set(5);
  t2.set(5 + wheel_size);
  t3.set(5);
  t.run();
  t2.run();
  t3.run();
  TESTASSERT(timers.nof_running_timers() == 3 && timers.nof_timers() == 3);
  for (size_t i = 0; i != 5; ++i) {
    TESTASSERT(!t.has_expired() && t.is_running());
    TESTASSERT(!t2.has_expired() && t2.is_running());
    TESTASSERT(!t3.has_expired() && t3.is_running());
    timers.tick_all();
  }
  TESTASSERT(t.has_expired() && !t.is_running());
  TESTASSERT(!t2.has_expired() && t2.is_running());
  TESTASSERT(t3.has_expired() && !t3.is_running());
  TESTASSERT(timers.nof_running_timers() == 1 && timers.nof_timers() == 3);
}

int main()
{
  timers_test1();
  timers_test2();
  timers_test3();
  timers_test4();
  timers_test5();
  timers_test6();
  timers_test7();
  printf("Success\n");
  return 0;
}
