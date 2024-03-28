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

#include "concurrent_queue.h"
#include "rigtorp/SPSCQueue.h"
#include "srsran/support/error_handling.h"
#include <chrono>
#include <thread>

namespace srsran {
namespace detail {

// Specialization for lockfree SPSC without blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>
{
  struct custom_deleter {
    void operator()(::rigtorp::SPSCQueue<T>* ptr) const
    {
      using namespace ::rigtorp;
      if (ptr != nullptr) {
        ptr->~SPSCQueue<T>();
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
    int   ret = posix_memalign(&ptr, alignof(::rigtorp::SPSCQueue<T>), sizeof(::rigtorp::SPSCQueue<T>));
    report_error_if_not(ret == 0, "Unable to allocate memory for SPSCQueue");
    queue.reset(new (ptr)::rigtorp::SPSCQueue<T>(qsize));
  }

  template <typename U>
  bool try_push(U&& elem)
  {
    return queue->try_push(std::forward<U>(elem));
  }

  bool try_pop(T& elem)
  {
    T* front = queue->front();
    if (front != nullptr) {
      elem = std::move(*front);
      queue->pop();
      return true;
    }
    return false;
  }

  size_t size() const { return queue->size(); }

  bool empty() const { return queue->empty(); }

  size_t capacity() const { return queue->capacity(); }

protected:
  std::unique_ptr<::rigtorp::SPSCQueue<T>, custom_deleter> queue;
};

// Specialization for lockfree SPSC using a spin sleep loop as blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>
  : public queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>
{
  using base_type = queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>;

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, std::chrono::microseconds sleep_time_) :
    queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>(qsize),
    sleep_time(sleep_time_)
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

  bool pop_blocking(T& elem) noexcept
  {
    T* f = front_blocking();
    if (f != nullptr) {
      elem = std::move(*f);
      this->queue->pop();
      return true;
    }
    return false;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    T* f = front_blocking();
    if (f != nullptr) {
      func(*f);
      this->queue->pop();
      return true;
    }
    return false;
  }

private:
  T* front_blocking()
  {
    while (running.load(std::memory_order_relaxed)) {
      T* front = this->queue->front();
      if (front != nullptr) {
        return front;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return nullptr;
  }

  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

} // namespace detail
} // namespace srsran