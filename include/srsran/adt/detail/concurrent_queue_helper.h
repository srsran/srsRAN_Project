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

#include "srsran/adt/span.h"
#include "srsran/support/detail/has_method.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>
#include <optional>
#include <thread>

namespace srsran {

namespace detail {

namespace queue_helper {

/// Generic implementation of try_pop_bulk for queues that do not have a specialized implementation.
template <typename Queue>
size_t try_pop_bulk_generic(Queue& queue, span<typename Queue::value_type> batch)
{
  size_t total_popped = 0;
  while (total_popped < batch.size()) {
    if (queue.try_pop(batch[total_popped])) {
      ++total_popped;
    } else {
      break;
    }
  }
  return total_popped;
}

/// Generic implementation of try_push_bulk for queues that do not have a specialized implementation.
template <typename Queue, typename T>
size_t try_push_bulk_generic(Queue& queue, span<T> batch)
{
  const size_t sz = batch.size();
  for (size_t i = 0; i != sz; ++i) {
    bool pushed = false;
    if constexpr (std::is_const_v<T>) {
      pushed = queue.try_push(batch[i]);
    } else {
      pushed = queue.try_push(std::move(batch[i]));
    }
    if (not pushed) {
      return i;
    }
  }
  return sz;
}

/// Policy for queues that use a sleep to wait for new elements to be pushed or popped.
struct sleep_wait_policy {
  sleep_wait_policy(std::chrono::microseconds sleep_time_) : sleep_time(sleep_time_) {}

  void                      request_stop() { running.store(false, std::memory_order_relaxed); }
  std::chrono::microseconds get_sleep_time() const { return sleep_time; }
  bool                      is_running() const { return running.load(std::memory_order_relaxed); }
  void                      wait() const { std::this_thread::sleep_for(sleep_time); }

private:
  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

/// Helper function to pop an element from a queue in a blocking manner, using the provided sleep policy.
template <typename Queue, typename T>
bool pop_blocking_generic(Queue& q, T& elem, const sleep_wait_policy& policy)
{
  while (policy.is_running()) {
    if (q.try_pop(elem)) {
      return true;
    }
    policy.wait();
  }
  return false;
}

/// Helper function to pop an element from a queue in a blocking manner with a timeout, using the provided sleep policy.
template <typename Queue, typename T>
bool pop_blocking_generic(Queue& q, T& elem, const sleep_wait_policy& policy, std::chrono::microseconds wait_time)
{
  const unsigned nof_waits = divide_ceil(wait_time.count(), policy.get_sleep_time().count());
  for (unsigned i = 0; i != nof_waits and policy.is_running(); ++i) {
    if (q.try_pop(elem)) {
      return true;
    }
    policy.wait();
  }
  return false;
}

} // namespace queue_helper

/// Checker that the queue has a pop_blocking method.
CREATE_TRAIT_HAS_METHOD(has_pop_blocking, pop_blocking);

/// Consumer wrapper for queues that do not expose any consumer API.
template <typename ConcurrentQueue, typename T>
class basic_queue_consumer
{
public:
  using value_type = T;

  basic_queue_consumer(ConcurrentQueue& q) : queue(&q) {}

  [[nodiscard]] bool try_pop(T& elem) { return queue->try_pop(elem); }

  [[nodiscard]] size_t try_pop_bulk(span<T> batch) { return queue->try_pop_bulk(batch); }

  template <typename U = T, std::enable_if_t<has_pop_blocking<ConcurrentQueue, bool(U&)>::value, int> = 0>
  [[nodiscard]] bool pop_blocking(U& elem)
  {
    return queue->pop_blocking(elem);
  }

private:
  ConcurrentQueue* queue;
};

/// Extension of queues with blocking api based on sleeps.
template <typename Derived>
class queue_sleep_crtp
{
public:
  explicit queue_sleep_crtp(std::chrono::microseconds sleep_time_) : policy(sleep_time_) {}

  void request_stop() { policy.request_stop(); }

  template <typename U>
  bool push_blocking(U&& elem)
  {
    while (policy.is_running()) {
      if (derived()->try_push(std::forward<U>(elem))) {
        return true;
      }
      policy.wait();
    }
    return false;
  }

  template <typename U>
  [[nodiscard]] bool pop_blocking(U& elem)
  {
    return queue_helper::pop_blocking_generic(*derived(), elem, policy);
  }

  template <typename U>
  [[nodiscard]] bool pop_blocking(U& elem, std::chrono::microseconds wait_time)
  {
    return queue_helper::pop_blocking_generic(*derived(), elem, policy, wait_time);
  }

  template <typename PoppingFunc>
  [[nodiscard]] bool call_on_pop_blocking(PoppingFunc&& func)
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

  queue_helper::sleep_wait_policy policy;
};

} // namespace detail
} // namespace srsran
