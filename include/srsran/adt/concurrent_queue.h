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

#include "srsran/adt/detail/concurrent_queue_params.h"
#include <atomic>
#include <optional>
#include <thread>

namespace srsran {

namespace detail {

/// Implementation of the concurrent queue with the provided policies.
template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl;

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

private:
  Derived* derived() { return static_cast<Derived*>(this); }

  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

} // namespace detail

/// \brief A concurrent queue that can be used to pass objects between threads. Different policies are supported:
/// - lockfree_spsc: a lock-free single-producer single-consumer queue. This is the fastest option.
/// - lockfree_mpmc: a lock-free multi-producer multi-consumer queue.
/// - locking_mpmc: a multi-producer multi-consumer queue that uses a mutex to protect the queue. This is the most
/// most general queue, but has the highest overhead. The producers and consumers will contend on the access to the
/// mutex.
/// - locking_mpsc: a multi-producer single-consumer queue that uses a mutex to protect the queue. This queue pops
/// all elements in a batch to minimize the contention on the mutex from the consumer side.
template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class concurrent_queue
{
  using queue_type = detail::queue_impl<T, Policy, BlockingPolicy>;

public:
  using value_type                                       = T;
  static const concurrent_queue_policy      queue_policy = Policy;
  static const concurrent_queue_wait_policy wait_policy  = BlockingPolicy;

  template <typename... Args>
  explicit concurrent_queue(size_t minimum_q_size, Args&&... args) : queue(minimum_q_size, std::forward<Args>(args)...)
  {
  }

  /// Pushes a new element into the queue in a non-blocking fashion. If the queue is full, the element is not pushed.
  /// \return true if the element was pushed, false otherwise.
  template <typename U>
  bool try_push(U&& elem) noexcept
  {
    return queue.try_push(std::forward<U>(elem));
  }

  /// Pushes a new element into the queue. If the queue is full, the call blocks, waiting for a new slot to become
  /// emptied.
  /// \return true if the element was pushed, false if the queue was closed.
  template <typename U,
            concurrent_queue_wait_policy Q                                         = BlockingPolicy,
            std::enable_if_t<Q != concurrent_queue_wait_policy::non_blocking, int> = 0>
  bool push_blocking(U&& elem) noexcept
  {
    return queue.push_blocking(std::forward<U>(elem));
  }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns false.
  bool try_pop(T& elem) { return queue.try_pop(elem); }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns an empty optional.
  std::optional<T> try_pop()
  {
    std::optional<T> ret;
    ret.emplace();
    if (not queue.try_pop(ret.value())) {
      ret.reset();
    }
    return ret;
  }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  bool pop_blocking(T& elem) { return queue.pop_blocking(elem); }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  std::optional<T> pop_blocking() noexcept
  {
    std::optional<T> ret;
    ret.emplace();
    if (not queue.pop_blocking(ret.value())) {
      ret.reset();
    }
    return ret;
  }

  /// \brief Provides a pointer to the front element in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns a nullptr.
  template <concurrent_queue_policy queue_policy                                          = Policy,
            std::enable_if_t<queue_policy == concurrent_queue_policy::lockfree_spsc, int> = 0>
  T* front()
  {
    return queue.front();
  }

  /// \brief Maximum capacity of the queue.
  size_t capacity() const { return queue.capacity(); }

  /// \brief Current size of the queue.
  size_t size() const { return queue.size(); }

  /// \brief Determines whether the queue is empty.
  bool empty() const { return queue.empty(); }

  /// \brief Request any blocking function to be interrupted.
  void request_stop() { queue.request_stop(); }

private:
  queue_type queue;
};

} // namespace srsran
