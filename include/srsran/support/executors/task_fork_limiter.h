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
#include "srsran/adt/moodycamel_mpmc_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/cpu_architecture_info.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
template <typename ExecType>
class task_fork_limiter final : public task_executor
{
public:
  template <typename E = ExecType>
  task_fork_limiter(E&&      exec,
                    unsigned max_forks_,
                    unsigned qsize_,
                    unsigned max_batch_ = std::numeric_limits<unsigned>::max()) :
    max_forks(max_forks_),
    max_batch(max_batch_),
    out_exec(std::forward<E>(exec)),
    queue(qsize_, cpu_architecture_info::get().get_host_nof_available_cpus())
  {
    report_fatal_error_if_not(max_forks > 0, "Fork limit executor must have a positive max_forks value.");
    report_fatal_error_if_not(max_batch > 0, "Fork limit executor must have a positive max_batch value.");
  }

  [[nodiscard]] bool execute(unique_task task) override { return dispatch<true>(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override { return dispatch<false>(std::move(task)); }

private:
  /// Value representative of one single fork in the state.
  static constexpr uint64_t one_fork_mask = static_cast<uint64_t>(1U) << 32U;

  template <bool Execute>
  bool dispatch(unique_task task)
  {
    // Save task to process in internal queue.
    if (not queue.try_push(std::move(task))) {
      return false;
    }

    // Increment the number of jobs in the queue and check if we can fork a new task.
    bool permission_to_fork = job_enqueued();
    if (not permission_to_fork) {
      // Too many concurrent fork branches are already active. No need to dispatch a new fork task.
      return true;
    }

    bool dispatch_successful = false;
    if constexpr (Execute) {
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute([this]() { handle_fork_tasks(); });
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { handle_fork_tasks(); });
    }
    if (not dispatch_successful) {
      // Failed to dispatch the task, handle it accordingly.
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  void handle_fork_tasks()
  {
    unique_task task;

    unsigned max_pops = max_batch;
    while (fork_has_tasks()) {
      if (not pop_task(task)) {
        // We were unable to pop a task, but there are still jobs in the queue. We will defer the remaining tasks.
        defer_remaining_tasks();
        return;
      }

      // Execute the task.
      task();

      // Check if we should yield back control to the worker because we reached the batch limit.
      if (--max_pops == 0) {
        defer_remaining_tasks();
        return;
      }
    }
  }

  bool defer_remaining_tasks()
  {
    // Dispatch batch dequeue job.
    bool dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { handle_fork_tasks(); });
    if (not dispatch_successful) {
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  void handle_failed_dispatch()
  {
    logger.error("task_fork_limiter: Couldn't dispatch forked task. Cause: out_exec queue is full");

    // If we failed to dispatch the task, we need to revert the increment of active forks.
    state.fetch_sub(one_fork_mask, std::memory_order_acq_rel);
  }

  bool pop_task(unique_task& task)
  {
    static constexpr unsigned max_pop_failures = 10;
    static constexpr unsigned max_attempts     = 5;

    unsigned attempts_count = 0;
    for (; attempts_count < max_attempts and not queue.try_pop(task); ++attempts_count) {
    }

    if (attempts_count < max_attempts) {
      // Job successfully dequeued.
      return true;
    }

    // There are still jobs left in the queue but we cannot dequeue them at the moment. We will re-try later.
    // Note: This can happen because the pop operation fails spuriously for some queue implementations.
    // Increment the state to indicate that we failed to pop a job.
    state.fetch_add(1U, std::memory_order_acq_rel);
    // Increment the pop failure counter.
    auto count_copy = count_pop_fails.fetch_add(1, std::memory_order_relaxed) + 1;
    if (count_copy >= max_pop_failures) {
      count_pop_fails.store(0, std::memory_order_relaxed);
      logger.warning("task_fork_limiter: Too many pop failures detected. There was likely a lost job in the queue");
    }
    return false;
  }

  /// Called to fetch the number of jobs stored in the queue.
  uint32_t jobs_in_queue(uint64_t state_cpy) const { return state_cpy & 0xffffffffU; }

  /// Called to fetch the number of active forks.
  uint32_t active_forks(uint64_t state_cpy) const { return (state_cpy >> 32U) & 0xffffffffU; }

  /// Called when a new job is enqueued.
  /// \return True if a new fork was created, false otherwise.
  bool job_enqueued()
  {
    auto prev = state.load(std::memory_order_relaxed);
    while (true) {
      // Increment the job count.
      uint64_t inc_val = 1U;
      if (active_forks(prev) < max_forks) {
        // We can also increment the number of active forks.
        inc_val += one_fork_mask;
      }
      if (state.compare_exchange_weak(prev, prev + inc_val, std::memory_order_acq_rel)) {
        // The state was successfully updated. Return true if a new fork was created.
        return inc_val > 1U;
      }
    }
    return false;
  }

  /// Called when a job runs to completion.
  bool fork_has_tasks()
  {
    auto prev = state.load(std::memory_order_acquire);
    while (true) {
      uint64_t dec_val = 0U;
      if (jobs_in_queue(prev) > 0) {
        // There are still jobs in the queue.
        dec_val = 1U;
      } else {
        // There are no jobs. We will try to deactivate the fork.
        dec_val = one_fork_mask;
      }
      if (state.compare_exchange_weak(prev, prev - dec_val, std::memory_order_acq_rel)) {
        return dec_val == 1U;
      }
    }
    count_pop_fails.store(0, std::memory_order_relaxed);
    return false;
  }

  /// Maximum number of concurrent forks allowed for this executor.
  const unsigned max_forks;
  /// Maximum number of tasks that can be processed by one fork, before it yields control back to the worker.
  const unsigned max_batch;

  // Executor to which tasks are dispatched in serialized manner.
  ExecType out_exec;

  /// Queue of pending tasks.
  concurrent_queue<unique_task,
                   concurrent_queue_policy::moodycamel_lockfree_mpmc,
                   concurrent_queue_wait_policy::non_blocking>
      queue;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");

  /// \brief Current state of the task fork limiter. It contains two parts: (i) 32 LSBs for the jobs stored in the
  /// queue, and (ii) 32 MSBs for the number of active forks.
  std::atomic<uint64_t> state{0};

  /// Counter of the number of unexpected pop failures.
  std::atomic<unsigned> count_pop_fails{0};
};

/// \brief Create an adaptor of the task_executor that limits the number of concurrent tasks to \c max_fork_size.
/// \param[in] out_exec Executor to which tasks are ultimately dispatched.
/// \param[in] max_fork_size Maximum number of concurrent tasks that can run concurrently in the wrapped executor.
/// \param[in] qsize Size of the internal queue
template <typename OutExec = task_executor*>
std::unique_ptr<task_executor> make_task_fork_limiter_ptr(OutExec&& out_exec, unsigned max_fork_size, unsigned qsize)
{
  return std::make_unique<task_fork_limiter<OutExec>>(std::forward<OutExec>(out_exec), max_fork_size, qsize);
}

} // namespace srsran
