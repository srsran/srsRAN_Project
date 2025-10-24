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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace srsran {
namespace detail {

/// Queue barrier implementation based on sleep.
struct queue_sleep_barrier {
public:
  queue_sleep_barrier(std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) : sleep_time(sleep_time_) {}

  void request_stop() { running = false; }

  void notify_push()
  {
    // Do nothing.
  }

  void notify_pop()
  {
    // Do nothing.
  }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    while (running.load(std::memory_order_relaxed) and not has_space()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    while (running.load(std::memory_order_relaxed) and not has_elem()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>         running{true};
  std::chrono::microseconds sleep_time;
};

/// Barrier implementation based on condition variable.
struct queue_cond_var_barrier {
public:
  void request_stop()
  {
    running = false;
    notify_push();
    notify_pop();
  }

  void notify_push() { cvar_push.notify_one(); }

  void notify_pop() { cvar_pop.notify_one(); }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    cvar_pop.wait(lock, [this, &has_space]() { return not running.load(std::memory_order_relaxed) or has_space(); });
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    cvar_push.wait(lock, [this, &has_elem]() { return not running.load(std::memory_order_relaxed) or has_elem(); });
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>       running{true};
  std::condition_variable cvar_pop, cvar_push;
};

} // namespace detail
} // namespace srsran
