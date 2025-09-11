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

#include "srsran/adt/detail/concurrent_queue_params.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/unique_thread.h"
#include <unordered_map>

namespace srsran {

template <bool Enabled>
class file_event_tracer;

namespace execution_config_helper {

using task_priority = enqueue_priority;

/// Parameters of a queue of tasks.
struct task_queue {
  /// Name attributed to this task queue.
  std::string name;
  /// Queueing policy used by this task queue.
  concurrent_queue_policy policy;
  /// Size of the queue used.
  unsigned size;
  /// Number of pre-reserved producers in the case of the moodycamel lockfree MPMC queue.
  unsigned nof_prereserved_producers = cpu_architecture_info::get().get_host_total_nof_cpus();
};

/// Arguments for a single task worker creation.
struct single_worker {
  /// Worker name.
  std::string name;
  /// Queue used by the task worker.
  task_queue queue;
  /// \brief Wait time in microseconds, when task queue has no pending tasks. If not set, a condition variable is
  /// used to wake up the worker when a new task is pushed.
  std::optional<std::chrono::microseconds> wait_sleep_time;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
};

/// Arguments for a task worker pool creation.
struct worker_pool {
  /// Worker Pool prefix name. Workers will be named as name#0, name#1, etc.
  std::string name;
  /// Number of workers in the pool.
  unsigned nof_workers;
  /// Queue(s) used by the task worker. The lower the index, the higher the priority.
  std::vector<task_queue> queues;
  /// \brief Wait time in microseconds, when task queue has no pending tasks.
  std::chrono::microseconds sleep_time;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Array of CPU bitmasks to assign to each worker in the pool.
  std::vector<os_sched_affinity_bitmask> masks;
};

/// Arguments for the creation of a priority multiqueue worker.
struct priority_multiqueue_worker {
  /// Worker name.
  std::string name;
  /// \brief Queues of different priorities. The lower the index, the higher the priority.
  /// The size of the vector matches the number of instantiated queues.
  std::vector<task_queue> queues;
  /// \brief Wait time in microseconds, when task queue has no pending tasks.
  std::chrono::microseconds spin_sleep_time;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
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
  [[nodiscard]] bool add_execution_context(std::unique_ptr<task_execution_context> ctxt);

  /// Returns a table of all executors stored in the repository.
  [[nodiscard]] const executor_table& executors() const { return executor_list; }

private:
  srslog::basic_logger& logger;

  std::unordered_map<std::string, std::unique_ptr<task_execution_context>> contexts;

  /// Container of executors instantiated for all execution contexts within this \c task_execution_manager.
  executor_table executor_list;
};

} // namespace srsran
