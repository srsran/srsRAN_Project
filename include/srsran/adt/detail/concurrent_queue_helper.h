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
#include <optional>
#include <thread>

namespace srsran {

namespace detail {

/// Extension of queues with blocking api based on sleeps.
template <typename Derived>
class queue_sleep_crtp
{
public:
  explicit queue_sleep_crtp(std::chrono::microseconds sleep_time_) : sleep_time(sleep_time_) {}

  void request_stop() { running.store(false, std::memory_order_relaxed); }

  template <typename U>
  bool push_blocking(U&& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (derived()->try_push(std::forward<U>(elem))) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  template <typename U>
  bool pop_blocking(U& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (derived()->try_pop(elem)) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    typename Derived::value_type elem;
    if (pop_blocking(elem)) {
      func(elem);
      return true;
    }
    return false;
  }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  auto pop_blocking() noexcept
  {
    std::optional<typename Derived::value_type> ret;
    if (not pop_blocking(ret.emplace())) {
      ret.reset();
    }
    return ret;
  }

protected:
  Derived* derived() { return static_cast<Derived*>(this); }

  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

} // namespace detail
} // namespace srsran
