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

#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"

namespace srsran {

/// Specialization for lock-based MPMC without a blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::locking_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::non_blocking;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  explicit concurrent_queue(size_t qsize) : queue(qsize) {}

  /// Pushes a new element into the queue in a non-blocking fashion. If the queue is full, the element is not pushed.
  /// \return true if the element was pushed, false otherwise.
  [[nodiscard]] bool try_push(const T& elem) { return queue.try_push(elem); }
  [[nodiscard]] bool try_push(T&& elem) { return queue.try_push(std::move(elem)).has_value(); }
  template <typename U>
  [[nodiscard]] size_t try_push_bulk(span<U> batch)
  {
    return detail::queue_helper::try_push_bulk_generic(*this, batch);
  }

  /// Pushes a new element into the queue in a blocking fashion. If the queue is full, the call blocks until space is
  /// made available.
  template <typename U>
  void push_blocking(U&& elem)
  {
    queue.push_blocking(std::forward<U>(elem));
  }

  [[nodiscard]] bool try_pop(T& elem) { return queue.try_pop(elem); }

  /// \brief Pops a batch of elements from the queue in a non-blocking fashion.
  [[nodiscard]] size_t try_pop_bulk(span<T> batch) { return detail::queue_helper::try_pop_bulk_generic(*this, batch); }

  void clear() { queue.clear(); }

  [[nodiscard]] size_t size() const { return queue.size(); }

  [[nodiscard]] bool empty() const { return queue.empty(); }

  [[nodiscard]] size_t capacity() const { return queue.max_size(); }

  consumer_type create_consumer() { return consumer_type(*this); }

protected:
  blocking_queue<T> queue;
};

/// Specialization for lock-based MPMC, using a condition variable as the blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
  : private concurrent_queue<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  using non_blocking_base_type =
      concurrent_queue<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::locking_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::condition_variable;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  // Inherited methods.
  using non_blocking_base_type::capacity;
  using non_blocking_base_type::clear;
  using non_blocking_base_type::empty;
  using non_blocking_base_type::non_blocking_base_type;
  using non_blocking_base_type::size;
  using non_blocking_base_type::try_pop;
  using non_blocking_base_type::try_pop_bulk;
  using non_blocking_base_type::try_push;

  /// \brief Request any blocking function to be interrupted.
  void request_stop() { this->queue.stop(); }

  bool push_blocking(const T& elem) { return this->queue.push_blocking(elem); }
  bool push_blocking(T&& elem) { return this->queue.push_blocking(std::move(elem)).has_value(); }

  bool pop_blocking(T& elem)
  {
    bool success = false;
    elem         = this->queue.pop_blocking(&success);
    return success;
  }
  bool pop_blocking(T& elem, std::chrono::microseconds wait_time)
  {
    bool success = false;
    elem         = this->queue.pop_wait_for(&success, wait_time);
    return success;
  }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  std::optional<T> pop_blocking() noexcept
  {
    std::optional<T> ret;
    if (not pop_blocking(ret.emplace())) {
      ret.reset();
    }
    return ret;
  }

  consumer_type create_consumer() { return consumer_type(*this); }
};

} // namespace srsran
