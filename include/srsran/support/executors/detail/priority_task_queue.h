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
#include "srsran/adt/span.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace detail {

static constexpr size_t enqueue_priority_to_queue_index(enqueue_priority prio, size_t nof_priority_levels)
{
  if (nof_priority_levels == 0) {
    return 0;
  }
  size_t queue_idx = std::numeric_limits<size_t>::max() - static_cast<size_t>(prio);
  return queue_idx < nof_priority_levels ? queue_idx : nof_priority_levels - 1;
}

static constexpr enqueue_priority queue_index_to_enqueue_priority(size_t queue_idx, size_t nof_priority_levels)
{
  return static_cast<enqueue_priority>(std::numeric_limits<size_t>::max() - queue_idx);
}

/// \brief Priority of a task, used to specify which queue the priority_task_queue will use to dispatch the task. The
/// higher the priority, the lower its integer value representation.
using task_priority = enqueue_priority;

/// \brief Concurrent queue for any policy and wait policy.
class any_task_concurrent_queue
{
public:
  template <typename... Args>
  void emplace(const concurrent_queue_params& params, Args&&... other_params);

  void   request_stop() { q->request_stop(); }
  bool   push_blocking(unique_task t) { return q->push_blocking(std::move(t)); }
  bool   try_push(unique_task t) { return q->try_push(std::move(t)); }
  bool   try_pop(unique_task& t) { return q->try_pop(t); }
  size_t size() const { return q->size(); }

  size_t capacity() const { return cap; }

private:
  class base_queue
  {
  public:
    virtual ~base_queue()                          = default;
    virtual void   request_stop()                  = 0;
    virtual bool   push_blocking(unique_task task) = 0;
    virtual bool   try_push(unique_task task)      = 0;
    virtual bool   try_pop(unique_task& t)         = 0;
    virtual size_t size() const                    = 0;
  };

  template <typename QueueType>
  class queue_impl;

  size_t cap = 0;

  std::unique_ptr<base_queue> q;
};

/// \brief Concurrent task queue, where the caller specifies the task priority while pushing it to the queue.
///
/// The prioritization is achieved via multiple queues. The pop functions will always start with the highest priority
/// queue until it is depleted, and then move to the second highest priority queue, and so on.
class priority_task_queue
{
public:
  priority_task_queue(span<const concurrent_queue_params> queues, std::chrono::microseconds wait_if_empty);

  /// Interrupt all blocking popping/pushing operations for this queue.
  void request_stop();

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, block caller.
  /// Return false, if the queue has been closed.
  bool push_blocking(task_priority prio, unique_task task);

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, drop the task and
  /// return false.
  SRSRAN_NODISCARD bool try_push(task_priority prio, unique_task task);

  /// \brief Pop a pending task, considering its priority level. If the queues are empty, return false.
  SRSRAN_NODISCARD bool try_pop(unique_task& t);

  /// \brief Pop a pending task, considering its priority level. If the queues are empty, this call blocks.
  /// If the queues are stopped, this function returns false.
  bool pop_blocking(unique_task& t);

  SRSRAN_NODISCARD size_t queue_capacity(task_priority prio) const { return queues[get_queue_idx(prio)].capacity(); }

  size_t nof_priority_levels() const { return queues.size(); }

  /// Get number of pending tasks.
  size_t size() const;

private:
  size_t get_queue_idx(task_priority prio) const
  {
    return detail::enqueue_priority_to_queue_index(prio, queues.size());
  }

  std::chrono::microseconds wait_on_empty;

  std::vector<any_task_concurrent_queue> queues;

  std::atomic<bool> running{true};
};

} // namespace detail
} // namespace srsran