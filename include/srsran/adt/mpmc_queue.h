/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rigtorp/MPMCQueue.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/error_handling.h"
#include <chrono>
#include <thread>

namespace srsran {
namespace detail {

// Specialization for lockfree MPMC using no blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  struct custom_deleter {
    void operator()(::rigtorp::MPMCQueue<T>* ptr) const
    {
      using mpmc_queue = ::rigtorp::MPMCQueue<T>;
      if (ptr != nullptr) {
        ptr->~mpmc_queue();
        free(ptr);
      }
    }
  };

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize)
  {
    // Note: Pre-c++17 does not support new with alignof > alignof(max_align_t).
    void* ptr = nullptr;
    int   ret = posix_memalign(&ptr, alignof(::rigtorp::MPMCQueue<T>), sizeof(::rigtorp::MPMCQueue<T>));
    report_error_if_not(ret == 0, "Unable to allocate memory for MPMCQueue");
    queue.reset(new (ptr)::rigtorp::MPMCQueue<T>(qsize));
  }

  bool try_push(const T& elem) { return queue->try_push(elem); }
  bool try_push(T&& elem) { return queue->try_push(std::move(elem)); }

  bool try_pop(T& elem) { return queue->try_pop(elem); }

  size_t size() const
  {
    // Note: MPMCqueue size can be negative.
    ptrdiff_t ret = queue->size();
    return static_cast<size_t>(std::max(ret, static_cast<ptrdiff_t>(0)));
  }

  bool empty() const { return queue->empty(); }

  size_t capacity() const { return queue->capacity(); }

protected:
  std::unique_ptr<::rigtorp::MPMCQueue<T>, custom_deleter> queue;
};

// Specialization for lockfree MPMC using a sleep as blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>
  : public queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  using base_type = queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    base_type(qsize), sleep_time(sleep_time_)
  {
  }

  void request_stop() { running = false; }

  template <typename U>
  bool push_blocking(U&& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (this->try_push(std::forward<U>(elem))) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  bool pop_blocking(T& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (this->try_pop(elem)) {
        return true;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    T elem;
    if (pop_blocking(elem)) {
      func(elem);
      return true;
    }
    return false;
  }

private:
  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

} // namespace detail
} // namespace srsran