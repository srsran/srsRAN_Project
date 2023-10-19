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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"
#include <unordered_map>

namespace srsran {

template <bool Enabled>
class file_event_tracer;

namespace execution_config_helper {

namespace detail {

/// Parameters of the task executor, including name and decorators.
struct executor_common {
  /// Name of the executor.
  std::string name;
  /// Whether to log when task fails to be dispatched.
  bool report_on_failure = true;
  /// \brief Whether to make an executor synchronous. If true, the executor will be blocking, until the pushed task is
  /// fully executed. This will have a negative impact on performance, but can be useful for debugging.
  bool synchronous = false;
};

} // namespace detail

/// Parameters of a queue of tasks.
struct task_queue {
  /// \brief Queue policy to use for the task queue. E.g. SPSC, MPSC, MPMC, etc.
  concurrent_queue_policy policy;
  /// Task queue size.
  unsigned size;
};

/// Arguments for a single task worker creation.
struct single_worker {
  /// Parameters for the creation of an executor of a single worker execution context.
  using executor = detail::executor_common;

  /// Worker name.
  std::string name;
  /// Queue used by the task worker.
  task_queue queue;
  /// Executors associated with this execution context.
  std::vector<executor> executors;
  /// \brief Wait time in microseconds, when task queue has no pending tasks. If not set, a condition variable is
  /// used to wake up the worker when a new task is pushed.
  optional<std::chrono::microseconds> wait_sleep_time;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
  /// Non null in case tracing of the worker executors is enabled.
  file_event_tracer<true>* tracer = nullptr;
};

/// Arguments for a task worker pool creation.
struct worker_pool {
  /// Parameters for the creation of an executor of a single worker execution context.
  using executor = detail::executor_common;

  /// Worker Pool prefix name. Workers will be named as name#0, name#1, etc.
  std::string name;
  /// Number of workers in the pool.
  unsigned nof_workers;
  /// Queue used by the task worker.
  task_queue queue;
  /// Executors associated with this execution context.
  std::vector<executor> executors;
  /// \brief Wait time in microseconds, when task queue has no pending tasks. This value should be set when the queue
  /// policy is lockfree MPMC.
  optional<std::chrono::microseconds> sleep_time;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Array of CPU bitmasks to assign to each worker in the pool.
  std::vector<os_sched_affinity_bitmask> masks;
  /// Non null in case tracing of the worker executors is enabled.
  file_event_tracer<true>* tracer = nullptr;
};

using task_priority = enqueue_priority;

/// Arguments for the creation of a priority multiqueue worker.
struct priority_multiqueue_worker {
  /// Parameters for the creation of an executor of a single worker execution context.
  struct executor : public detail::executor_common {
    /// 0 is highest priority, -1 is second highest, etc. Must be a negative number.
    task_priority priority;

    executor(const std::string& name_,
             task_priority      priority_,
             bool               report_on_failure_ = true,
             bool               synchronous_       = false) :
      executor_common{name_, report_on_failure_, synchronous_}, priority(priority_)
    {
    }
  };

  /// Worker name.
  std::string name;
  /// \brief Queues of different priorities. The lower the index, the higher the priority.
  /// The size of the vector matches the number of instantiated queues.
  std::vector<task_queue> queues;
  /// \brief Wait time in microseconds, when task queue has no pending tasks.
  std::chrono::microseconds spin_sleep_time;
  /// Executors associated with this execution context.
  std::vector<executor> executors;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
  /// Non null in case tracing of the worker executors is enabled.
  file_event_tracer<true>* tracer = nullptr;
};

} // namespace execution_config_helper

/// General execution context to which tasks can be dispatched via task executors.
class task_execution_context
{
public:
  virtual ~task_execution_context() = default;

  /// Stop the execution context.
  virtual void stop() = 0;

  /// Name of the execution context.
  virtual std::string name() const = 0;

  /// Executors instantiated in this execution context.
  virtual std::vector<std::pair<std::string, task_executor*>> executors() const = 0;
};

/// Create a single worker execution context.
std::unique_ptr<task_execution_context> create_execution_context(const execution_config_helper::single_worker& params);

/// Create a worker pool execution context.
std::unique_ptr<task_execution_context> create_execution_context(const execution_config_helper::worker_pool& params);

/// Create a multi-priority worker execution context.
std::unique_ptr<task_execution_context>
create_execution_context(const execution_config_helper::priority_multiqueue_worker& params);

/// Repository of execution contexts and task executors.
class task_execution_manager
{
public:
  using executor_table = std::unordered_map<std::string, task_executor*>;

  /// Creates an empty task execution manager.
  task_execution_manager();

  /// Stops all execution contexts.
  void stop();

  /// Add new execution context to repository.
  SRSRAN_NODISCARD bool add_execution_context(std::unique_ptr<task_execution_context> ctxt);

  /// Returns a table of all executors stored in the repository.
  SRSRAN_NODISCARD const executor_table& executors() const { return executor_list; }

private:
  srslog::basic_logger& logger;

  std::unordered_map<std::string, std::unique_ptr<task_execution_context>> contexts;

  /// Container of executors instantiated for all execution contexts within this \c task_execution_manager.
  executor_table executor_list;
};

} // namespace srsran