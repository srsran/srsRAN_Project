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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/detail/concurrent_queue_barrier.h"
#include "srsran/adt/ring_buffer.h"

namespace srsran {

namespace detail {

/// Specialization for lock-based MPSC, using a condition variable or sleep as blocking mechanism. The dequeues are done
/// in a batch to minimize mutex contention.
template <typename T, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl<T, concurrent_queue_policy::locking_mpsc, BlockingPolicy>
{
  using queue_barrier = std::conditional_t<BlockingPolicy == concurrent_queue_wait_policy::condition_variable,
                                           queue_cond_var_barrier,
                                           queue_sleep_barrier>;

  const size_t                        cap;
  std::atomic<unsigned>               count_local_objs{0};
  std::array<ring_buffer<T, true>, 2> queues;
  unsigned                            index_queue_for_pop = 0;
  mutable std::mutex                  mutex;
  queue_barrier                       barrier;

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, Args&&... args) :
    cap(qsize),
    queues({ring_buffer<T, true>{(unsigned)qsize}, ring_buffer<T, true>{(unsigned)qsize}}),
    barrier(std::forward<Args>(args)...)
  {
  }

  void request_stop()
  {
    std::lock_guard<std::mutex> lock(mutex);
    pushing_queue().clear();
    barrier.request_stop();
  }

  template <typename U>
  bool try_push(U&& elem)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (barrier.is_running() and not pushing_queue().full()) {
      pushing_queue().push(std::forward<U>(elem));
      barrier.notify_push();
      return true;
    }
    return false;
  }

  template <typename U>
  bool push_blocking(U&& elem)
  {
    std::unique_lock<std::mutex> lock(mutex);
    barrier.wait_push(lock, [this]() { return not pushing_queue().full(); });
    if (barrier.is_running()) {
      pushing_queue().push(std::forward<U>(elem));
      barrier.notify_push();
      return true;
    }
    return false;
  }

  bool try_pop(T& elem)
  {
    T* f = front<false>();
    if (f != nullptr) {
      elem = std::move(*f);
      pop();
      return true;
    }
    return false;
  }

  bool pop_blocking(T& elem) noexcept
  {
    T* f = front<true>();
    if (f != nullptr) {
      elem = std::move(*f);
      pop();
      return true;
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    T* f = front<true>();
    if (f != nullptr) {
      func(*f);
      pop();
      return true;
    }
    return false;
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return pushing_queue().size() + count_local_objs.load(std::memory_order_relaxed);
  }

  bool empty() const
  {
    if (count_local_objs.load(std::memory_order_relaxed) > 0) {
      return false;
    }
    std::lock_guard<std::mutex> lock(mutex);
    return pushing_queue().empty();
  }

  size_t capacity() const { return cap; }

private:
  template <bool Blocking>
  T* front()
  {
    if (not barrier.is_running()) {
      return nullptr;
    }
    if (not popping_queue().empty()) {
      // Successful pop.
      return &popping_queue().top();
    }
    {
      std::unique_lock<std::mutex> lock(mutex);
      if (Blocking) {
        barrier.wait_pop(lock, [this]() { return !pushing_queue().empty(); });
        if (not barrier.is_running()) {
          return nullptr;
        }
      } else if (pushing_queue().empty()) {
        return nullptr;
      }
      // Swap queues.
      index_queue_for_pop = 1 - index_queue_for_pop;
    }
    barrier.notify_pop();
    count_local_objs.store(popping_queue().size(), std::memory_order_relaxed);
    return &popping_queue().top();
  }

  void pop()
  {
    popping_queue().pop();
    count_local_objs.fetch_sub(1, std::memory_order_relaxed);
  }

  ring_buffer<T, true>& popping_queue() { return queues[index_queue_for_pop]; }

  ring_buffer<T, true>& pushing_queue() { return queues[1 - index_queue_for_pop]; }

  const ring_buffer<T, true>& pushing_queue() const { return queues[1 - index_queue_for_pop]; }
};

} // namespace detail

} // namespace srsran
