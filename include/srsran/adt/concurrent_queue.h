/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rigtorp/SPSCQueue.h"
#include "srsran/adt/blocking_queue.h"
#include "srsran/adt/optional.h"
#include "srsran/support/compiler.h"

namespace srsran {

enum class concurrent_queue_policy { lockfree_spsc, locking_mpmc, locking_mpsc };

enum class concurrent_queue_wait_policy { condition_variable, sleep };

namespace detail {

struct sleep_barrier {
public:
  sleep_barrier(std::chrono::microseconds sleep_time_) : sleep_time(sleep_time_) {}

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

private:
  std::atomic<bool>         running{true};
  std::chrono::microseconds sleep_time;
};

struct cond_var_barrier {
public:
  void request_stop() { running = false; }

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

private:
  std::atomic<bool>       running{true};
  std::condition_variable cvar_pop, cvar_push;
};

template <typename T, concurrent_queue_policy Policy, concurrent_queue_wait_policy BlockingPolicy>
class queue_wrapper;

template <typename T>
class queue_wrapper<T, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>
{
public:
  template <typename... Args>
  explicit queue_wrapper(size_t qsize, std::chrono::microseconds sleep_time_) : queue(qsize), sleep_time(sleep_time_)
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

template <typename T>
class queue_wrapper<T, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>
{
public:
  explicit queue_wrapper(size_t qsize) : queue(qsize) {}

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
    if (BlockOnEmpty) {
      return queue.pop_blocking();
    }
    T t;
    if (queue.try_pop(t)) {
      return optional<T>{std::move(t)};
    }
    return nullopt;
  }

  template <bool BlockOnFull, typename PoppingFunc>
  bool call_and_pop(const PoppingFunc& func)
  {
    optional<T> ret;
    if (BlockOnFull) {
      ret = queue.pop_blocking(func);
    } else {
      ret = queue.try_pop(func);
    }
    if (ret.has_value()) {
      func(*ret);
      return true;
    }
    return false;
  }

  size_t size() const { return queue.size(); }

  bool empty() const { return queue.empty(); }

  size_t capacity() const { return queue.max_size(); }

private:
  blocking_queue<T> queue;
};

template <typename T, concurrent_queue_wait_policy BlockingPolicy>
class queue_wrapper<T, concurrent_queue_policy::locking_mpsc, BlockingPolicy>
{
  using queue_barrier = std::conditional_t<BlockingPolicy == concurrent_queue_wait_policy::condition_variable,
                                           cond_var_barrier,
                                           sleep_barrier>;

public:
  template <typename... Args>
  explicit queue_wrapper(size_t qsize, Args&&... args) : cap(qsize), barrier(std::forward<Args>(args)...)
  {
    queue.reserve(qsize);
    popped_items.reserve(qsize);
  }

  template <bool BlockOnFull, typename U>
  bool push(U&& elem) noexcept
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (BlockOnFull and queue.size() == queue.capacity()) {
      barrier.wait_push(lock, [this]() { return queue.size() < queue.capacity(); });
    }
    if (queue.size() < queue.capacity()) {
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

  size_t size() const
  {
    size_t                      count = popped_items.size() - popped_item_idx;
    std::lock_guard<std::mutex> lock(mutex);
    return count + queue.size();
  }

  bool empty() const
  {
    if (popped_item_idx < popped_items.size()) {
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
    if (popped_item_idx < popped_items.size()) {
      return &popped_items[popped_item_idx++];
    }
    popped_item_idx = 0;
    popped_items.clear();
    {
      std::unique_lock<std::mutex> lock(mutex);
      if (Blocking) {
        barrier.wait_pop(lock, [this]() { return !queue.empty(); });
      }
      if (queue.empty()) {
        return nullptr;
      }
      popped_items.swap(queue);
    }
    barrier.notify_pop();
    return &popped_items[popped_item_idx++];
  }

  size_t             popped_item_idx = 0;
  const size_t       cap;
  std::vector<T>     queue, popped_items;
  mutable std::mutex mutex;
  queue_barrier      barrier;
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
class concurrent_queue : public detail::queue_wrapper<T, Policy, BlockingPolicy>
{
  using base_type = detail::queue_wrapper<T, Policy, BlockingPolicy>;

public:
  using value_type                                       = T;
  static const concurrent_queue_policy      queue_policy = Policy;
  static const concurrent_queue_wait_policy wait_policy  = BlockingPolicy;

  using base_type::queue_wrapper;

  bool try_push(T&& elem) noexcept { return base_type::template push<false>(std::move(elem)); }

  bool try_push(const T& elem) noexcept { return base_type::template push<false>(elem); }

  bool push_blocking(T&& elem) noexcept { return base_type::template push<true>(std::move(elem)); }

  bool push_blocking(const T& elem) noexcept { return base_type::template push<true>(elem); }

  optional<T> try_pop() { return base_type::template pop<false>(); }

  optional<T> pop_blocking() { return base_type::template pop<true>(); }

  template <typename CallOnPop>
  bool try_pop(const CallOnPop& func)
  {
    return base_type::call_and_pop<false>(func);
  }

  template <typename CallOnPop>
  bool pop_blocking(const CallOnPop& func)
  {
    return base_type::call_and_pop<true>(func);
  }
};

} // namespace srsran