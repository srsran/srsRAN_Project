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

#include "srsran/adt/optional.h"

namespace srsran {

/// \brief Types of concurrent queues. They differ in type of synchronization mechanism and number of
/// producers/consumers supported. Supported types are:
/// - lockfree_spsc: lockfree single producer single consumer queue (SPSC).
/// - lockfree_mpmc: lockfree multiple producer multiple consumer queue (MPMC).
/// - locking_mpmc: multiple producer multiple consumer (MPMC) queue that uses a mutex for synchronization. It is the
/// most generic type of queue, but it is also the slowest. It relies on a condition variable to wake up producers and
/// consumers.
/// - locking_mpsc: similar to the locking_mpmc, but it leverages batch popping on the consumer side, to reduce
/// mutex contention.
enum class concurrent_queue_policy { lockfree_spsc, lockfree_mpmc, locking_mpmc, locking_mpsc };

/// \brief Types of barriers used for blocking pushes/pops of elements. Three types:
/// - condition_variable: uses a condition variable to wake up producers and consumers.
/// - sleep: spins on a sleep if the queue is full, in case of blocking push, and if the queue is empty in case of
/// blocking pop.
/// - non_blocking: no blocking mechanism is exposed.
enum class concurrent_queue_wait_policy { condition_variable, sleep, non_blocking };

/// \brief Parameters used to construct a concurrent queue.
struct concurrent_queue_params {
  /// \brief Queue policy to use for the task queue. E.g. SPSC, MPSC, MPMC, etc.
  concurrent_queue_policy policy;
  /// Task queue size.
  unsigned size;
};

namespace detail {

/// Implementation of the concurrent queue with the provided policies.
template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl;

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

/// \brief Queue priority used to map to specific queue of the \c priority_multiqueue_task_worker. The higher the
/// priority, the lower its integer value representation.
enum class enqueue_priority : size_t { min = 0, max = std::numeric_limits<size_t>::max() };

/// Reduce priority by \c dec amount.
constexpr enqueue_priority operator-(enqueue_priority lhs, size_t dec)
{
  return static_cast<enqueue_priority>(static_cast<size_t>(lhs) - dec);
}

} // namespace srsran
