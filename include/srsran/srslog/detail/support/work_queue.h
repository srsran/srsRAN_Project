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
#include "srsran/srslog/detail/support/backend_capacity.h"

namespace srslog {

namespace detail {

//: TODO: this is a temp work queue.

/// Thread safe generic data type work queue.
template <typename T, size_t capacity = SRSLOG_QUEUE_CAPACITY>
class work_queue
{
  rigtorp::MPMCQueue<T>   queue;
  static constexpr size_t threshold = capacity * 0.98;

public:
  work_queue() : queue(capacity) {}

  work_queue(const work_queue&)            = delete;
  work_queue& operator=(const work_queue&) = delete;

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(const T& value) { return queue.try_push(value); }

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(T&& value) { return queue.try_push(std::move(value)); }

  /// Extracts the top most element from the queue if it exists.
  /// Returns a pair with a bool indicating if the pop has been successful.
  std::pair<bool, T> try_pop()
  {
    T item;
    if (!queue.try_pop(item)) {
      return {false, T()};
    }
    return {true, std::move(item)};
  }

  /// Capacity of the queue.
  size_t get_capacity() const { return capacity; }

  /// Returns true when the queue is almost full, otherwise returns false.
  bool is_almost_full() const
  {
    auto s = queue.size();
    // size() may return a negative value for an empty queue.
    if (s < 0) {
      return false;
    }
    return static_cast<size_t>(s) > threshold;
  }
};

} // namespace detail

} // namespace srslog
