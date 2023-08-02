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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/variant.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/unique_thread.h"
#include <unordered_map>

namespace srsran {

struct task_executor_description {
  std::string name;
};

/// Configuration specifying the type and parameters of a execution context and its respective task executors.
struct execution_context_description {
  struct single {
    /// \brief Queue policy to use for the task queue. E.g. SPSC, MPSC, etc.
    concurrent_queue_policy queue_policy;
    /// \brief Wait time in microseconds, when task queue has no pending tasks. If not set, a condition variable is
    /// used to wake up the worker when a new task is pushed.
    optional<std::chrono::microseconds> wait_sleep_time;
  };
  struct pool {
    unsigned nof_workers;
  };

  /// Name of the execution context.
  std::string name;
  /// task queue size.
  unsigned                    queue_size;
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
  /// Arguments specific to a given execution context / worker type.
  variant<single, pool> type;
  /// Executors associated with this execution context.
  std::vector<task_executor_description> executors;
};

struct task_worker_manager_config {
  std::vector<execution_context_description> execution_contexts;
};

/// General execution context to which tasks can be dispatched via task executors.
class task_execution_context
{
public:
  virtual ~task_execution_context() = default;

  /// Stop the execution context.
  virtual void stop() = 0;

  /// Adds a new task executor to the task execution context.
  virtual task_executor* add_executor(const task_executor_description& desc) = 0;
};

/// Repository of execution contexts and task executors.
class task_execution_manager
{
public:
  /// Creates a task worker manager given a configuration. Returns nullptr if the configuration is invalid.
  static std::unique_ptr<task_execution_manager> create(const task_worker_manager_config& config);

  /// Stops all execution contexts.
  void stop();

  /// Table of all executors stored in the repository.
  SRSRAN_FORCE_INLINE const std::unordered_map<std::string, task_executor*>& executors() const { return executor_list; }

private:
  task_execution_manager() = default;

  /// Add new execution context to repository.
  SRSRAN_NODISCARD bool add_execution_context(const std::string& name, std::unique_ptr<task_execution_context> mng);

  /// Add new executor to an existing execution context of the repository.
  SRSRAN_NODISCARD bool add_executor(const std::string& context_name, const task_executor_description& exec_desc);

  std::unordered_map<std::string, std::unique_ptr<task_execution_context>> contexts;
  std::unordered_map<std::string, task_executor*>                          executor_list;
};

/// Creates a task worker manager given a configuration.
task_execution_manager create_task_execution_manager(const task_worker_manager_config& config);

} // namespace srsran