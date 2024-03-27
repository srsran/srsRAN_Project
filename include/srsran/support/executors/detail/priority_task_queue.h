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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace detail {

/// \brief Priority of a task, used to specify which queue the priority_task_queue will use to dispatch the task. The
/// higher the priority, the lower its integer value representation.
using task_priority = enqueue_priority;

class any_task_concurrent_queue
{
public:
  template <typename... Args>
  void emplace(const concurrent_queue_params& params, Args&&... other_params);

  void request_stop() { q->request_stop(); }
  bool push_blocking(unique_task t) { return q->push_blocking(std::move(t)); }
  bool try_push(unique_task t) { return q->try_push(std::move(t)); }
  bool try_pop(unique_task& t) { return q->try_pop(t); }

private:
  class base_queue
  {
  public:
    virtual ~base_queue()                        = default;
    virtual void request_stop()                  = 0;
    virtual bool push_blocking(unique_task task) = 0;
    virtual bool try_push(unique_task task)      = 0;
    virtual bool try_pop(unique_task& t)         = 0;
  };

  template <typename QueueType>
  class queue_impl;

  std::unique_ptr<base_queue> q;
};

/// List of task queues with different priority levels.
class priority_task_queue
{
public:
  priority_task_queue(span<const concurrent_queue_params> queues);

  /// Interrupt all the queues.
  void request_stop();

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, block caller.
  /// Return false, if the queue has been closed.
  bool push_blocking(task_priority prio, unique_task task);

  /// Dispatch a task to be run. If the internal queue associated with the priority level is full, drop the task and
  /// return false.
  SRSRAN_NODISCARD bool try_push(task_priority prio, unique_task task);

  /// Pop a pending task, considering its priority level.
  bool try_pop(unique_task& t);

private:
  size_t get_queue_idx(task_priority prio) const;

  std::vector<any_task_concurrent_queue> queues;
};

} // namespace detail
} // namespace srsran