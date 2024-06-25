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

#pragma once

#include "rigtorp/MPMCQueue.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/error_handling.h"
#include <chrono>
#include <thread>

namespace srsran {
namespace detail {

/// Specialization for lockfree MPMC without a blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  template <typename... Args>
  explicit queue_impl(size_t qsize) : queue(qsize)
  {
  }

  bool try_push(const T& elem) { return queue.try_push(elem); }

  bool try_push(T&& elem) { return queue.try_push(std::move(elem)); }

  bool try_pop(T& elem) { return queue.try_pop(elem); }

  size_t size() const
  {
    // Note: MPMCqueue size can be negative.
    ptrdiff_t ret = queue.size();
    return static_cast<size_t>(std::max(ret, static_cast<ptrdiff_t>(0)));
  }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.capacity(); }

protected:
  ::rigtorp::MPMCQueue<T> queue;
};

/// Specialization for lockfree MPMC using a sleep as the blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>
  : public queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  using base_type = queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    base_type(qsize), sleep_time(sleep_time_)
  {
  }

  void request_stop() { running = false; }

  template <typename U>
  bool push_blocking(U&& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (this->try_push(std::forward<U>(elem))) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  bool pop_blocking(T& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (this->try_pop(elem)) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    T elem;
    if (pop_blocking(elem)) {
      func(elem);
      return true;
    }
    return false;
  }

private:
  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

} // namespace detail
} // namespace srsran
