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
  template <typename E = ExecType>
  task_fork_limiter(E&&      exec,
                    unsigned max_forks_,
                    unsigned qsize_,
                    unsigned max_batch_ = std::numeric_limits<unsigned>::max()) :
    max_forks(max_forks_), max_batch(max_batch_), out_exec(std::forward<E>(exec)), queue(qsize_)
  {
    report_fatal_error_if_not(max_forks > 0, "Fork limit executor must have a positive max_forks value.");
    report_fatal_error_if_not(max_batch > 0, "Fork limit executor must have a positive max_batch value.");
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
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute([this]() { process_enqueued_tasks(); });
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { process_enqueued_tasks(); });
    }
    if (not dispatch_successful) {
      // Failed to dispatch the task, handle it accordingly.
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  void process_enqueued_tasks()
  {
    active_forks.fetch_add(1, std::memory_order_acq_rel);
    unique_task task;

    unsigned max_pops  = max_batch;
    unsigned jobs_left = job_count.load(std::memory_order_acquire);
    while (jobs_left > 0) {
      if (not pop_task(task)) {
        break;
      }

      // Execute the task.
      task();

      // Decrement the job count after successful pop and task execution.
      jobs_left = job_count.fetch_sub(1, std::memory_order_acq_rel) - 1;

      // Check if we should yield back control to the worker.
      if (--max_pops == 0 and jobs_left > 0) {
        defer_remaining_tasks();
        break;
      }
    }

    active_forks.fetch_sub(1, std::memory_order_acq_rel);
  }

  bool defer_remaining_tasks()
  {
    // Dispatch batch dequeue job.
    bool dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { process_enqueued_tasks(); });
    if (not dispatch_successful) {
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  void handle_failed_dispatch()
  {
    srslog::fetch_basic_logger("ALL").error(
        "task_fork_limiter: Couldn't dispatch forked task. Cause: out_exec queue is full");

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
    if (not queue.try_pop(task)) {
      // Failed to pop a task from the queue.

      // Check if this is the last fork alive.
      unsigned active_forks_count = active_forks.load(std::memory_order_acquire);
      if (active_forks_count > 1) {
        // There are other forks alive.
        return false;
      }

      // Re-read pending job count.
      unsigned jobs_left = job_count.load(std::memory_order_relaxed);
      if (jobs_left == 0) {
        // No more jobs left to process. Probably, another fork did the pop operation concurrently and depleted the
        // queue.
        count_pop_fails.store(0, std::memory_order_relaxed);
        return false;
      }

      // There are still jobs left in the queue and this is the last fork alive. This can happen because the pop
      // operation fails spuriously for some queue implementations.
      auto count_copy = count_pop_fails.fetch_add(1, std::memory_order_relaxed) + 1;
      if (count_copy >= 1000) {
        count_pop_fails.store(0, std::memory_order_relaxed);
        srslog::fetch_basic_logger("ALL").warning(
            "task_fork_limiter: Too many pop failures detected. There was likely a lost job in the queue");
      }
      defer_remaining_tasks();
      return false;
    }
    return true;
  }

  /// Maximum number of concurrent forks allowed for this executor.
  const unsigned max_forks;
  /// Maximum number of tasks that can be processed by one fork, before it yields control back to the worker.
  const unsigned max_batch;

  // Executor to which tasks are dispatched in serialized manner.
  ExecType out_exec;

  // Queue of pending tasks.
  concurrent_queue<unique_task, QueuePolicy, concurrent_queue_wait_policy::non_blocking> queue;

  /// Current number of active forks, used to limit the number of concurrent tasks.
  std::atomic<unsigned> job_count{0};

  /// Counter of the number of active forks.
  std::atomic<unsigned> active_forks{0};

  /// Counter of the number of unexpected pop failures.
  std::atomic<unsigned> count_pop_fails{0};
};

/// \brief Create an adaptor of the task_executor that limits the number of concurrent tasks to \c max_fork_size.
/// \param[in] out_exec Executor to which tasks are ultimately dispatched.
/// \param[in] max_fork_size Maximum number of concurrent tasks that can run concurrently in the wrapped executor.
/// \param[in] qsize Size of the internal queue
template <concurrent_queue_policy QueuePolicy, typename OutExec = task_executor*>
std::unique_ptr<task_executor> make_task_fork_limiter_ptr(OutExec&& out_exec, unsigned max_fork_size, unsigned qsize)
{
  return std::make_unique<task_fork_limiter<OutExec, QueuePolicy>>(
      std::forward<OutExec>(out_exec), max_fork_size, qsize);
}

} // namespace srsran
