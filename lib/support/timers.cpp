/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/timers.h"

using namespace srsran;

timer_manager::timer_manager(size_t capacity) : time_wheel(WHEEL_SIZE)
{
  // Pre-reserve timers.
  while (timer_list.size() < capacity) {
    timer_list.emplace_back(*this, timer_list.size());
  }

  // Push to free list in reverse order to keep ascending ids.
  for (auto i = timer_list.rbegin(), e = timer_list.rend(); i != e; ++i) {
    free_list.push_front(&*i);
  }

  nof_free_timers = timer_list.size();
}

void timer_manager::tick_all()
{
  std::unique_lock<std::mutex> lock(mutex);

  timer_tick_t cur_time_local = cur_time.load(std::memory_order_relaxed) + 1;
  auto&        wheel_list     = time_wheel[cur_time_local & WHEEL_MASK];

  // iterate intrusive linked list of running timers with same wheel index
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    timer_handle& timer = timer_list[it->id];
    ++it;

    // Check if the timer expires right now.
    if (decode_timeout(timer.state.load(std::memory_order_relaxed)) != cur_time_local) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    stop_timer_nolock(timer, true);

    // Call callback if configured.
    if (!timer.callback.is_empty()) {
      // Unlock mutex. It can happen that the callback tries to run a timer too.
      lock.unlock();

      timer.callback(timer.id);

      // Lock again to keep protecting the wheel.
      lock.lock();
    }
  }

  // Advance current time. unique timers "time_elapsed" will change as a result.
  cur_time.fetch_add(1, std::memory_order_relaxed);
}

void timer_manager::stop_all()
{
  std::lock_guard<std::mutex> lock(mutex);

  // Does not call callback.
  for (timer_handle& timer : timer_list) {
    stop_timer_nolock(timer, false);
  }
}

timer_manager::timer_handle& timer_manager::create_timer()
{
  std::lock_guard<std::mutex> lock(mutex);

  timer_handle* t;
  if (!free_list.empty()) {
    t = &free_list.front();
    srsran_assert(!t->allocated, "Invalid timer state. id={}", t->id);
    free_list.pop_front();
    --nof_free_timers;
  } else {
    // Need to increase deque.
    timer_list.emplace_back(*this, timer_list.size());
    t = &timer_list.back();
  }
  t->allocated = true;
  return *t;
}

void timer_manager::destroy_timer(timer_handle& timer)
{
  std::lock_guard<std::mutex> lock(mutex);
  destroy_timer_nolock(timer);
}

void timer_manager::destroy_timer_nolock(timer_handle& timer)
{
  if (!timer.allocated) {
    // Already deallocated.
    return;
  }

  stop_timer_nolock(timer, false);
  timer.allocated = false;
  timer.state.store(encode_state(STOPPED_FLAG, 0, 0), std::memory_order_relaxed);
  timer.callback = {};
  free_list.push_front(&timer);
  ++nof_free_timers;
  // Leave id unchanged.
}

void timer_manager::start_run(timer_handle& timer, timer_tick_difference_t duration)
{
  std::lock_guard<std::mutex> lock(mutex);
  start_run_nolock(timer, duration);
}

void timer_manager::start_run_nolock(timer_handle& timer, timer_tick_difference_t duration)
{
  uint64_t timer_old_state   = timer.state.load(std::memory_order_relaxed);
  duration                   = duration == 0 ? decode_duration(timer_old_state) : duration;
  timer_tick_t new_timeout   = cur_time.load(std::memory_order_relaxed) + duration;
  size_t       new_wheel_pos = new_timeout & WHEEL_MASK;

  timer_tick_t old_timeout = decode_timeout(timer_old_state);
  bool         was_running = decode_is_running_flag(timer_old_state);
  if (was_running && (old_timeout & WHEEL_MASK) == new_wheel_pos) {
    // If no change in timer wheel position. Just update absolute timeout.
    timer.state.store(encode_state(RUNNING_FLAG, duration, new_timeout), std::memory_order_relaxed);
    return;
  }

  // Stop timer if it was running, removing it from wheel in the process.
  if (was_running) {
    time_wheel[old_timeout & WHEEL_MASK].pop(&timer);
    --nof_timers_running;
  }

  // Insert timer in wheel.
  time_wheel[new_wheel_pos].push_front(&timer);
  timer.state.store(encode_state(RUNNING_FLAG, duration, new_timeout), std::memory_order_relaxed);
  ++nof_timers_running;
}

void timer_manager::stop_timer(timer_handle& timer, bool expiry)
{
  std::lock_guard<std::mutex> lock(mutex);
  stop_timer_nolock(timer, expiry);
}

void timer_manager::stop_timer_nolock(timer_handle& timer, bool expiry)
{
  uint64_t timer_old_state = timer.state.load(std::memory_order_relaxed);
  if (!decode_is_running_flag(timer_old_state)) {
    return;
  }

  // If already running, need to disconnect it from previous wheel.
  timer_tick_t old_timeout = decode_timeout(timer_old_state);
  time_wheel[old_timeout & WHEEL_MASK].pop(&timer);
  uint64_t new_state =
      encode_state(expiry ? EXPIRED_FLAG : STOPPED_FLAG, decode_duration(timer_old_state), old_timeout);
  timer.state.store(new_state, std::memory_order_relaxed);
  --nof_timers_running;
}

unique_timer timer_manager::create_unique_timer()
{
  return unique_timer(&create_timer());
}
