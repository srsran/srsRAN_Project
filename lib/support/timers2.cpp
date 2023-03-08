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

/* ****  Timer Wheel configuration parameters.  **** */
static constexpr size_t WHEEL_SHIFT = 16U;
static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

/// Maximum timeout duration supported for a given timer in ticks.
static constexpr timer_duration MAX_TIMER_DURATION = timer_duration{std::numeric_limits<unsigned>::max() / 2};

timer_manager2::timer_frontend::timer_frontend(timer_manager2& parent_, timer_id_t id_) : parent(parent_), id(id_) {}

void timer_manager2::timer_frontend::destroy()
{
  epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::stopped;
  parent.push_timer_command(cmd_t{id, epoch.load(std::memory_order_relaxed), cmd_t::destroy});
};

void timer_manager2::timer_frontend::set(timer_duration dur)
{
  srsran_assert(dur <= MAX_TIMER_DURATION, "Invalid timer duration ({}>{})", dur.count(), MAX_TIMER_DURATION.count());
  epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  duration = dur;
  if (state == state_t::running) {
    // If we are setting the timer when it is already running, force run restart.
    parent.push_timer_command(
        cmd_t{id, epoch.load(std::memory_order_relaxed), cmd_t::start, (unsigned)duration.count()});
  }
}

void timer_manager2::timer_frontend::set(timer_duration dur, unique_function<void(timer_id_t)> callback_)
{
  set(dur);
  callback = std::move(callback_);
}

void timer_manager2::timer_frontend::run()
{
  epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::running;
  parent.push_timer_command(cmd_t{id, epoch.load(std::memory_order_relaxed), cmd_t::start, (unsigned)duration.count()});
}

void timer_manager2::timer_frontend::stop()
{
  epoch.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::stopped;
  parent.push_timer_command(cmd_t{id, epoch.load(std::memory_order_relaxed), cmd_t::stop});
}

// /////////////////////

constexpr timer_duration timer_manager2::INVALID_DURATION;

timer_manager2::timer_manager2(size_t capacity) : time_wheel(WHEEL_SIZE)
{
  // Pre-reserve timers.
  while (timer_list.size() < capacity) {
    timer_list.emplace_back();
    timer_list.back().frontend = std::make_unique<timer_frontend>(*this, (timer_id_t)next_timer_id++);
  }

  // Push to free list in reverse order to keep ascending ids.
  for (auto i = timer_list.rbegin(), e = timer_list.rend(); i != e; ++i) {
    free_list.push_back(i->frontend.get());
  }
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
  for (variant<cmd_t, std::unique_ptr<timer_frontend>>& event : cmds_to_process) {
    if (variant_holds_alternative<std::unique_ptr<timer_frontend>>(event)) {
      // New timer was created in the frontend.
      create_timer_handle(std::move(variant_get<std::unique_ptr<timer_frontend>>(event)));
      continue;
    }

    // Existing timer.
    const cmd_t&  cmd   = variant_get<cmd_t>(event);
    timer_handle& timer = timer_list[cmd.id];

    // Update the timer backend epoch to match frontend.
    timer.backend.epoch = cmd.epoch;

    // Stop timer if it is currently running.
    try_stop_timer_backend(timer, false);

    if (cmd.action == cmd_t::start) {
      // Start new timer run.
      start_timer_backend(timer, cmd.duration);
    } else if (cmd.action == cmd_t::destroy) {
      destroy_timer_backend(timer);
    }
  }

  // Advance time.
  cur_time++;

  // Process the timer runs which expire in this tick.
  auto& wheel_list = time_wheel[cur_time & WHEEL_MASK];

  // > iterate intrusive linked list of running timers with same wheel index
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    srsran_assert(it->frontend != nullptr, "invalid state of timer in timer wheel");
    timer_handle& timer = timer_list[it->frontend->id];
    ++it; // we move iterator already, in case, the current timer gets removed from the linked list.

    // If the timer doesnt expire yet, continue the iteration in the same wheel bucket.
    if (cur_time != timer.backend.timeout) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    stop_timer_backend(timer, true);
  }
}

void timer_manager2::push_timer_command(cmd_t cmd)
{
  std::lock_guard<std::mutex> lock(cmd_mutex);
  pending_cmds.emplace_back(cmd);
}

void timer_manager2::create_timer_handle(std::unique_ptr<timer_frontend> timer)
{
  srsran_assert(timer->id >= timer_list.size() or timer_list[timer->id].frontend == nullptr,
                "Duplicate timer id detection");
  if (timer->id >= timer_list.size()) {
    timer_list.resize(timer->id + 1);
  }
  timer_list[timer->id].frontend = std::move(timer);
}

void timer_manager2::start_timer_backend(timer_handle& timer, unsigned duration)
{
  srsran_assert(timer.backend.state != state_t::running, "Invalid timer state");
  srsran_assert(timer.frontend != nullptr, "Invalid timer state");

  timer.backend.timeout = cur_time + std::max(duration, 1U);
  timer.backend.state   = state_t::running;
  time_wheel[timer.backend.timeout & WHEEL_MASK].push_front(&timer);
  nof_timers_running++;
}

bool timer_manager2::try_stop_timer_backend(timer_handle& timer, bool expiry_reason)
{
  if (timer.backend.state != state_t::running) {
    return false;
  }

  // Pop timer from the timer wheel.
  time_wheel[timer.backend.timeout & WHEEL_MASK].pop(&timer);
  --nof_timers_running;

  // Update timer state in the backend.
  timer.backend.state = expiry_reason ? state_t::expired : state_t::stopped;

  if (not expiry_reason) {
    // In case of stop command, no need to do any more actions. (stop is only set by timer frontend).
    return true;
  }

  // In case of expiry, and the backend and frontend (estimate) epoches match, dispatch the callback to frontend
  // executor.
  epoch_type frontend_epoch_estim = timer.frontend->epoch.load(std::memory_order_relaxed);
  if (timer.backend.epoch == frontend_epoch_estim) {
    timer.frontend->exec->defer([frontend = timer.frontend.get(), expiry_epoch = timer.backend.epoch]() {
      // In case, the timer state has not been updated since the task was dispatched (epoches match).
      // Note: Now that we are in the same execution context as the timer frontend, the frontend epoch is precise.
      if (frontend->epoch.load(std::memory_order_relaxed) == expiry_epoch) {
        // Update timer frontend state to expired.
        frontend->state = state_t::expired;

        // Run callback if configured.
        if (not frontend->callback.is_empty()) {
          frontend->callback(frontend->id);
        }
      }
    });
  }
  return true;
}

void timer_manager2::stop_timer_backend(timer_handle& timer, bool expiry_reason)
{
  srsran_assert(timer.backend.state == state_t::running, "Stopping timer that was not running");
  try_stop_timer_backend(timer, expiry_reason);
}

void timer_manager2::destroy_timer_backend(timer_handle& timer)
{
  srsran_assert(timer.backend.state != state_t::running, "Destroying timer that is running not allowed");
  // clear frontend (it is already released by unique_timer2).
  timer.frontend->state    = state_t::stopped;
  timer.frontend->duration = timer_manager2::INVALID_DURATION;
  timer.frontend->callback = {};
  timer.frontend->exec     = nullptr;
  // clear backend.
  timer.backend.state   = state_t::stopped;
  timer.backend.timeout = 0;
  // Add timer handle in free list.
  std::lock_guard<std::mutex> lock(free_list_mutex);
  free_list.emplace_back(timer.frontend.get());
}

timer_manager2::timer_frontend& timer_manager2::create_frontend_timer(task_executor& exec)
{
  // Allocate timer frontend with unique timer id.
  timer_id_t      id           = INVALID_TIMER_ID;
  timer_frontend* cached_timer = nullptr;
  {
    std::lock_guard<std::mutex> lock(free_list_mutex);
    if (!free_list.empty()) {
      cached_timer = free_list.back();
      free_list.pop_back();
    } else {
      // Need to allocate new timer.
      id = (timer_id_t)next_timer_id++;
    }
  }

  // In case it fails to reuse a cached timer frontend object. Need to create a new one.
  if (cached_timer == nullptr) {
    auto new_handle  = std::make_unique<timer_frontend>(*this, id);
    new_handle->exec = &exec;
    cached_timer     = new_handle.get();

    // Forward created timer handle to the backend.
    {
      std::lock_guard<std::mutex> lock(cmd_mutex);
      pending_cmds.emplace_back(std::move(new_handle));
    }
  } else {
    srsran_assert(cached_timer->exec == nullptr, "Reassignment of timer detected");
    // Assign new executor to created timer.
    cached_timer->exec = &exec;
  }

  return *cached_timer;
}

unique_timer2 timer_manager2::create_unique_timer(task_executor& exec)
{
  return unique_timer2(create_frontend_timer(exec));
}

size_t timer_manager2::nof_timers() const
{
  std::lock_guard<std::mutex> lock(free_list_mutex);
  return timer_list.size() - free_list.size();
}

/// Returns the number of running timers handled by this instance.
unsigned timer_manager2::nof_running_timers() const
{
  return nof_timers_running;
}
