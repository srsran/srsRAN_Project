/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/unique_function.h"
#include "srsran/support/executors/task_executor.h"
#include <chrono>
#include <memory>

namespace srsran {

/// Type used to represent a unique timer identifier.
enum class timer_id_t : unsigned { invalid = std::numeric_limits<unsigned>::max() };

/// Unit used to represent a time duration in terms of timer_manager ticks.
using timer_duration = std::chrono::milliseconds;

/// Type used to represent a particular time tick of the timers.
using tick_point_t = unsigned;

namespace timer_detail {

/// Possible states for a timer.
enum class state_t { stopped, running, expired };

/// Constant used to represent invalid timer durations.
constexpr timer_duration INVALID_DURATION = timer_duration::max();

/// Data relative to a timer state that is accessed from the front-end side (the unique_timer) getters.
struct frontend_state {
  /// Timer identifier. This identifier should remain constant throughout the timer lifetime.
  const timer_id_t id;
  /// The current state of timer (e.g. running/expired/notify_stop) from the perspective of the timer front-end.
  state_t state = state_t::stopped;
  /// Duration of each timer run.
  timer_duration duration = INVALID_DURATION;

  explicit frontend_state(timer_id_t id_) : id(id_) {}
};

} // namespace timer_detail

class unique_timer;

/// \brief This class implements the service associated with the management of the timers of the application, including
/// the allocation, destruction, starting/halting and timeout triggering of the registered timers. The user
/// can check and control the state of an individual timer via the respective \c unique_timer handle object.
///
/// The \c timer_manager and \c unique_timer classes follow a publisher/subscriber model. The \c timer_manager,
/// the publisher, receives periodic ticks from a clock source (e.g. RF front-end or system clock) that advance its
/// internal clock. It also contains an internal list of subscribed \c unique_timers. Once enough time has passed and
/// a running timer time-outs, the respective timer gets notified via a callback. The user can control the timer
/// starting time, duration until timeout and timeout callback function definition via the \c unique_timer interface.
///
/// The implementation assumes that the "tick()" method is always called from the same thread and that a unique_timer
/// object is not accessed concurrently by different threads. However, separate unique_timers can be safely used from
/// separate threads. A unique_timer object and the timer_manager communicate with one another via thread-safe queues.
class timer_manager
{
public:
  /// Constructs a timer manager.
  /// \param pre_reserve_capacity Number of timers to pre-reserve and speed up timer construction.
  explicit timer_manager(size_t pre_reserve_capacity = 64);

  ~timer_manager();

  /// Advances one tick and triggers any running timer that just expired.
  void tick();

  /// Creates a new instance of a unique timer.
  unique_timer create_unique_timer(task_executor& exec);

  /// Returns the number of timers handled by this instance.
  size_t nof_timers() const;

  /// Returns the number of running timers handled by this instance.
  size_t nof_running_timers() const;

  /// Gets the current tick count.
  tick_point_t now() const;

private:
  class manager_impl;

  /// Impl class for timer manager.
  std::unique_ptr<manager_impl> impl;
};

/// \brief This class represents a timer which invokes a user-provided callback upon timer expiration.
/// This class API is not thread-safe.
class unique_timer
{
  using state_t = timer_detail::state_t;

public:
  unique_timer()                               = default;
  unique_timer(const unique_timer&)            = delete;
  unique_timer& operator=(const unique_timer&) = delete;

  unique_timer(unique_timer&& other) noexcept : handle(other.handle) { other.handle = nullptr; }
  unique_timer& operator=(unique_timer&& other) noexcept;
  ~unique_timer();

  /// Destroy current unique_timer context, cancelling any pending event.
  void reset();

  /// Returns true if the timer is valid, otherwise returns false if already released.
  bool is_valid() const { return handle != nullptr; }

  /// Returns the unique timer identifier.
  timer_id_t id() const { return is_valid() ? handle->id : timer_id_t::invalid; }

  /// Returns true if the timer duration has been set, otherwise returns false.
  bool is_set() const { return is_valid() && handle->duration != timer_detail::INVALID_DURATION; }

  /// Returns true if the timer is currently running, otherwise returns false.
  bool is_running() const { return is_valid() && handle->state == state_t::running; }

  /// Returns true if the timer has expired, otherwise returns false.
  bool has_expired() const { return is_valid() && handle->state == state_t::expired; }

  /// Returns the configured duration for this timer measured in ticks.
  timer_duration duration() const { return is_valid() ? handle->duration : timer_detail::INVALID_DURATION; }

  /// Configures the duration of the timer calling the provided callback upon timer expiration.
  void set(timer_duration duration, unique_function<void(timer_id_t)> callback);

  /// Configures the duration of the timer.
  void set(timer_duration duration);

  /// Activates the timer to start ticking.
  void run();

  /// Stops the timer from ticking.
  void stop();

  /// Gets the current tick of the timer.
  tick_point_t now() const;

private:
  friend class timer_manager;

  explicit unique_timer(timer_detail::frontend_state& impl) : handle(&impl) {}

  timer_detail::frontend_state* handle = nullptr;
};

/// Factory of timers that associates created timers to specific task executors.
class timer_factory
{
public:
  timer_factory() = default;
  timer_factory(timer_manager& timer_mng_, task_executor& timer_exec_) :
    timer_mng(&timer_mng_), timer_exec(&timer_exec_)
  {
  }

  unique_timer create_timer() { return timer_mng->create_unique_timer(*timer_exec); }

private:
  timer_manager* timer_mng  = nullptr;
  task_executor* timer_exec = nullptr;
};

} // namespace srsran
