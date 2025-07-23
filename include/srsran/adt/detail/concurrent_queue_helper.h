/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/support/detail/has_method.h"
#include <atomic>
#include <optional>
#include <thread>

namespace srsran {

namespace detail {

/// Checker that the queue has a pop_blocking method.
CREATE_TRAIT_HAS_METHOD(has_pop_blocking, pop_blocking);

/// Consumer wrapper for queues that do not expose any consumer API.
template <typename ConcurrentQueue, typename T>
class basic_queue_consumer
{
public:
  basic_queue_consumer(ConcurrentQueue& q) : queue(&q) {}

  [[nodiscard]] bool try_pop(T& elem) { return queue->try_pop(elem); }

  template <typename U = T, std::enable_if_t<has_pop_blocking<ConcurrentQueue, bool(U&)>::value, int> = 0>
  [[nodiscard]] bool pop_blocking(T& elem)
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
