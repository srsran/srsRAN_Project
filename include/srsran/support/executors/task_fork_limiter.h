/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/detail/concurrent_queue_params.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
template <typename ExecType, concurrent_queue_policy QueuePolicy>
class task_fork_limiter final : public task_executor
{
public:
  template <typename E>
  task_fork_limiter(E&& exec, unsigned max_forks_, unsigned qsize_) :
    max_forks(max_forks_), out_exec(std::forward<E>(exec)), queue(qsize_)
  {
    report_fatal_error_if_not(max_forks > 0, "Fork limit executor must have a positive max_forks value.");
  }

  [[nodiscard]] bool execute(unique_task task) override { return dispatch<true>(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override { return dispatch<false>(std::move(task)); }

private:
  template <bool Execute>
  bool dispatch(unique_task task)
  {
    // Safe task to process in internal queue.
    if (not queue.try_push(std::move(task))) {
      return false;
    }

    unsigned prev_forks         = job_count.fetch_add(1, std::memory_order_acq_rel);
    bool     permission_to_fork = prev_forks + 1 <= max_forks;

    if (not permission_to_fork) {
      // Too many concurrent tasks are already dispatched. No need to dispatch this task.
      return true;
    }

    bool dispatch_successful = false;
    if constexpr (Execute) {
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute([this]() { handle_enqueued_task(); });
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { handle_enqueued_task(); });
    }
    if (not dispatch_successful) {
      // Failed to dispatch the task, handle it accordingly.
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  void handle_enqueued_task()
  {
    unique_task task;

    unsigned jobs_left = job_count.load(std::memory_order_acquire);
    while (jobs_left > 0) {
      if (not pop_task(task)) {
        break;
      }

      // Execute the task.
      task();

      // Decrement the job count after successful pop and task execute.
      jobs_left = job_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }
  }

  void handle_failed_dispatch()
  {
    srslog::fetch_basic_logger("ALL").error("Couldn't dispatch forked task. Cause: out_exec queue is full");

    // If we failed to dispatch the task, we need to revert the increment of job count.
    // TODO: Improve the failed dispatch handling. For instance, we could let it not fail if there is at least a forked
    // task already running.
    unsigned jobs_left = job_count.load(std::memory_order_acquire);
    while (jobs_left > 0) {
      unique_task dropped_task;
      if (not pop_task(dropped_task)) {
        break;
      }
      jobs_left = job_count.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }
  }

  bool pop_task(unique_task& task)
  {
    static constexpr unsigned max_attempts = 100;
    unsigned                  nof_attempts = 0;
    for (; nof_attempts < max_attempts and not queue.try_pop(task); ++nof_attempts) {
      // Failed to pop a task from the queue.

      // Re-read pending job count.
      unsigned jobs_left = job_count.load(std::memory_order_acquire);
      if (jobs_left > 0) {
        // This is likely a spurious pop failure (some MPMC queue implementations have this issue). We will retry the
        // pop.
        continue;
      }

      // No more tasks to process.
      // Note: This is expected to happen even when jobs_left > 0, when there are more than one concurrent consumer.
      return false;
    }

    if (nof_attempts >= max_attempts) {
      // Unexpected failure to pop enqueued tasks. Possible reason: Are you using an SPSC queue with multiple
      // producers?
      srslog::fetch_basic_logger("ALL").error("Couldn't pop pending tasks.");
      return false;
    }
    return true;
  }

  /// Maximum number of concurrent forks allowed for this executor.
  const unsigned max_forks;

  // Executor to which tasks are dispatched in serialized manner.
  ExecType out_exec;

  // Queue of pending tasks.
  concurrent_queue<unique_task, QueuePolicy, concurrent_queue_wait_policy::non_blocking> queue;

  /// Current number of active forks, used to limit the number of concurrent tasks.
  std::atomic<int> job_count{0};
};

/// \brief Create an adaptor of the task_executor that limits the number of concurrent tasks to \c max_fork_size.
/// \param[in] out_exec Executor to which tasks are ultimately dispatched.
/// \param[in] max_fork_size Maximum number of concurrent tasks that can run concurrently in the wrapped executor.
/// \param[in] qparams Parameters of the queue used to store tasks that are waiting to be executed.
template <typename OutExec = task_executor*>
std::unique_ptr<task_executor>
make_task_fork_limiter_ptr(OutExec&& out_exec, unsigned max_fork_size, const concurrent_queue_params& qparams)
{
  switch (qparams.policy) {
    case concurrent_queue_policy::lockfree_mpmc:
      return std::make_unique<task_fork_limiter<OutExec, concurrent_queue_policy::lockfree_mpmc>>(
          std::forward<OutExec>(out_exec), max_fork_size, qparams.size);
    case concurrent_queue_policy::lockfree_spsc:
      report_fatal_error("Invalid queue policy for fork_limit_executor: lockfree_spsc");
    case concurrent_queue_policy::locking_mpmc:
      return std::make_unique<task_fork_limiter<OutExec, concurrent_queue_policy::locking_mpmc>>(
          std::forward<OutExec>(out_exec), max_fork_size, qparams.size);
    case concurrent_queue_policy::locking_mpsc:
      report_fatal_error("Invalid queue policy for fork_limit_executor: lockfree_mpsc");
    case concurrent_queue_policy::moodycamel_lockfree_mpmc:
      return std::make_unique<task_fork_limiter<OutExec, concurrent_queue_policy::moodycamel_lockfree_mpmc>>(
          std::forward<OutExec>(out_exec), max_fork_size, qparams.size);
    default:
      break;
  }
  report_fatal_error("Unsupported queue policy.");
  return nullptr;
}

} // namespace srsran
