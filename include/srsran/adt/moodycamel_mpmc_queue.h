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

#include "cameron314/concurrentqueue.h"
#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"
#include "srsran/adt/span.h"

namespace srsran {

/// Specialization for moodycamel lockfree MPMC without a blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::moodycamel_lockfree_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::non_blocking;

  explicit concurrent_queue(size_t qsize) : queue(qsize) {}

  bool try_push(const T& elem) { return queue.enqueue(elem); }
  bool try_push(T&& elem) { return queue.enqueue(std::move(elem)); }
  template <typename U>
  bool try_push_bulk(span<U> batch)
  {
    return queue.enqueue_bulk(batch);
  }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns false.
  bool try_pop(T& elem) { return queue.try_dequeue(elem); }

  bool try_pop_bulk(span<const T> batch) { return queue.try_dequeue_bulk(batch); }

  /// Returns an estimate of the total number of elements currently in the queue.
  size_t size() const { return queue.size_approx(); }

  /// Returns an estimate on whether the queue is empty.
  bool empty() const { return size() == 0; }

  /// This queue is unbounded. It allocates when current capacity is reached.
  size_t capacity() const { return std::numeric_limits<size_t>::max(); }

protected:
  moodycamel::ConcurrentQueue<T> queue;
};

/// Specialization for moodycamel lockfree MPMC with a blocking mechanism based on sleeps.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::sleep>
  : public detail::queue_sleep_crtp<
        concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::sleep>>,
    private concurrent_queue<T,
                             concurrent_queue_policy::moodycamel_lockfree_mpmc,
                             concurrent_queue_wait_policy::non_blocking>

{
  using non_block_queue_base = concurrent_queue<T,
                                                concurrent_queue_policy::moodycamel_lockfree_mpmc,
                                                concurrent_queue_wait_policy::non_blocking>;
  using blocking_ext_base    = detail::queue_sleep_crtp<concurrent_queue>;

public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::moodycamel_lockfree_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::sleep;

  // Inherited non-blocking API methods.
  using non_block_queue_base::capacity;
  using non_block_queue_base::empty;
  using non_block_queue_base::size;
  using non_block_queue_base::try_pop;
  using non_block_queue_base::try_pop_bulk;
  using non_block_queue_base::try_push;
  using non_block_queue_base::try_push_bulk;

  // Inherited blocking API methods
  using blocking_ext_base::call_on_pop_blocking;
  using blocking_ext_base::pop_blocking;
  using blocking_ext_base::push_blocking;
  using blocking_ext_base::request_stop;

  explicit concurrent_queue(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    blocking_ext_base(sleep_time_), non_block_queue_base(qsize)
  {
  }
};

} // namespace srsran
