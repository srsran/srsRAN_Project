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
#include "srsran/adt/moodycamel_mpmc_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

template <typename ExecType>
class task_fork_limiter;

/// Executor of a task_fork_limiter with a specific priority level.
template <typename ExecType>
class task_fork_limiter_executor final : public task_executor
{
public:
  task_fork_limiter_executor(task_fork_limiter<ExecType>& fork_limiter_, enqueue_priority prio_) :
    fork_limiter(&fork_limiter_), prio(prio_)
  {
  }

  [[nodiscard]] bool execute(unique_task task) override { return fork_limiter->execute(prio, std::move(task)); }
  [[nodiscard]] bool defer(unique_task task) override { return fork_limiter->defer(prio, std::move(task)); }

private:
  task_fork_limiter<ExecType>* fork_limiter = nullptr;
  enqueue_priority             prio;
};

template <typename ExecType>
class task_fork_limiter final : public task_executor
{
public:
  using executor_type = task_fork_limiter_executor<ExecType>;

  template <typename E = ExecType>
  task_fork_limiter(E&&                  exec,
                    unsigned             max_forks_,
                    span<const unsigned> qsizes_,
                    unsigned             max_batch_ = std::numeric_limits<unsigned>::max()) :
    max_forks(max_forks_), max_batch(max_batch_), out_exec(std::forward<E>(exec))
  {
    report_fatal_error_if_not(max_forks > 0, "Fork limit executor must have a positive max_forks value.");
    report_fatal_error_if_not(max_batch > 0, "Fork limit executor must have a positive max_batch value.");
    report_fatal_error_if_not(not qsizes_.empty(), "Fork limit executor must have at least one queue.");
    queues.reserve(qsizes_.size());
    exec_list.reserve(qsizes_.size());
    for (const auto& qsize : qsizes_) {
      report_fatal_error_if_not(qsize > 0, "Fork limit executor must have a positive queue size.");
      queues.emplace_back(qsize, std::thread::hardware_concurrency());
      enqueue_priority prio = enqueue_priority::max - exec_list.size();
      exec_list.emplace_back(*this, prio);
    }
  }
  template <typename E = ExecType>
  task_fork_limiter(E&&      exec,
                    unsigned max_forks_,
                    unsigned qsize_,
                    unsigned max_batch_ = std::numeric_limits<unsigned>::max()) :
    task_fork_limiter(std::forward<E>(exec), max_forks_, std::array<unsigned, 1>{qsize_}, max_batch_)
  {
  }

  [[nodiscard]] bool execute(enqueue_priority prio, unique_task task) { return dispatch<true>(prio, std::move(task)); }
  [[nodiscard]] bool defer(enqueue_priority prio, unique_task task) { return dispatch<false>(prio, std::move(task)); }

  [[nodiscard]] bool execute(unique_task task) override
  {
    return dispatch<true>(enqueue_priority::min, std::move(task));
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    return dispatch<false>(enqueue_priority::min, std::move(task));
  }

  /// Retrieves the executors of different priority levels provided by this task fork limiter. The first executor in the
  /// returned span is the one with the highest priority.
  span<executor_type> get_executors() { return exec_list; }

private:
  /// Value representative of one single fork in the state.
  static constexpr uint64_t one_fork_mask = static_cast<uint64_t>(1U) << 32U;
  using queue_type                        = concurrent_queue<unique_task,
                                                             concurrent_queue_policy::moodycamel_lockfree_mpmc,
                                                             concurrent_queue_wait_policy::non_blocking>;

  template <bool Execute>
  bool dispatch(enqueue_priority prio, unique_task task)
  {
    size_t queue_index =
        std::min(static_cast<size_t>(enqueue_priority::max) - static_cast<size_t>(prio), queues.size() - 1);

    // Save task to process in internal queue.
    if (not queues[queue_index].try_push(std::move(task))) {
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
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute([this]() { handle_fork_tasks(false); });
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { handle_fork_tasks(false); });
    }
    if (not dispatch_successful) {
      // Failed to dispatch the task, handle it accordingly.
      handle_failed_dispatch();
      return false;
    }
    return true;
  }

  /// Called to handle the tasks of a fork.
  void handle_fork_tasks(bool job_already_reserved)
  {
    unique_task task;

    unsigned max_pops = max_batch;
    while (std::exchange(job_already_reserved, false) or reserve_fork_task()) {
      if (not pop_task(task)) {
        // We were unable to pop a task, but there are still jobs in the queue. We will defer the remaining tasks.
        defer_remaining_tasks(true);
        return;
      }

      // Execute the task.
      task();

      // Check if we should yield back control to the worker because we reached the batch limit.
      if (--max_pops == 0) {
        defer_remaining_tasks(false);
        return;
      }
    }
  }

  bool defer_remaining_tasks(bool job_already_reserved)
  {
    // Dispatch batch dequeue job.
    bool dispatch_successful = detail::get_task_executor_ref(out_exec).defer(
        [this, job_already_reserved]() { handle_fork_tasks(job_already_reserved); });
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
    // Note: We retry several times because, even if we know that there are jobs pending, tasks might be concurrently
    // being enqueued and dequeued at different priority levels.
    static constexpr unsigned max_pop_failures = 10;
    for (unsigned attempt = 0; attempt != max_pop_failures; ++attempt) {
      // Starting from the queue with the highest priority, try to pop a task.
      for (auto& queue : queues) {
        if (queue.try_pop(task)) {
          return true;
        }
      }
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

  /// Called when a fork wants to reserve a task to be run.
  /// \return True if the fork can continue processing tasks, false if it should yield back control to the worker,
  /// because there are not more pending tasks.
  [[nodiscard]] bool reserve_fork_task()
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
  /// Logger used to report errors and warnings.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");

  // Executor to which tasks are dispatched in serialized manner.
  ExecType out_exec;

  /// Queue(s) of pending tasks.
  std::vector<queue_type> queues;

  /// Executor list that contains the executors of different priority levels provided by this task fork limiter.
  std::vector<executor_type> exec_list;

  /// \brief Current state of the task fork limiter. It contains two parts: (i) 32 LSBs for the jobs stored in the
  /// queue, and (ii) 32 MSBs for the number of active forks.
  std::atomic<uint64_t> state{0};

  /// Counter of the number of unexpected pop failures.
  std::atomic<unsigned> count_pop_fails{0};
};

/// \brief Create an adaptor of the task_executor that limits the number of concurrent tasks to \c max_fork_size. This
/// adaptor only provides a single priority level for the tasks.
/// \param[in] out_exec Executor to which tasks are ultimately dispatched.
/// \param[in] max_fork_size Maximum number of concurrent tasks that can run concurrently in the wrapped executor.
/// \param[in] qsize Sizes of the internal queue.
/// \param[in] max_batch Maximum number of tasks that can be processed by one fork, before it yields control back to the
/// worker.
template <typename OutExec = task_executor*>
std::unique_ptr<task_executor> make_task_fork_limiter_ptr(OutExec&& out_exec,
                                                          unsigned  max_fork_size,
                                                          unsigned  qsize,
                                                          unsigned  max_batch = std::numeric_limits<unsigned>::max())
{
  return std::make_unique<task_fork_limiter<OutExec>>(std::forward<OutExec>(out_exec), max_fork_size, qsize, max_batch);
}

} // namespace srsran
