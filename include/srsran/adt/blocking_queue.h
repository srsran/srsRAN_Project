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

#include "ring_buffer.h"
#include "srsran/adt/optional.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace srsran {

namespace detail {

/// \brief Base common class for definition of blocking queue data structures with the following features:
/// - it stores pushed/popped samples in an internal ring buffer
/// - provides blocking and non-blocking push/pop APIs
/// - thread-safe
/// \tparam RingBuffer underlying ring buffer data type (e.g. static_ring_buffer<T, N> or ring_buffer<T>)
/// \tparam PushingFunc function void(const T&) called while pushing an element to the queue
/// \tparam PoppingFunc function void(const T&) called while popping an element from the queue
template <typename RingBuffer, typename PushingFunc, typename PoppingFunc>
class base_blocking_queue
{
  using T = typename RingBuffer::value_type;

  using blocking_tag     = std::true_type;
  using non_blocking_tag = std::false_type;
  using wait_for_tag     = std::chrono::microseconds;
  using wait_until_tag   = std::chrono::steady_clock::time_point;

public:
  /// Possible outcomes for pop operation.
  enum class result { failed, success, timeout };

  /// \brief Creates a blocking_queue.
  /// \param on_push_func Callable to be called on every inserted element.
  /// \param on_pop_func Callable to be called on every popped element.
  template <typename... Args>
  base_blocking_queue(PushingFunc on_push_func, PoppingFunc on_pop_func, Args&&... args) :
    push_func(on_push_func), pop_func(on_pop_func), ring_buf(std::forward<Args>(args)...)
  {
  }
  base_blocking_queue(const base_blocking_queue&)            = delete;
  base_blocking_queue(base_blocking_queue&&)                 = delete;
  base_blocking_queue& operator=(const base_blocking_queue&) = delete;
  base_blocking_queue& operator=(base_blocking_queue&&)      = delete;

  /// \brief Sets queue state to "notify_stop" and awake any threads currently blocked waiting (either pushing or
  /// popping).
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
      ring_buf.clear();
    }
  }

  /// \brief Try to push new element to queue.
  /// \return If the queue is full or inactive, returns false. Otherwise, it returns true.
  bool try_push(const T& t) { return push_(t, non_blocking_tag{}); }

  /// \brief Try to push new r-value element to queue.
  /// \return If the queue is full or inactive, returns error type storing the element that failed to be pushed.
  /// Otherwise, it returns success type.
  error_type<T> try_push(T&& t) { return push_(std::move(t), non_blocking_tag{}); }

  /// \brief Tries to push all elements in a range into the queue.
  /// \return Returns number of inserted elements.
  template <typename It>
  unsigned try_push(It b, It e)
  {
    return push_(b, e, non_blocking_tag{});
  }

  /// \brief Tries to push all elements in a span into the queue.
  /// \return Returns number of inserted elements.
  unsigned try_push(span<T> t) { return try_push(t.begin(), t.end()); }

  /// \brief Pushes an element into the queue. If the queue is full, this call *blocks* waiting for another thread
  /// to pop an element from the queue or set the queue as inactive.
  /// \return Returns true if element was pushed. False, otherwise.
  bool push_blocking(const T& t) { return push_(t, blocking_tag{}); }

  /// \brief Pushes an r-value element into the queue. If the queue is full, this call *blocks* waiting for another
  /// thread to pop an element from the queue or set the queue as inactive.
  /// \return If the queue is inactive, returns error type storing the element that failed to be pushed.
  /// Otherwise, it returns success type.
  error_type<T> push_blocking(T&& t) { return push_(std::move(t), blocking_tag{}); }

  /// \brief Pushes all elements in a range into the queue. If the queue becomes full, this call *blocks* waiting
  /// for space to become available or the queue to become inactive.
  /// \return Returns number of inserted elements.
  template <typename It>
  unsigned push_blocking(It b, It e)
  {
    return push_(b, e, blocking_tag{});
  }

  /// \brief Pushes all elements in a span into the queue. If the queue becomes full, this call *blocks* waiting
  /// for space to become available or the queue to become inactive.
  /// \return Returns number of inserted elements.
  unsigned push_blocking(span<T> t) { return push_blocking(t.begin(), t.end()); }

  /// \brief Tries to pop one object from the queue.
  /// \return If queue is empty or inactive, returns false. Otherwise, returns true.
  bool try_pop(T& obj) { return pop_(obj, non_blocking_tag{}) == result::success; }

  /// \brief Tries to pop a range of elements from the queue.
  /// \return Number of popped elements.
  template <typename It>
  unsigned try_pop(It b, It e)
  {
    return pop_(b, e, non_blocking_tag{});
  }

  /// \brief Tries to pop a range of elements from the queue into a span.
  /// \return Number of popped elements.
  unsigned try_pop(span<T> t) { return try_pop(t.begin(), t.end()); }

  /// \brief Pops element from the queue. If queue is empty, this call *blocks* waiting for another thread to push an
  /// new element to the queue or that the queue is set to inactive.
  /// \return Popped element.
  T pop_blocking(bool* success = nullptr)
  {
    T      obj{};
    result ret = pop_(obj, blocking_tag{});
    if (success != nullptr) {
      *success = ret == result::success;
    }
    return obj;
  }

  /// \brief Pops a range of elements from the queue. If queue is empty, this call *blocks* waiting for another thread
  /// to push a new element to the queue or that the queue is set to inactive.
  /// \param[out] b First element that is to be popped from the queue.
  /// \param[out] e End of the range of elements to be popped from the queue.
  /// \return Number of popped elements. This number must be lower than std::distance(b, e).
  template <typename It>
  unsigned pop_blocking(It b, It e)
  {
    return pop_(b, e, blocking_tag{});
  }

  /// \brief Pops a span of elements from the queue. If queue is empty, this call *blocks* waiting for another thread
  /// to push a new element to the queue or that the queue is set to inactive.
  /// \param[out] t Span of elements popped from the queue. The size of this span defines the maximum number of elements
  /// to be popped.
  /// \return Number of actually popped elements.
  unsigned pop_blocking(span<T> t) { return pop_blocking(t.begin(), t.end(), blocking_tag{}); }

  /// \brief Pops an element from the queue. If the queue is empty, this call *blocks* waiting an element to be pushed
  /// to the queue or that the queue becomes inactive. This wait is bounded by \c duration.
  /// \param[out] obj Popped element.
  /// \param[in] duration Maximum duration to wait for a pushed element.
  /// \return True if element was successfully popped. False, otherwise.
  result pop_wait_for(T& obj, std::chrono::microseconds duration) { return pop_(obj, duration); }

  /// \brief Pops an element from the queue. If the queue is empty, this call *blocks* waiting an element to be pushed
  /// to the queue or that the queue becomes inactive. This wait is bounded by \c until.
  /// \param[out] obj Popped element.
  /// \param[in] until Maximum time point to wait for a pushed element.
  /// \return True if element was successfully popped. False, otherwise.
  result pop_wait_until(T& obj, std::chrono::steady_clock::time_point until) { return pop_(obj, until); }

  /// \brief Keeps popping and discarding elements until the \c stop_condition returns true.
  /// \param[in] stop_condition Callable with signature "bool(const T&)" that checks if the discarding can be halted.
  /// \return The element for which \c stop_condition returned true if it exists. Nullopt otherwise.
  template <typename StopCondition>
  optional<T> pop_and_discard_until(const StopCondition& stop_condition)
  {
    return pop_and_discard_until_(stop_condition);
  }

  /// \brief Clear all elements of the queue.
  void clear()
  {
    // TODO: optimize to a single lock.
    T obj;
    while (pop_(obj, non_blocking_tag{}) == result::success) {
    }
  }

  /// \brief Returns the number of elements stored in the queue.
  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return ring_buf.size();
  }

  /// \brief Checks whether the queue is empty.
  bool empty() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return ring_buf.empty();
  }

  /// \brief Checks whether the queue is full.
  bool full() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return ring_buf.full();
  }

  /// \brief Checks the maximum number of elements of the queue.
  size_t max_size() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return ring_buf.max_size();
  }

  /// \brief Checks whether the queue is inactive.
  bool is_stopped() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return not active;
  }

  /// \brief Apply provided callable on first position of the queue.
  template <typename F>
  bool try_call_on_front(const F& f)
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (not ring_buf.empty()) {
      f(ring_buf.top());
      return true;
    }
    return false;
  }

  /// \brief Apply provided callable on elements of the queue, until the callable returns false.
  template <typename F>
  bool apply_first(const F& func)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return ring_buf.apply_first(func);
  }

  PushingFunc push_func;
  PoppingFunc pop_func;

protected:
  bool                    active      = true;
  uint8_t                 nof_waiting = 0;
  mutable std::mutex      mutex;
  std::condition_variable cvar_empty, cvar_full;
  RingBuffer              ring_buf;

  ~base_blocking_queue() { stop(); }

  // Blocks producer while task queue is full.
  bool wait_push_possible(std::unique_lock<std::mutex>& lock, blocking_tag /**/)
  {
    if (active and ring_buf.full()) {
      nof_waiting++;
      while (ring_buf.full() and active) {
        cvar_full.wait(lock);
      }
      nof_waiting--;
    }
    return active;
  }

  // Returns true if there is space in the task queue.
  bool wait_push_possible(std::unique_lock<std::mutex>& /**/, non_blocking_tag /**/)
  {
    return active and not ring_buf.full();
  }

  template <typename BlockingMode>
  bool push_(const T& t, const BlockingMode& mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (wait_push_possible(lock, mode)) {
      push_func(t);
      ring_buf.push(t);
      cvar_empty.notify_one();
      return true;
    }
    return false;
  }

  template <typename BlockingMode>
  srsran::error_type<T> push_(T&& t, const BlockingMode& mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (wait_push_possible(lock, mode)) {
      push_func(t);
      ring_buf.push(std::move(t));
      cvar_empty.notify_one();
      return {};
    }
    return std::move(t);
  }

  template <typename It, typename BlockingMode>
  unsigned push_(It b, It e, const BlockingMode& mode)
  {
    unsigned count = 0;
    for (auto it = b; it != e;) {
      std::unique_lock<std::mutex> lock(mutex);
      if (not wait_push_possible(lock, mode)) {
        return count;
      }
      unsigned n = ring_buf.try_push(it, e);
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

  // Blocks until there is space in the queue.
  void wait_pop_possible_(std::unique_lock<std::mutex>& lock, const blocking_tag& /**/)
  {
    cvar_empty.wait(lock, [this]() { return not ring_buf.empty() or not active; });
  }

  // Blocks until there is space or timeout.
  void wait_pop_possible_(std::unique_lock<std::mutex>& lock, const wait_for_tag& duration)
  {
    cvar_empty.wait_for(lock, duration, [this]() { return not ring_buf.empty() or not active; });
  }

  // Blocks until there is space or timeout.
  void wait_pop_possible_(std::unique_lock<std::mutex>& lock, const wait_until_tag& until)
  {
    cvar_empty.wait_until(lock, until, [this]() { return not ring_buf.empty() or not active; });
  }

  // Blocks until there is space in the queue.
  bool wait_pop_possible(std::unique_lock<std::mutex>& /**/, const non_blocking_tag& /**/)
  {
    return active and not ring_buf.empty();
  }

  template <typename BlockingMode>
  bool wait_pop_possible(std::unique_lock<std::mutex>& lock, const BlockingMode& mode)
  {
    if (active and ring_buf.empty()) {
      nof_waiting++;
      wait_pop_possible_(lock, mode);
      nof_waiting--;
    }
    return active and not ring_buf.empty();
  }

  template <typename BlockingMode>
  result pop_(T& obj, const BlockingMode& mode)
  {
    std::unique_lock<std::mutex> lock(mutex);
    if (wait_pop_possible(lock, mode)) {
      obj = std::move(ring_buf.top());
      pop_func(obj);
      ring_buf.pop();
      cvar_full.notify_one();
      return result::success;
    }
    return active ? result::timeout : result::failed;
  }

  template <typename It, typename BlockingMode>
  unsigned pop_(It b, It e, const BlockingMode& mode)
  {
    unsigned count = 0;
    for (auto it = b; it != e;) {
      std::unique_lock<std::mutex> lock(mutex);
      if (not wait_pop_possible(lock, mode)) {
        break;
      }
      unsigned n = ring_buf.pop_into(it, e);
      if (n == 0) {
        break;
      }
      for (unsigned i = 0; i != n; ++i) {
        pop_func(*it);
        ++it;
      }
      count += n;
      lock.unlock();
      // notify all waiting producers because we may have emptied more than one slot of the queue.
      cvar_full.notify_all();
    }
    return count;
  }

  template <typename StopCondition>
  optional<T> pop_and_discard_until_(const StopCondition& cond)
  {
    optional<T>                  ret{};
    bool                         notify_needed = false;
    std::unique_lock<std::mutex> lock(mutex);
    while (wait_pop_possible(lock, non_blocking_tag{})) {
      pop_func(ring_buf.top());
      notify_needed = true;
      if (cond(ring_buf.top())) {
        ret = std::move(ring_buf.top());
        ring_buf.pop();
        break;
      }
      ring_buf.pop();
    }
    if (notify_needed) {
      cvar_full.notify_one();
    }
    return ret;
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
          typename PushingCallback = detail::noop_operation,
          typename PoppingCallback = detail::noop_operation>
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
          typename PushingCallback = detail::noop_operation,
          typename PoppingCallback = detail::noop_operation>
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

} // namespace srsran
