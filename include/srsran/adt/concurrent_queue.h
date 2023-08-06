/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/optional.h"
#include "srsran/support/compiler.h"

namespace srsran {

/// \brief Types of concurrent queues. They differ in type of synchronization mechanism and number of
/// producers/consumers. Supported types are:
/// - lockfree_spsc: lockfree single producer single consumer queue.
/// - locking_mpmc: multiple producer multiple consumer queue that uses a mutex for synchronization. It is the most
/// generic type of queue, but it is also the slowest. It relies on a condition variable to wake up producers and
/// consumers.
/// - locking_mpsc: similar to the locking_mpmc, but it leverages batch popping on the consumer side, to reduce
/// mutex contention.
enum class concurrent_queue_policy { lockfree_spsc, locking_mpmc, locking_mpsc };

/// Types of barriers used for blocking pushes/pops of elements.
enum class concurrent_queue_wait_policy { condition_variable, sleep };

namespace detail {

template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl;

// Specialization for lockfree SPSC using a sleep as blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>
{
public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    queue(qsize), sleep_time(sleep_time_)
  {
  }

  void request_stop() { running = false; }

  template <bool BlockOnFull, typename U>
  bool push(U&& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (queue.try_push(std::forward<U>(elem))) {
        return true;
      }
      if (BlockOnFull) {
        std::this_thread::sleep_for(sleep_time);
      } else {
        break;
      }
    }
    return false;
  }

  template <bool BlockOnEmpty>
  optional<T> pop()
  {
    optional<T> ret;
    T*          f = front<BlockOnEmpty>();
    if (f != nullptr) {
      ret = std::move(*f);
      queue.pop();
      return ret;
    }
    return nullopt;
  }

  template <bool BlockOnEmpty, typename PoppingFunc>
  bool call_and_pop(const PoppingFunc& func)
  {
    T* ret = front<BlockOnEmpty>();
    if (ret != nullptr) {
      func(*ret);
      queue.pop();
      return true;
    }
    return false;
  }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.capacity(); }

  void clear()
  {
    while (queue.front()) {
      queue.pop();
    }
  }

private:
  template <bool BlockOnEmpty>
  T* front()
  {
    while (running.load(std::memory_order_relaxed)) {
      T* front = queue.front();
      if (not BlockOnEmpty or front != nullptr) {
        return front;
      }
      std::this_thread::sleep_for(sleep_time);
    }
    return nullptr;
  }

  rigtorp::SPSCQueue<T>     queue;
  std::chrono::microseconds sleep_time;
  std::atomic<bool>         running{true};
};

// Specialization for lock-based MPMC, using a condition variable as blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
{
public:
  explicit queue_impl(size_t qsize) : queue(qsize) {}

  void request_stop() { queue.stop(); }

  template <bool BlockOnFull>
  bool push(const T& elem) noexcept
  {
    if (BlockOnFull) {
      return queue.push_blocking(elem);
    }
    return queue.try_push(elem);
  }

  template <bool BlockOnFull>
  bool push(T&& elem) noexcept
  {
    if (BlockOnFull) {
      return queue.push_blocking(std::move(elem)).has_value();
    }
    return queue.try_push(std::move(elem)).has_value();
  }

  template <bool BlockOnEmpty>
  optional<T> pop()
  {
    T t;
    if (BlockOnEmpty) {
      bool success = false;
      t            = queue.pop_blocking(&success);
      return success ? optional<T>{std::move(t)} : optional<T>{};
    }
    if (queue.try_pop(t)) {
      return optional<T>{std::move(t)};
    }
    return nullopt;
  }

  template <bool BlockOnFull, typename PoppingFunc>
  bool call_and_pop(const PoppingFunc& func)
  {
    bool success = false;
    T    t;
    if (BlockOnFull) {
      t = queue.pop_blocking(&success);
    } else {
      success = queue.try_pop(t);
    }
    if (success) {
      func(t);
      return true;
    }
    return false;
  }

  void clear() { queue.clear(); }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.max_size(); }

private:
  blocking_queue<T> queue;
};

// Barrier implementation based on sleep.
struct queue_sleep_barrier {
public:
  queue_sleep_barrier(std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) : sleep_time(sleep_time_) {}

  void request_stop() { running = false; }

  void notify_push()
  {
    // do nothing.
  }

  void notify_pop()
  {
    // do nothing.
  }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    while (running.load(std::memory_order_relaxed) and not has_space()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    while (running.load(std::memory_order_relaxed) and not has_elem()) {
      lock.unlock();
      std::this_thread::sleep_for(sleep_time);
      lock.lock();
    }
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>         running{true};
  std::chrono::microseconds sleep_time;
};

// Barrier implementation based on condition variable.
struct queue_cond_var_barrier {
public:
  void request_stop()
  {
    running = false;
    notify_push();
    notify_pop();
  }

  void notify_push() { cvar_push.notify_one(); }

  void notify_pop() { cvar_pop.notify_one(); }

  template <typename HasSpace>
  void wait_push(std::unique_lock<std::mutex>& lock, const HasSpace& has_space)
  {
    cvar_pop.wait(lock, [this, &has_space]() { return not running.load(std::memory_order_relaxed) or has_space(); });
  }

  template <typename HasElem>
  void wait_pop(std::unique_lock<std::mutex>& lock, const HasElem& has_elem)
  {
    cvar_push.wait(lock, [this, &has_elem]() { return not running.load(std::memory_order_relaxed) or has_elem(); });
  }

  bool is_running() const { return running.load(std::memory_order_relaxed); }

private:
  std::atomic<bool>       running{true};
  std::condition_variable cvar_pop, cvar_push;
};

// Specialization for lock-based MPSC, using a condition variable or sleep as blocking mechanism. The dequeues are
// done in a batch to minimize mutex contention.
template <typename T, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl<T, concurrent_queue_policy::locking_mpsc, BlockingPolicy>
{
  using queue_barrier = std::conditional_t<BlockingPolicy == concurrent_queue_wait_policy::condition_variable,
                                           queue_cond_var_barrier,
                                           queue_sleep_barrier>;

public:
  template <typename... Args>
  explicit queue_impl(size_t qsize, Args&&... args) : cap(qsize), barrier(std::forward<Args>(args)...)
  {
    queue.reserve(qsize);
    popped_items.reserve(qsize);
  }

  void request_stop()
  {
    std::lock_guard<std::mutex> lock(mutex);
    queue.clear();
    barrier.request_stop();
  }

  template <bool BlockOnFull, typename U>
  bool push(U&& elem) noexcept
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (BlockOnFull) {
      barrier.wait_push(lock, [this]() { return queue.size() < cap; });
    }
    if (barrier.is_running() and queue.size() < cap) {
      queue.push_back(std::forward<U>(elem));
      barrier.notify_push();
      return true;
    }
    return false;
  }

  template <bool BlockOnEmpty>
  optional<T> pop()
  {
    T* item = pop_<BlockOnEmpty>();
    return item != nullptr ? optional<T>{std::move(*item)} : optional<T>{};
  }

  template <bool BlockOnEmpty, typename PoppingFunc>
  bool call_and_pop(const PoppingFunc& func)
  {
    T* item = pop_<BlockOnEmpty>();
    if (item != nullptr) {
      func(*item);
      return true;
    }
    return false;
  }

  void clear()
  {
    {
      std::lock_guard<std::mutex> lock(mutex);
      queue.clear();
    }
    count_local_objs.store(0, std::memory_order_relaxed);
    popped_items.clear();
    barrier.notify_pop();
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size() + count_local_objs.load(std::memory_order_relaxed);
  }

  bool empty() const
  {
    if (count_local_objs.load(std::memory_order_relaxed) > 0) {
      return false;
    }
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
  }

  size_t capacity() const { return cap; }

private:
  template <bool Blocking>
  T* pop_()
  {
    unsigned count = count_local_objs.load(std::memory_order_relaxed);
    if (barrier.is_running() and count > 0) {
      count_local_objs.fetch_sub(1, std::memory_order_relaxed);
      T* t = &popped_items[popped_items.size() - count];
      return t;
    }
    popped_items.clear();
    {
      std::unique_lock<std::mutex> lock(mutex);
      if (Blocking) {
        barrier.wait_pop(lock, [this]() { return !queue.empty(); });
      }
      if (not barrier.is_running() or queue.empty()) {
        return nullptr;
      }
      popped_items.swap(queue);
    }
    barrier.notify_pop();
    count_local_objs.store(popped_items.size() - 1, std::memory_order_relaxed);
    return &popped_items[0];
  }

  const size_t          cap;
  std::atomic<unsigned> count_local_objs{0};
  std::vector<T>        queue, popped_items;
  mutable std::mutex    mutex;
  queue_barrier         barrier;
};

} // namespace detail

/// \brief A concurrent queue that can be used to pass objects between threads. Different policies are supported:
/// - lockfree_spsc: a lock-free single-producer single-consumer queue. This is the fastest option.
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
  bool try_push(T&& elem) noexcept { return queue.template push<false>(std::move(elem)); }
  bool try_push(const T& elem) noexcept { return queue.template push<false>(elem); }

  /// Pushes a new element into the queue. If the queue is full, the call blocks, waiting for a new slot to become
  /// emptied.
  /// \return true if the element was pushed, false if the queue was closed.
  bool push_blocking(T&& elem) noexcept { return queue.template push<true>(std::move(elem)); }
  bool push_blocking(const T& elem) noexcept { return queue.template push<true>(elem); }

  /// Pops an element from the queue in a non-blocking fashion. If the queue is empty, the call returns an empty
  /// optional.
  optional<T> try_pop() { return queue.template pop<false>(); }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  optional<T> pop_blocking() { return queue.template pop<true>(); }

  /// \brief Pops an element from the queue and calls the provided function with the popped element. If the queue is
  /// empty, the call returns false. Otherwise, it returns true.
  template <typename CallOnPop>
  bool try_pop(const CallOnPop& func)
  {
    return queue.template call_and_pop<false, CallOnPop>(func);
  }

  /// \brief Pops an element from the queue and calls the provided function with the popped element. If the queue is
  /// empty, the function blocks, waiting for a new element to be pushed. It returns false if the queue is closed.
  template <typename CallOnPop>
  bool pop_blocking(const CallOnPop& func)
  {
    return queue.template call_and_pop<true, CallOnPop>(func);
  }

  /// \brief Empties the queue.
  void clear() { queue.clear(); }

  /// \brief Maximum capacity of the queue.
  size_t capacity() const { return queue.capacity(); }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  void request_stop() { queue.request_stop(); }

private:
  queue_type queue;
};

} // namespace srsran