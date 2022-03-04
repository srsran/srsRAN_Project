#include "srsgnb/support/timers.h"

using namespace srsgnb;

timer_handler::timer_handler(size_t capacity) : time_wheel(WHEEL_SIZE)
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

void timer_handler::tick_all()
{
  std::unique_lock<std::mutex> lock(mutex);

  timer_tick_t cur_time_local = cur_time.load(std::memory_order_relaxed) + 1;
  auto&        wheel_list     = time_wheel[cur_time_local & WHEEL_MASK];

  for (auto i = wheel_list.begin(); i != wheel_list.end();) {
    timer_impl& timer = timer_list[i->id];
    ++i;

    // Check if the timer expires right now.
    if (decode_timeout(timer.state.load(std::memory_order_relaxed)) != cur_time_local) {
      continue;
    }

    // Stop timer (callback has to see the timer has already expired).
    stop_timer(timer, true);

    // Call callback if configured.
    if (!timer.callback.is_empty()) {
      // Unlock mutex. It can happen that the callback tries to run a timer too.
      lock.unlock();

      timer.callback(timer.id);

      // Lock again to keep protecting the wheel.
      lock.lock();
    }
  }

  cur_time.fetch_add(1, std::memory_order_relaxed);
}

void timer_handler::stop_all()
{
  std::lock_guard<std::mutex> lock(mutex);

  // Does not call callback.
  for (timer_impl& timer : timer_list) {
    stop_timer(timer, false);
  }
}

timer_handler::timer_impl& timer_handler::alloc_timer()
{
  std::lock_guard<std::mutex> lock(mutex);

  timer_impl* t;
  if (!free_list.empty()) {
    t = &free_list.front();
    srsran_assert(!t->allocated, "Invalid timer id=%" PRIu32 " state", t->id);
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

void timer_handler::dealloc_timer(timer_impl& timer)
{
  if (!timer.allocated) {
    // Already deallocated.
    return;
  }

  stop_timer(timer, false);
  timer.allocated = false;
  timer.state.store(encode_state(STOPPED_FLAG, 0, 0), std::memory_order_relaxed);
  timer.callback = {};
  free_list.push_front(&timer);
  ++nof_free_timers;
  // Leave id unchanged.
}

void timer_handler::start_run(timer_impl& timer, timer_tick_difference_t duration)
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

void timer_handler::stop_timer(timer_impl& timer, bool expiry)
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

unique_timer timer_handler::create_unique_timer()
{
  return unique_timer(&alloc_timer());
}
