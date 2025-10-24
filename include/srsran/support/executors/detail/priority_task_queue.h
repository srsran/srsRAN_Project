/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"
#include "srsran/adt/span.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace detail {

/// Convert enqueue priority into queue index in the priority_task_queue.
constexpr size_t enqueue_priority_to_queue_index(enqueue_priority prio, size_t nof_priority_levels)
{
  if (nof_priority_levels == 0) {
    return 0;
  }
  size_t queue_idx = std::numeric_limits<size_t>::max() - static_cast<size_t>(prio);
  return queue_idx < nof_priority_levels ? queue_idx : nof_priority_levels - 1;
}

/// Convert queue index in the priority_task_queue into enqueue priority.
constexpr enqueue_priority queue_index_to_enqueue_priority(size_t queue_idx, size_t nof_priority_levels)
{
  return static_cast<enqueue_priority>(std::numeric_limits<size_t>::max() - queue_idx);
}

/// \brief Priority of a task, used to specify which queue the priority_task_queue will use to dispatch the task. The
/// higher the priority, the lower its integer value representation.
using task_priority = enqueue_priority;

/// \brief Concurrent queue for any policy and wait policy.
///
/// This class performs type erasure of the concurrent_queue<> template.
class any_task_queue;
class any_task_consumer;

/// \brief Concurrent task queue, where the caller specifies the task priority while pushing it to the queue.
///
/// The prioritization is achieved via multiple queues. The pop functions will always start with the highest priority
/// queue until it is depleted, and then move to the second highest priority queue, and so on.
class priority_task_queue
{
public:
  using value_type = unique_task;

  /// Consumer interface to pop elements from the priority_task_queue.
  /// \remark The consumer is not thread-safe, so it should not be used concurrently.
  class consumer_type
  {
  public:
    explicit consumer_type(priority_task_queue& parent_);
    ~consumer_type();
    [[nodiscard]] bool   try_pop(unique_task& t);
    [[nodiscard]] size_t try_pop_bulk(span<unique_task> t);
    [[nodiscard]] bool   pop_blocking(unique_task& t);
    [[nodiscard]] bool   pop_blocking(unique_task& t, std::chrono::microseconds wait_time);

  private:
    priority_task_queue*                            parent;
    std::vector<std::unique_ptr<any_task_consumer>> consumers;
  };

  priority_task_queue(span<const concurrent_queue_params> queues, std::chrono::microseconds wait_if_empty);
  ~priority_task_queue();

  /// Interrupt all blocking popping/pushing operations for this queue.
  void request_stop();

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, block caller.
  void push_blocking(task_priority prio, unique_task task);

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, drop the task and
  /// return false.
  [[nodiscard]] bool try_push(task_priority prio, unique_task task);

  /// \brief Pop a pending task, considering its priority level. If the queues are empty, return false.
  [[nodiscard]] bool try_pop(unique_task& t);

  /// \brief Pop a batch of pending tasks, considering their priority level. If the queues are empty, return false.
  [[nodiscard]] size_t try_pop_bulk(span<unique_task> batch);

  /// \brief Pop a pending task, considering its priority level. If the queues are empty, this call blocks.
  /// If the queues are stopped, this function returns false.
  bool pop_blocking(unique_task& t);
  bool pop_blocking(unique_task& t, std::chrono::microseconds wait_time);

  [[nodiscard]] size_t queue_capacity(task_priority prio) const;

  size_t nof_priority_levels() const { return queues.size(); }

  /// Get number of pending tasks.
  size_t size() const;

  /// Creats a consumer for this queue.
  consumer_type create_consumer();

private:
  size_t get_queue_idx(task_priority prio) const
  {
    return detail::enqueue_priority_to_queue_index(prio, queues.size());
  }

  std::vector<std::unique_ptr<detail::any_task_queue>> queues;
  detail::queue_helper::sleep_wait_policy              policy;
};

} // namespace detail
} // namespace srsran
