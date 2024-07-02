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

#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/detail/concurrent_queue_barrier.h"

namespace srsran {
namespace detail {

/// Specialization for lock-based MPMC without a blocking mechanism..
template <typename T>
class queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  explicit queue_impl(size_t qsize) : queue(qsize) {}

  void request_stop() { queue.stop(); }

  bool try_push(const T& elem) { return queue.try_push(elem); }
  bool try_push(T&& elem) { return queue.try_push(std::move(elem)).has_value(); }

  std::optional<T> try_pop()
  {
    std::optional<T> t;
    t.emplace();
    if (not queue.try_pop(t.value())) {
      t.reset();
    }
    return t;
  }

  bool try_pop(T& elem) { return queue.try_pop(elem); }

  void clear() { queue.clear(); }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.max_size(); }

protected:
  blocking_queue<T> queue;
};

/// Specialization for lock-based MPMC, using a condition variable as the blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
  : public queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  using base_type = queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  explicit queue_impl(size_t qsize) : base_type(qsize) {}

  bool push_blocking(const T& elem) { return this->queue.push_blocking(elem); }
  bool push_blocking(T&& elem) { return this->queue.push_blocking(std::move(elem)).has_value(); }

  bool pop_blocking(T& elem)
  {
    bool success = false;
    elem         = this->queue.pop_blocking(&success);
    return success;
  }
};

} // namespace detail
} // namespace srsran
