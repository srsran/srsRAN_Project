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

#include "rigtorp/MPMCQueue.h"
#include "rigtorp/SPSCQueue.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/detail/tuple_utils.h"
#include "srsran/adt/optional.h"
#include "srsran/support/compiler.h"

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
enum class concurrent_queue_policy { lockfree_spsc, lockfree_mpmc, locking_mpmc, locking_mpsc };

/// \brief Types of barriers used for blocking pushes/pops of elements. Three types:
/// - condition_variable: uses a condition variable to wake up producers and consumers.
/// - sleep: spins on a sleep if the queue is full, in case of blocking push, and if the queue is empty in case of
/// blocking pop.
/// - non_blocking: no blocking mechanism is exposed.
enum class concurrent_queue_wait_policy { condition_variable, sleep, non_blocking };

namespace detail {

template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class queue_impl;

// Specialization for lockfree SPSC without blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::non_blocking>
{
public:
  template <typename... Args>
  explicit queue_impl(size_t qsize) : queue(qsize)
  {
  }

  template <typename U>
  bool try_push(U&& elem)
  {
    return queue.try_push(std::forward<U>(elem));
  }

  bool try_pop(T& elem)
  {
    T* front = queue.front();
    if (front != nullptr) {
      elem = std::move(*front);
      queue.pop();
      return true;
    }
    return false;
  }

  template <typename PoppingFunc>
  bool try_call_on_pop(PoppingFunc&& func)
  {
    T* front = queue.front();
    if (front != nullptr) {
      func(*front);
      queue.pop();
      return true;
    }
    return false;
  }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.capacity(); }

protected:
  rigtorp::SPSCQueue<T> queue;
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
      func(*f);
      this->queue.pop();
      return true;
    }
    return false;
  }

private:
  T* front_blocking()
  {
    while (running.load(std::memory_order_relaxed)) {
      T* front = this->queue.front();
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

// Specialization for lockfree MPMC using no blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
public:
  template <typename... Args>
  explicit queue_impl(size_t qsize) : queue(qsize)
  {
  }

  bool try_push(const T& elem) { return queue.try_push(elem); }
  bool try_push(T&& elem) { return queue.try_push(std::move(elem)); }

  bool try_pop(T& elem) { return queue.try_pop(elem); }

  template <typename PoppingFunc>
  bool try_call_on_pop(PoppingFunc&& func)
  {
    T t;
    if (queue.try_pop(t)) {
      func(t);
      return true;
    }
    return false;
  }

  size_t size() const
  {
    // Note: MPMCqueue size can be negative.
    ptrdiff_t ret = queue.size();
    return static_cast<size_t>(std::max(ret, static_cast<ptrdiff_t>(0)));
  }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.capacity(); }

protected:
  rigtorp::MPMCQueue<T> queue;
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

// Specialization for lock-based MPMC, using a condition variable as blocking mechanism.
template <typename T>
class queue_impl<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
{
public:
  explicit queue_impl(size_t qsize) : queue(qsize) {}

  void request_stop() { queue.stop(); }

  bool try_push(const T& elem) { return queue.try_push(elem); }
  bool try_push(T&& elem) { return not queue.try_push(std::move(elem)).is_error(); }
  bool push_blocking(const T& elem) { return queue.push_blocking(elem); }
  bool push_blocking(T&& elem) { return not queue.push_blocking(std::move(elem)).is_error(); }

  optional<T> try_pop()
  {
    optional<T> t;
    t.emplace();
    if (not queue.try_pop(t.value())) {
      t.reset();
    }
    return t;
  }

  bool try_pop(T& elem) { return queue.try_pop(elem); }

  template <typename PoppingFunc>
  bool try_call_on_pop(PoppingFunc&& func)
  {
    T t;
    if (queue.try_pop(t)) {
      func(t);
      return true;
    }
    return false;
  }

  bool pop_blocking(T& elem)
  {
    bool success = false;
    elem         = queue.pop_blocking(&success);
    return success;
  }

  template <typename PoppingFunc>
  bool call_on_pop_blocking(PoppingFunc&& func)
  {
    bool success = false;
    T    elem    = queue.pop_blocking(&success);
    if (success) {
      func(elem);
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
  bool try_call_on_pop(PoppingFunc&& func)
  {
    T* f = front<false>();
    if (f != nullptr) {
      func(*f);
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

  ring_buffer<T, true>&       popping_queue() { return queues[index_queue_for_pop]; }
  ring_buffer<T, true>&       pushing_queue() { return queues[1 - index_queue_for_pop]; }
  const ring_buffer<T, true>& pushing_queue() const { return queues[1 - index_queue_for_pop]; }

  const size_t                        cap;
  std::atomic<unsigned>               count_local_objs{0};
  std::array<ring_buffer<T, true>, 2> queues;
  unsigned                            index_queue_for_pop = 0;
  mutable std::mutex                  mutex;
  queue_barrier                       barrier;
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
  template <typename U>
  bool try_push(U&& elem) noexcept
  {
    return queue.try_push(std::forward<U>(elem));
  }

  /// Pushes a new element into the queue. If the queue is full, the call blocks, waiting for a new slot to become
  /// emptied.
  /// \return true if the element was pushed, false if the queue was closed.
  template <typename U,
            concurrent_queue_wait_policy Q                                         = BlockingPolicy,
            std::enable_if_t<Q != concurrent_queue_wait_policy::non_blocking, int> = 0>
  bool push_blocking(U&& elem) noexcept
  {
    return queue.push_blocking(std::forward<U>(elem));
  }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns false.
  bool try_pop(T& elem) { return queue.try_pop(elem); }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns an empty optional.
  optional<T> try_pop()
  {
    optional<T> ret;
    queue.try_call_on_pop([&ret](T& t) mutable { ret = std::move(t); });
    return ret;
  }

  /// \brief Pops an element from the queue and calls the provided function with the popped element.
  ///
  /// If the queue is empty, the call returns false, and the CallOnPop function is not called. Otherwise, it returns
  /// true.
  template <typename CallOnPop>
  bool try_call_on_pop(const CallOnPop& func)
  {
    return queue.try_call_on_pop(func);
  }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  bool pop_blocking(T& elem) { return queue.pop_blocking(elem); }

  /// Pops an element from the queue. If the queue is empty, the call blocks, waiting for a new element to be pushed.
  optional<T> pop_blocking() noexcept
  {
    optional<T> ret;
    queue.call_on_pop_blocking([&ret](T& t) mutable { ret = std::move(t); });
    return ret;
  }

  /// \brief Pops an element from the queue and calls the provided function with the popped element. If the queue is
  /// empty, the function blocks, waiting for a new element to be pushed. It returns false if the queue is closed.
  template <typename CallOnPop>
  bool call_on_pop_blocking(const CallOnPop& func)
  {
    return queue.call_on_pop_blocking(func);
  }

  /// \brief Maximum capacity of the queue.
  size_t capacity() const { return queue.capacity(); }

  /// \brief Current size of the queue.
  size_t size() const { return queue.size(); }

  /// \brief Determines whether the queue is empty.
  bool empty() const { return queue.empty(); }

  /// \brief Request any blocking function to be interrupted.
  void request_stop() { queue.request_stop(); }

private:
  queue_type queue;
};

/// \brief Queue priority used to map to specific queue of the \c priority_multiqueue_task_worker. The higher the
/// priority, the lower its integer value representation.
enum class enqueue_priority : size_t { min = 0, max = std::numeric_limits<size_t>::max() };

/// Reduce priority by \c dec amount.
constexpr enqueue_priority operator-(enqueue_priority lhs, size_t dec)
{
  return static_cast<enqueue_priority>(static_cast<size_t>(lhs) - dec);
}

namespace detail {

static constexpr size_t enqueue_priority_to_queue_index(enqueue_priority prio, size_t nof_priority_levels)
{
  size_t queue_idx = std::numeric_limits<size_t>::max() - static_cast<size_t>(prio);
  return queue_idx < nof_priority_levels ? queue_idx : nof_priority_levels - 1;
}

static constexpr enqueue_priority queue_index_to_enqueue_priority(size_t queue_idx, size_t nof_priority_levels)
{
  return static_cast<enqueue_priority>(std::numeric_limits<size_t>::max() - queue_idx);
}

} // namespace detail

/// \brief Concurrent priority queue, where the caller specifies the element priority statically while pushing it to
/// the queue.
///
/// The prioritization is achieved via multiple queues. The pop functions will always start with the highest priority
/// queue until it is depleted, and then move to the second highest priority queue, and so on.
/// \tparam T The type of the pushed/popped object.
/// \tparam QueuePolicies The queueing policies of each of the queues of different priorities.
template <typename T, concurrent_queue_policy... QueuePolicies>
class concurrent_priority_queue
{
public:
  /// \brief Construct a new prioritized queue.
  /// \param task_queue_sizes The sizes of the task queues for each priority level.
  /// \param wait_interval The amount of time to suspend the thread, when no tasks are pending.
  concurrent_priority_queue(const std::array<unsigned, sizeof...(QueuePolicies)>& priority_queue_sizes,
                            std::chrono::microseconds                             wait_interval_) :
    queues(detail::as_tuple(priority_queue_sizes)), wait_interval(wait_interval_)
  {
  }

  /// \brief Push new object with priority level \c Priority.
  template <enqueue_priority Priority, typename U>
  SRSRAN_NODISCARD bool try_push(U&& u)
  {
    return std::get<detail::enqueue_priority_to_queue_index(Priority, nof_priority_levels())>(queues).try_push(
        std::forward<U>(u));
  }

  /// \brief Push new object with priority level \c Priority.
  template <enqueue_priority Priority, typename U>
  SRSRAN_NODISCARD bool push_blocking(U&& u)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (try_push<Priority>(std::forward<U>(u))) {
        return true;
      }
      std::this_thread::sleep_for(wait_interval);
    }
    return false;
  }

  /// \brief Pop object from queue, by starting first with the objects with highest priority.
  bool try_pop(T& elem)
  {
    return detail::any_of(queues, [&elem](auto& queue) mutable { return queue.try_pop(elem); });
  }

  optional<T> try_pop()
  {
    optional<T> elem;
    detail::any_of(queues, [&elem](auto& queue) mutable {
      elem = queue.try_pop();
      return elem.has_value();
    });
    return elem;
  }

  /// \brief Call function on object popped from the queue.
  template <typename CallOnPop>
  bool try_call_on_pop(const CallOnPop& func)
  {
    return detail::any_of(queues, [&func](auto& queue) mutable { return queue.try_call_on_pop(func); });
  }

  bool pop_blocking(T& elem)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (try_pop(elem)) {
        return true;
      }
      std::this_thread::sleep_for(wait_interval);
    }
    return false;
  }

  optional<T> pop_blocking()
  {
    optional<T> t;
    while (running.load(std::memory_order_relaxed)) {
      t = try_pop();
      if (t.has_value()) {
        break;
      }
      std::this_thread::sleep_for(wait_interval);
    }
    return t;
  }

  template <typename CallOnPop>
  bool call_on_pop_blocking(const CallOnPop& func)
  {
    while (running.load(std::memory_order_relaxed)) {
      if (try_call_on_pop(func)) {
        return true;
      }
      std::this_thread::sleep_for(wait_interval);
    }
    return false;
  }

  void request_stop() { running = false; }

  /// \brief Get specified priority queue capacity.
  template <enqueue_priority Priority>
  SRSRAN_NODISCARD size_t capacity() const
  {
    return std::get<detail::enqueue_priority_to_queue_index(Priority, nof_priority_levels())>(queues).capacity();
  }

  /// Number of priority levels supported by this queue.
  static constexpr size_t nof_priority_levels() { return sizeof...(QueuePolicies); }

private:
  std::tuple<concurrent_queue<T, QueuePolicies, concurrent_queue_wait_policy::non_blocking>...> queues;

  // Time that thread spends sleeping when there are no pending tasks.
  const std::chrono::microseconds wait_interval;

  std::atomic<bool> running{true};
};

} // namespace srsran