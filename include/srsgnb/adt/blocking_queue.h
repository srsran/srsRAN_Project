/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ring_buffer.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace srsgnb {

namespace detail {

/// \brief Base common class for definition of blocking queue data structures with the following features:
/// - it stores pushed/popped samples in an internal circular buffer
/// - provides blocking and non-blocking push/pop APIs
/// - thread-safe
/// \tparam CircBuffer underlying circular buffer data type (e.g. static_circular_buffer<T, N> or
/// dyn_circular_buffer<T>)
/// \tparam PushingFunc function void(const T&) called while pushing an element to the queue
/// \tparam PoppingFunc function void(const T&) called while popping an element from the queue
template <typename CircBuffer, typename PushingFunc, typename PoppingFunc>
class base_blocking_queue
{
  using T = typename CircBuffer::value_type;

public:
  template <typename... Args>
  base_blocking_queue(PushingFunc push_func_, PoppingFunc pop_func_, Args&&... args) :
    push_func(push_func_), pop_func(pop_func_), circ_buffer(std::forward<Args>(args)...)
  {
  }
  base_blocking_queue(const base_blocking_queue&)            = delete;
  base_blocking_queue(base_blocking_queue&&)                 = delete;
  base_blocking_queue& operator=(const base_blocking_queue&) = delete;
  base_blocking_queue& operator=(base_blocking_queue&&)      = delete;

  void stop()
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (active) {
      active = false;
      if (nof_waiting > 0) {
        // Stop pending pushing/popping threads
        do {
          lock.unlock();
          cvar_empty.notify_all();
          cvar_full.notify_all();
          std::this_thread::yield();
          lock.lock();
        } while (nof_waiting > 0);
      }

      // Empty queue
      circ_buffer.clear();
    }
  }

  bool          try_push(const T& t) { return push_(t, false); }
  error_type<T> try_push(T&& t) { return push_(std::move(t), false); }
  template <typename It>
  unsigned try_push(It b, It e)
  {
    return push_(b, e, false);
  }
  unsigned      try_push(span<T> t) { return try_push(t.begin(), t.end()); }
  bool          push_blocking(const T& t) { return push_(t, true); }
  error_type<T> push_blocking(T&& t) { return push_(std::move(t), true); }
  template <typename It>
  unsigned push_blocking(It b, It e)
  {
    return push_(b, e, true);
  }
  unsigned push_blocking(span<T> t) { return push_blocking(t.begin(), t.end()); }
  bool     try_pop(T& obj) { return pop_(obj, false); }
  template <typename It>
  unsigned try_pop(It b, It e)
  {
    return pop_(b, e, false);
  }
  unsigned try_pop(span<T> t) { return try_pop(t.begin(), t.end()); }
  T        pop_blocking(bool* success = nullptr)
  {
    T    obj{};
    bool ret = pop_(obj, true);
    if (success != nullptr) {
      *success = ret;
    }
    return obj;
  }
  template <typename It>
  unsigned pop_blocking(It b, It e)
  {
    return pop_(b, e, true);
  }
  unsigned pop_blocking(span<T> t) { return pop_blocking(t.begin(), t.end()); }
  bool pop_wait_until(T& obj, const std::chrono::steady_clock::time_point& until) { return pop_(obj, true, &until); }
  void clear()
  {
    T obj;
    while (pop_(obj, false)) {
    }
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.size();
  }
  bool empty() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.empty();
  }
  bool full() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.full();
  }
  size_t max_size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.max_size();
  }
  bool is_stopped() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return not active;
  }
  template <typename F>
  bool try_call_on_front(const F& f)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not circ_buffer.empty()) {
      f(circ_buffer.top());
      return true;
    }
    return false;
  }

  template <typename F>
  bool apply_first(const F& func)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return circ_buffer.apply_first(func);
  }

  PushingFunc push_func;
  PoppingFunc pop_func;

protected:
  bool                    active      = true;
  uint8_t                 nof_waiting = 0;
  mutable std::mutex      mutex;
  std::condition_variable cvar_empty, cvar_full;
  CircBuffer              circ_buffer;

  ~base_blocking_queue() { stop(); }

  bool push_is_possible(std::unique_lock<std::mutex>& lock, bool block_mode)
  {
    if (not active) {
      return false;
    }
    if (circ_buffer.full()) {
      if (not block_mode) {
        return false;
      }
      nof_waiting++;
      while (circ_buffer.full() and active) {
        cvar_full.wait(lock);
      }
      nof_waiting--;
    }
    if (not active) {
      return false;
    }
    return true;
  }

  bool push_(const T& t, bool block_mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (push_is_possible(lock, block_mode)) {
      push_func(t);
      circ_buffer.push(t);
      // Note: I have read diverging opinions about notifying before or after the unlock. In this case,
      // it seems that TSAN complains if notify comes after.
      cvar_empty.notify_one();
      lock.unlock();
      return true;
    }
    return false;
  }
  srsgnb::error_type<T> push_(T&& t, bool block_mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (push_is_possible(lock, block_mode)) {
      push_func(t);
      circ_buffer.push(std::move(t));
      cvar_empty.notify_one();
      lock.unlock();
      return {};
    }
    return std::move(t);
  }
  template <typename It>
  unsigned push_(It b, It e, bool block_mode)
  {
    unsigned count = 0;
    for (auto it = b; it != e;) {
      std::unique_lock<std::mutex> lock(mutex);
      if (not push_is_possible(lock, block_mode)) {
        return count;
      }
      unsigned n = circ_buffer.try_push(it, e);
      if (n == 0) {
        break;
      }
      for (unsigned i = 0; i != n; ++i) {
        push_func(*it);
        ++it;
      }
      count += n;
      cvar_empty.notify_one();
    }
    return count;
  }

  bool pop_is_possible(std::unique_lock<std::mutex>&                lock,
                       bool                                         block,
                       const std::chrono::steady_clock::time_point* until = nullptr)
  {
    if (not active) {
      return false;
    }
    if (circ_buffer.empty()) {
      if (not block) {
        return false;
      }
      nof_waiting++;
      if (until == nullptr) {
        cvar_empty.wait(lock, [this]() { return not circ_buffer.empty() or not active; });
      } else {
        cvar_empty.wait_until(lock, *until, [this]() { return not circ_buffer.empty() or not active; });
      }
      nof_waiting--;
      if (circ_buffer.empty()) {
        // either queue got deactivated or there was a timeout
        return false;
      }
    }
    return true;
  }
  bool pop_(T& obj, bool block, const std::chrono::steady_clock::time_point* until = nullptr)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (pop_is_possible(lock, block, until)) {
      obj = std::move(circ_buffer.top());
      pop_func(obj);
      circ_buffer.pop();
      cvar_full.notify_one();
      lock.unlock();
      return true;
    }
    return false;
  }
  template <typename It>
  unsigned pop_(It b, It e, bool block, const std::chrono::steady_clock::time_point* until = nullptr)
  {
    unsigned count = 0;
    for (auto it = b; it != e;) {
      std::unique_lock<std::mutex> lock(mutex);
      if (not pop_is_possible(lock, block, until)) {
        break;
      }
      unsigned n = circ_buffer.pop_into(it, e);
      if (n == 0) {
        break;
      }
      for (unsigned i = 0; i != n; ++i) {
        pop_func(*it);
        ++it;
      }
      count += n;
      cvar_full.notify_one();
    }
    return count;
  }
};

} // namespace detail

/// Blocking queue with buffer storage represented via a std::vector<T>. Features:
/// - Blocking push/pop API via push_blocking(...) and pop_blocking(...) methods
/// - Non-blocking push/pop API via try_push(...) and try_pop(...) methods
/// - Size can be defined at runtime.
/// - thread-safe
/// \tparam T value type stored by buffer
/// \tparam PushingCallback function void(const T&) called while pushing an element to the queue
/// \tparam PoppingCallback function void(const T&) called while popping an element from the queue
template <typename T,
          typename PushingCallback = detail::noop_operator,
          typename PoppingCallback = detail::noop_operator>
class blocking_queue : public detail::base_blocking_queue<ring_buffer<T, true>, PushingCallback, PoppingCallback>
{
  using super_type = detail::base_blocking_queue<ring_buffer<T, true>, PushingCallback, PoppingCallback>;

public:
  explicit blocking_queue(size_t size, PushingCallback push_callback = {}, PoppingCallback pop_callback = {}) :
    super_type(push_callback, pop_callback, size)
  {
  }
};

/// Blocking queue with fixed, embedded buffer storage via a std::array<T, N>.
/// - Blocking push/pop API via push_blocking(...) and pop_blocking(...) methods
/// - Non-blocking push/pop API via try_push(...) and try_pop(...) methods
/// - Only one initial allocation for the std::array<T, N>
/// - thread-safe
/// \tparam T value type stored by buffer
/// \tparam N size of queue
/// \tparam PushingCallback function void(const T&) called while pushing an element to the queue
/// \tparam PoppingCallback function void(const T&) called while popping an element from the queue
template <typename T,
          size_t N,
          typename PushingCallback = detail::noop_operator,
          typename PoppingCallback = detail::noop_operator>
class static_blocking_queue
  : public detail::base_blocking_queue<static_ring_buffer<T, N>, PushingCallback, PoppingCallback>
{
  using base_t = detail::base_blocking_queue<static_ring_buffer<T, N>, PushingCallback, PoppingCallback>;

public:
  explicit static_blocking_queue(PushingCallback push_callback = {}, PoppingCallback pop_callback = {}) :
    base_t(push_callback, pop_callback)
  {
  }
};

} // namespace srsgnb