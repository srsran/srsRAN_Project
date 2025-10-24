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

#include "rigtorp/SPSCQueue.h"
#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"
#include <chrono>

namespace srsran {

/// Specialization for lockfree SPSC without a blocking API.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>
{
public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::lockfree_spsc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::non_blocking;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  explicit concurrent_queue(size_t qsize) : queue(qsize) {}

  template <typename U>
  [[nodiscard]] bool try_push(U&& elem)
  {
    return queue.try_push(std::forward<U>(elem));
  }
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

  [[nodiscard]] bool try_pop(T& elem)
  {
    T* front = queue.front();
    if (front != nullptr) {
      elem = std::move(*front);
      queue.pop();
      return true;
    }
    return false;
  }

  [[nodiscard]] size_t try_pop_bulk(span<T> batch) { return detail::queue_helper::try_pop_bulk_generic(*this, batch); }

  /// \brief Provides a pointer to the front element in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns a nullptr.
  [[nodiscard]] T* front() { return queue.front(); }

  [[nodiscard]] size_t size() const { return queue.size(); }

  [[nodiscard]] bool empty() const { return queue.empty(); }

  [[nodiscard]] size_t capacity() const { return queue.capacity(); }

  /// Creates a sequential consumer for this queue.
  consumer_type create_consumer() { return consumer_type(*this); }

protected:
  ::rigtorp::SPSCQueue<T> queue;
};

/// Specialization for lockfree SPSC using a spin sleep loop as the blocking mechanism.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>
  : private concurrent_queue<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>,
    public detail::queue_sleep_crtp<
        concurrent_queue<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>>
{
  using non_blocking_base_type =
      concurrent_queue<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>;
  using blocking_base_type = detail::queue_sleep_crtp<concurrent_queue>;

public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::lockfree_spsc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::sleep;
  using consumer_type                                        = detail::basic_queue_consumer<concurrent_queue, T>;

  template <typename... Args>
  explicit concurrent_queue(size_t qsize, std::chrono::microseconds sleep_time_) :
    non_blocking_base_type(qsize), blocking_base_type(sleep_time_)
  {
  }

  // Inherited methods.
  using blocking_base_type::push_blocking;
  using blocking_base_type::request_stop;
  using non_blocking_base_type::capacity;
  using non_blocking_base_type::empty;
  using non_blocking_base_type::front;
  using non_blocking_base_type::size;
  using non_blocking_base_type::try_pop;
  using non_blocking_base_type::try_push;

  bool pop_blocking(T& elem) noexcept
  {
    T* f = front_blocking();
    if (f != nullptr) {
      elem = std::move(*f);
      this->queue.pop();
      return true;
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    T* f = front_blocking();
    if (f != nullptr) {
      // skip std::move
      func(*f);
      this->queue.pop();
      return true;
    }
    return false;
  }

  /// Creates a consumer for this queue.
  consumer_type create_consumer() { return consumer_type(*this); }

private:
  T* front_blocking()
  {
    while (this->policy.is_running()) {
      T* front_val = this->queue.front();
      if (front_val != nullptr) {
        return front_val;
      }
      this->policy.wait();
    }
    return nullptr;
  }
};

} // namespace srsran
