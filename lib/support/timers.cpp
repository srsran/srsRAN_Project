/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/timers.h"

using namespace srsran;

/* ****  Timer Wheel configuration parameters.  **** */
static constexpr size_t WHEEL_SHIFT = 16U;
static constexpr size_t WHEEL_SIZE  = 1U << WHEEL_SHIFT;
static constexpr size_t WHEEL_MASK  = WHEEL_SIZE - 1U;

/// Maximum timeout duration supported for a given timer in ticks.
static constexpr timer_duration MAX_TIMER_DURATION = timer_duration{std::numeric_limits<unsigned>::max() / 2};

timer_manager::timer_frontend::timer_frontend(timer_manager& parent_, timer_id_t id_) : parent(parent_), id(id_) {}

void timer_manager::timer_frontend::destroy()
{
  cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::stopped;
  parent.push_timer_command(cmd_t{id, cmd_id.load(std::memory_order_relaxed), cmd_t::destroy});
};

void timer_manager::timer_frontend::set(timer_duration dur)
{
  srsran_assert(dur <= MAX_TIMER_DURATION, "Invalid timer duration ({}>{})", dur.count(), MAX_TIMER_DURATION.count());
  cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed);
  duration = dur;
  if (state == state_t::running) {
    // If we are setting the timer when it is already running, force run restart.
    parent.push_timer_command(
        cmd_t{id, cmd_id.load(std::memory_order_relaxed), cmd_t::start, (unsigned)duration.count()});
  }
}

void timer_manager::timer_frontend::set(timer_duration dur, unique_function<void(timer_id_t)> callback_)
{
  set(dur);
  timeout_callback = std::move(callback_);
}

void timer_manager::timer_frontend::run()
{
  cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::running;
  parent.push_timer_command(
      cmd_t{id, cmd_id.load(std::memory_order_relaxed), cmd_t::start, (unsigned)duration.count()});
}

void timer_manager::timer_frontend::stop()
{
  cmd_id.fetch_add(1, std::memory_order::memory_order_relaxed);
  state = state_t::stopped;
  parent.push_timer_command(cmd_t{id, cmd_id.load(std::memory_order_relaxed), cmd_t::stop});
}

// /////////////////////

constexpr timer_duration timer_manager::INVALID_DURATION;

timer_manager::timer_manager(size_t capacity) : logger(srslog::fetch_basic_logger("ALL")), time_wheel(WHEEL_SIZE)
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

void timer_manager::tick()
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
    timer_handle& timer = timer_list[static_cast<unsigned>(cmd.id)];

    // Update the timer backend cmd_id to match frontend.
    timer.backend.cmd_id = cmd.cmd_id;

    // Stop timer if it is currently running.
    try_stop_timer_backend(timer, false);

    if (cmd.action == cmd_t::start) {
      // Start new timer run.
      start_timer_backend(timer, cmd.duration);
    } else if (cmd.action == cmd_t::destroy) {
      destroy_timer_backend(timer);
    }
  }

  // Re-trigger timeout for timers that failed to be triggered in the previous slot.
  handle_postponed_timeouts();

  // Advance time.
  cur_time++;

  // Process the timer runs which expire in this tick.
  auto& wheel_list = time_wheel[cur_time & WHEEL_MASK];

  // > iterate intrusive linked list of running timers with same wheel index
  for (auto it = wheel_list.begin(); it != wheel_list.end();) {
    srsran_assert(it->frontend != nullptr, "invalid state of timer in timer wheel");
    timer_handle& timer = timer_list[static_cast<unsigned>(it->frontend->id)];
    ++it; // we move iterator already, in case, the current timer gets removed from the linked list.

    // If the timer doesnt expire yet, continue the iteration in the same wheel bucket.
    if (cur_time != timer.backend.timeout) {
      continue;
    }

    // Stop timer. Note: callback has to see the timer has already expired.
    stop_timer_backend(timer, true);
  }
}

void timer_manager::push_timer_command(cmd_t cmd)
{
  std::lock_guard<std::mutex> lock(cmd_mutex);
  pending_cmds.emplace_back(cmd);
}

void timer_manager::create_timer_handle(std::unique_ptr<timer_frontend> timer)
{
  unsigned timer_idx = static_cast<unsigned>(timer->id);
  srsran_assert(timer_idx >= timer_list.size() or timer_list[timer_idx].frontend == nullptr,
                "Duplicate timer id detection");
  if (timer_idx >= timer_list.size()) {
    timer_list.resize(timer_idx + 1);
  }
  timer_list[timer_idx].frontend = std::move(timer);
}

void timer_manager::start_timer_backend(timer_handle& timer, unsigned duration)
{
  srsran_assert(timer.backend.state != state_t::running, "Invalid timer state");
  srsran_assert(timer.frontend != nullptr, "Invalid timer state");

  timer.backend.timeout = cur_time + std::max(duration, 1U);
  timer.backend.state   = state_t::running;
  time_wheel[timer.backend.timeout & WHEEL_MASK].push_front(&timer);
  nof_timers_running++;
}

bool timer_manager::trigger_timeout_handling(timer_handle& timer)
{
  return timer.frontend->exec->defer([frontend = timer.frontend.get(), expiry_epoch = timer.backend.cmd_id]() {
    // In case, the timer state has not been updated since the task was dispatched (epoches match).
    // Note: Now that we are in the same execution context as the timer frontend, the frontend cmd_id is precise.
    if (frontend->cmd_id.load(std::memory_order_relaxed) == expiry_epoch) {
      srsran_assert(frontend->state == state_t::running, "The timer can only expire if it was already running");
      // Update timer frontend state to expired.
      frontend->state = state_t::expired;

      // Run callback if configured.
      if (not frontend->timeout_callback.is_empty()) {
        frontend->timeout_callback(frontend->id);
      }
    }
  });
}

bool timer_manager::try_stop_timer_backend(timer_handle& timer, bool expiry_reason)
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
  cmd_id_t current_cmd_id_estim = timer.frontend->cmd_id.load(std::memory_order_relaxed);
  if (timer.backend.cmd_id == current_cmd_id_estim) {
    bool success = trigger_timeout_handling(timer);

    // When it was not possible to dispatch timeout callback to executor, postpone the dispatch.
    if (not success) {
      logger.warning("Failed to dispatch timeout handling for timer={}. Re-scheduling the handling to the next slot",
                     timer.frontend->id);
      failed_to_trigger_timers.push_back(std::make_pair(timer.frontend->id, timer.backend.cmd_id));
    }
  }
  return true;
}

void timer_manager::handle_postponed_timeouts()
{
  while (not failed_to_trigger_timers.empty()) {
    timer_handle& timer       = timer_list[static_cast<unsigned>(failed_to_trigger_timers.front().first)];
    cmd_id_t      prev_cmd_id = failed_to_trigger_timers.front().second;

    if (timer.backend.cmd_id == prev_cmd_id and
        timer.backend.cmd_id == timer.frontend->cmd_id.load(std::memory_order_relaxed) and
        not trigger_timeout_handling(timer)) {
      // Timeout handling dispatch failed again. No point in continuing loop.
      break;
    }

    // Either the cmd_id was updated or the timeout dispatch was successful. Remove element from the list.
    failed_to_trigger_timers.pop_front();
  }
}

void timer_manager::stop_timer_backend(timer_handle& timer, bool expiry_reason)
{
  srsran_assert(timer.backend.state == state_t::running, "Stopping timer that was not running");
  try_stop_timer_backend(timer, expiry_reason);
}

void timer_manager::destroy_timer_backend(timer_handle& timer)
{
  srsran_assert(timer.backend.state != state_t::running, "Destroying timer that is running not allowed");
  // clear frontend (it is already released by unique_timer).
  timer.frontend->state            = state_t::stopped;
  timer.frontend->duration         = timer_manager::INVALID_DURATION;
  timer.frontend->timeout_callback = {};
  timer.frontend->exec             = nullptr;
  // clear backend.
  timer.backend.state   = state_t::stopped;
  timer.backend.timeout = 0;
  // Add timer handle in free list.
  std::lock_guard<std::mutex> lock(free_list_mutex);
  free_list.emplace_back(timer.frontend.get());
}

timer_manager::timer_frontend& timer_manager::create_frontend_timer(task_executor& exec)
{
  // Allocate timer frontend with unique timer id.
  timer_id_t      id           = timer_id_t::invalid;
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

unique_timer timer_manager::create_unique_timer(task_executor& exec)
{
  return unique_timer(create_frontend_timer(exec));
}

size_t timer_manager::nof_timers() const
{
  std::lock_guard<std::mutex> lock(free_list_mutex);
  return timer_list.size() - free_list.size();
}

/// Returns the number of running timers handled by this instance.
unsigned timer_manager::nof_running_timers() const
{
  return nof_timers_running;
}
