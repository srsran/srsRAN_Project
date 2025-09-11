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

#include <limits>

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
/// - moodycamel_lockfree_mpmc: Lock-free MPMC queue with unbounded capacity and that does not ensure elements
/// enqueued by independent producers come out in the same order (not linearizable).
enum class concurrent_queue_policy {
  lockfree_spsc,
  lockfree_mpmc,
  locking_mpmc,
  locking_mpsc,
  moodycamel_lockfree_mpmc
};

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
  /// In case of moodycamel MPMC, the number of pre-reserved producers can be set.
  unsigned nof_prereserved_producers = 2;
};

/// \brief Queue priority used to map to specific queue of the \c priority_multiqueue_task_worker. The higher the
/// priority, the lower its integer value representation.
enum class enqueue_priority : std::size_t { min = 0, max = std::numeric_limits<std::size_t>::max() };

/// Reduce priority by \c dec amount.
constexpr enqueue_priority operator-(enqueue_priority lhs, std::size_t dec)
{
  return static_cast<enqueue_priority>(static_cast<std::size_t>(lhs) - dec);
}

/// \brief A concurrent queue that can be used to pass objects between threads. Different policies are supported:
/// - lockfree_spsc: a lock-free single-producer single-consumer queue. This is the fastest option.
/// - lockfree_mpmc: a lock-free multi-producer multi-consumer queue.
/// - locking_mpmc: a multi-producer multi-consumer queue that uses a mutex to protect the queue. This is the most
/// general queue, but has the highest overhead. The producers and consumers will contend on the access to the mutex.
/// - locking_mpsc: a multi-producer single-consumer queue that uses a mutex to protect the queue. This queue pops
/// all elements in a batch to minimize the contention on the mutex from the consumer side.
/// - moodycamel_lockfree_mpmc: Lock-free MPMC queue with unbounded capacity and that does not ensure elements
/// enqueued by independent producers come out in the same order (not linearizable).
template <typename T,
          concurrent_queue_policy      Policy,
          concurrent_queue_wait_policy BlockingPolicy = concurrent_queue_wait_policy::non_blocking>
class concurrent_queue;

} // namespace srsran
