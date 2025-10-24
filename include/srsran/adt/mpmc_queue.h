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

#include "rigtorp/MPMCQueue.h"
#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"
#include <chrono>

namespace srsran {

/// Specialization for lockfree MPMC without a blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::lockfree_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::non_blocking;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  explicit concurrent_queue(size_t qsize) : queue(qsize) {}

  [[nodiscard]] bool try_push(const T& elem) { return queue.try_push(elem); }
  [[nodiscard]] bool try_push(T&& elem) { return queue.try_push(std::move(elem)); }
  template <typename U>
  [[nodiscard]] size_t try_push_bulk(span<U> batch)
  {
    return detail::queue_helper::try_push_bulk_generic(*this, batch);
  }
  template <typename U>
  void push_blocking(U&& elem)
  {
    queue.push(std::forward<U>(elem));
  }

  [[nodiscard]] bool try_pop(T& elem) { return queue.try_pop(elem); }

  [[nodiscard]] size_t try_pop_bulk(span<T> batch) { return detail::queue_helper::try_pop_bulk_generic(*this, batch); }

  [[nodiscard]] size_t size() const
  {
    // Note: MPMCqueue size can be negative.
    ptrdiff_t ret = queue.size();
    return static_cast<size_t>(std::max(ret, static_cast<ptrdiff_t>(0)));
  }

  [[nodiscard]] bool empty() const { return queue.empty(); }

  [[nodiscard]] size_t capacity() const { return queue.capacity(); }

  /// Creates a consumer for this queue.
  consumer_type create_consumer() { return consumer_type(*this); }

protected:
  ::rigtorp::MPMCQueue<T> queue;
};

/// Specialization for lockfree MPMC using a sleep as the blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>
  : private concurrent_queue<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>,
    public detail::queue_sleep_crtp<
        concurrent_queue<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>>
{
  using nonblocking_base_type =
      concurrent_queue<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;
  using sleep_base_type = detail::queue_sleep_crtp<concurrent_queue>;

public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::lockfree_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::sleep;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  explicit concurrent_queue(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    nonblocking_base_type(qsize), sleep_base_type(sleep_time_)
  {
  }

  using nonblocking_base_type::capacity;
  using nonblocking_base_type::empty;
  using nonblocking_base_type::size;
  using nonblocking_base_type::try_pop;
  using nonblocking_base_type::try_push;
  using sleep_base_type::call_on_pop_blocking;
  using sleep_base_type::pop_blocking;
  using sleep_base_type::push_blocking;
  using sleep_base_type::request_stop;

  /// Creates a consumer for this queue.
  consumer_type create_consumer() { return consumer_type(*this); }
};

} // namespace srsran
