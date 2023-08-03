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

template <bool Enabled>
class file_event_tracer;

/// Parameters of the task executor, including name and decorators.
struct task_executor_description {
  /// Name of the executor.
  std::string name;
  /// \brief Whether to make an executor synchronous. If true, the executor will be blocking, until the pushed task is
  /// fully executed. This will have a negative impact on performance, but can be useful for debugging.
  bool synchronous = false;
  /// In case tracing of the executor is enabled, the tracer is stored here.
  file_event_tracer<true>* tracer = nullptr;
};

/// Configuration specifying the type and parameters of a execution context and its respective task executors.
struct execution_context_description {
  /// Arguments used to instantiate a task queue.
  struct task_queue {
    /// \brief Queue policy to use for the task queue. E.g. SPSC, MPSC, etc.
    concurrent_queue_policy policy;
    /// Task queue size.
    unsigned size;
  };
  /// Arguments for a single task worker creation.
  struct single {
    task_queue queue;
    /// \brief Wait time in microseconds, when task queue has no pending tasks. If not set, a condition variable is
    /// used to wake up the worker when a new task is pushed.
    optional<std::chrono::microseconds> wait_sleep_time;
  };
  /// Arguments for a task worker pool creation.
  struct pool {
    /// Queue used by the worker pool.
    task_queue queue;
    /// Number of workers in the pool.
    unsigned nof_workers;
  };
  /// Arguments for the creation of a priority multiqueue worker.
  struct priority_multiqueue {
    /// \brief Queues of different priorities. The lower the index, the higher the priority.
    /// The size of the vector matches the number of instantiated queues.
    std::vector<task_queue> queues;
    /// \brief Wait time in microseconds, when task queue has no pending tasks.
    std::chrono::microseconds spin_sleep_time;
  };

  /// Name of the execution context.
  std::string name;
  /// OS priority of the worker thread.
  os_thread_realtime_priority prio = os_thread_realtime_priority::no_realtime();
  /// Bit mask to set worker cpu affinity.
  os_sched_affinity_bitmask mask = {};
  /// Arguments specific to a given execution context / worker type.
  variant<single, pool, priority_multiqueue> type;
  /// Executors associated with this execution context.
  std::vector<task_executor_description> executors;
};

/// General execution context to which tasks can be dispatched via task executors.
class task_execution_context
{
public:
  virtual ~task_execution_context() = default;

  /// Stop the execution context.
  virtual void stop() = 0;

  /// Adds a new task executor to the task execution context.
  virtual std::unique_ptr<task_executor> create_executor(const task_executor_description& desc) = 0;
};

/// Repository of execution contexts and task executors.
class task_execution_manager
{
public:
  using executor_table = std::unordered_map<std::string, std::unique_ptr<task_executor>>;

  /// Creates an empty task execution manager.
  task_execution_manager();

  /// Stops all execution contexts.
  void stop();

  /// Add new execution context to repository.
  SRSRAN_NODISCARD bool add_execution_context(const execution_context_description& desc);

  /// Table of all executors stored in the repository.
  SRSRAN_FORCE_INLINE SRSRAN_NODISCARD const executor_table& executors() const { return executor_list; }

private:
  bool store_execution_context(const std::string& name, std::unique_ptr<task_execution_context> mng);

  bool add_executor(const std::string& context_name, const task_executor_description& exec_desc);

  srslog::basic_logger& logger;

  std::unordered_map<std::string, std::unique_ptr<task_execution_context>> contexts;

  /// Container of executors instantiated for all execution contexts within this \c task_execution_manager.
  executor_table executor_list;
};

} // namespace srsran