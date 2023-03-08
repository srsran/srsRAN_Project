/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/timers2.h"

using namespace srsran;

/// Wheel configuration parameters.
static constexpr size_t WHEEL_SHIFT = 16U;
static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

static constexpr timer_tick_difference_t MAX_TIMER_DURATION = std::numeric_limits<timer_tick_difference_t>::max() / 2;

timer_manager2::timer_handle::timer_handle(timer_manager2& parent_, timer_id_t id_) : id(id_), parent(parent_) {}

bool timer_manager2::timer_handle::empty() const
{
  return backend.exec == nullptr;
}

void timer_manager2::timer_handle::destroy()
{
  frontend.epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  frontend.state = stopped;
  parent.push_timer_command(cmd_t{id, frontend.epoch.load(std::memory_order_relaxed), cmd_t::destroy});
};

void timer_manager2::timer_handle::set(unsigned duration)
{
  srsran_assert(duration <= MAX_TIMER_DURATION,
                "Invalid timer duration={}>{}",
                duration,
                (timer_tick_difference_t)MAX_TIMER_DURATION);
  frontend.epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  frontend.duration = duration;
  if (frontend.state == running) {
    // If we are setting the timer when it is already running.
    parent.push_timer_command(
        cmd_t{id, frontend.epoch.load(std::memory_order_relaxed), cmd_t::start, frontend.duration});
  }
}

void timer_manager2::timer_handle::set(unsigned duration, unique_function<void(timer_id_t)> callback)
{
  srsran_assert(duration <= MAX_TIMER_DURATION,
                "Invalid timer duration={}>{}",
                duration,
                (timer_tick_difference_t)MAX_TIMER_DURATION);
  frontend.epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  frontend.duration = duration;
  frontend.callback = std::move(callback);
  parent.push_timer_command(cmd_t{id,
                                  frontend.epoch.load(std::memory_order_relaxed),
                                  frontend.state == running ? cmd_t::start : cmd_t::stop,
                                  frontend.duration});
}

void timer_manager2::timer_handle::run()
{
  frontend.epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  frontend.state = running;
  parent.push_timer_command(cmd_t{id, frontend.epoch.load(std::memory_order_relaxed), cmd_t::start, frontend.duration});
}

void timer_manager2::timer_handle::stop()
{
  frontend.epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  frontend.state = stopped;
  parent.push_timer_command(cmd_t{id, frontend.epoch.load(std::memory_order_relaxed), cmd_t::stop});
}

// /////////////////////

timer_manager2::timer_manager2(size_t capacity) : time_wheel(WHEEL_SIZE)
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

void timer_manager2::tick_all()
{
  // Extract new commands from the timer front-ends to process in this tick.
  {
    cmds_to_process.clear();
    std::lock_guard<std::mutex> lock(cmd_mutex);
    pending_cmds.swap(cmds_to_process);
  }

  // Process new commands coming from the front-end.
  for (const cmd_t& cmd : cmds_to_process) {
    timer_handle& timer = timer_list[cmd.id];

    // Update the timer backend epoch to match frontend.
    timer.backend.epoch = cmd.epoch;

    // Stop timer if it is currently running.
    try_stop_timer_impl(timer, false);

    if (cmd.action == cmd_t::start) {
      // Start new timer run.
      start_timer_impl(timer, cmd.duration);
    } else if (cmd.action == cmd_t::destroy) {
      destroy_timer_impl(timer);
    }
  }

  // Advance time.
  cur_time++;

  // Process the timer runs which expire in this tick.
  auto& wheel_list = time_wheel[cur_time & WHEEL_MASK];

  // > iterate intrusive linked list of running timers with same wheel index
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    timer_handle& timer = timer_list[it->id];
    ++it; // we move iterator already, in case, the current timer gets removed from the linked list.

    // If the timer doesnt expire yet, continue the iteration in the same wheel bucket.
    if (cur_time != timer.backend.timeout) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    stop_timer_impl(timer, true);
  }
}

void timer_manager2::push_timer_command(cmd_t cmd)
{
  std::lock_guard<std::mutex> lock(cmd_mutex);
  pending_cmds.push_back(cmd);
}

void timer_manager2::start_timer_impl(timer_handle& timer, unsigned duration)
{
  srsran_assert(timer.backend.state != running, "Invalid timer state");

  timer.backend.timeout = cur_time + std::max(duration, 1U);
  timer.backend.state   = running;
  time_wheel[timer.backend.timeout & WHEEL_MASK].push_front(&timer);
  nof_timers_running++;
}

bool timer_manager2::try_stop_timer_impl(timer_handle& timer, bool expiry_reason)
{
  if (timer.backend.state != running) {
    return false;
  }

  // Pop timer from the timer wheel.
  time_wheel[timer.backend.timeout & WHEEL_MASK].pop(&timer);
  --nof_timers_running;

  // Update timer state in the backend.
  timer.backend.state = expiry_reason ? expired : stopped;

  if (not expiry_reason) {
    // In case of stop command, no need to do any more actions. (stop is only set by timer frontend).
    return true;
  }

  // In case of expiry, and the backend and frontend (estimate) epoches match, dispatch the callback to frontend
  // executor.
  epoch_type frontend_epoch_estim = timer.frontend.epoch.load(std::memory_order_relaxed);
  if (timer.backend.epoch == frontend_epoch_estim) {
    timer.backend.exec->execute([this, tid = timer.id, expiry_epoch = timer.backend.epoch]() {
      auto& timer_frontend = timer_list[tid].frontend;

      // In case, the timer state has not been updated since the task was dispatched (epoches match).
      // Note: Now that we are in the same execution context as the timer frontend, the frontend epoch is precise.
      if (timer_frontend.epoch == expiry_epoch) {
        // Update timer frontend state to expired.
        timer_frontend.state = expired;

        // Run callback if configured.
        if (not timer_frontend.callback.is_empty()) {
          timer_frontend.callback(tid);
        }
      }
    });
  }
  return true;
}

void timer_manager2::stop_timer_impl(timer_handle& timer, bool expiry_reason)
{
  srsran_assert(timer.backend.state == running, "Stopping timer that was not running");
  try_stop_timer_impl(timer, expiry_reason);
}

void timer_manager2::destroy_timer_impl(timer_handle& timer)
{
  srsran_assert(timer.backend.state != running, "Destroying timer that is running not allowed");
  // clear frontend.
  timer.frontend.epoch.store(0, std::memory_order_relaxed);
  timer.frontend.state    = stopped;
  timer.frontend.duration = timer_manager2::INVALID_DURATION;
  timer.frontend.callback = {};
  // clear backend.
  timer.backend.epoch   = 0;
  timer.backend.state   = stopped;
  timer.backend.timeout = 0;
  timer.backend.exec    = nullptr;
  // Add timer handle in free list.
  free_list.push_front(&timer);
  ++nof_free_timers;
}

timer_manager2::timer_handle& timer_manager2::create_timer(task_executor& exec)
{
  timer_handle* t;
  if (!free_list.empty()) {
    t = &free_list.front();
    srsran_assert(t->empty(), "Invalid timer state. id={}", t->id);
    free_list.pop_front();
    --nof_free_timers;
  } else {
    // Need to increase deque.
    timer_list.emplace_back(*this, timer_list.size());
    t = &timer_list.back();
  }
  t->backend.exec = &exec;
  return *t;
}

unique_timer timer_manager2::create_unique_timer(task_executor& exec)
{
  return unique_timer(create_timer(exec));
}
