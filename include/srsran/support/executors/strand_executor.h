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
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/task_executor_utils.h"

namespace srsran {

template <typename Executor, concurrent_queue_policy queue_policy>
class strand_executor final : public task_executor
{
public:
  template <typename Exec>
  explicit strand_executor(Exec&& executor_, unsigned strand_queue_size) :
    executor(std::forward<Exec>(executor_)), queue(strand_queue_size)
  {
  }

  bool execute(unique_task task) override { return dispatch_impl(std::move(task), true); }
  bool defer(unique_task task) override { return dispatch_impl(std::move(task), false); }

private:
  bool dispatch_impl(unique_task task, bool is_execute)
  {
    // Check if the adapted executor gives us permission to run pending tasks inline. An example of when this may happen
    // is when the caller is running in the same execution context of the underlying task worker or task worker pool.
    // However, this permission is still not a sufficient condition to simply call the task inline. For instance, if the
    // execution context is a thread pool, we have to ensure that the strand is not already acquired by another thread
    // of the same pool. If we do not do this, running the task inline would conflict with the strand strict
    // serialization requirements. For this reason, we will always enqueue the task and try to acquire the strand.
    bool can_run_inline = is_execute and detail::executor_lets_run_task_inline(executor);

    // Enqueue task in strand queue.
    if (not queue.try_push(std::move(task))) {
      return false;
    }

    // Try to acquire the strand.
    bool strand_acquired = job_count.fetch_add(1U, std::memory_order_acq_rel) == 0U;
    if (not strand_acquired) {
      // If we failed to acquire the strand, it means that another call has already acquired it and scheduled
      // the running of all enqueued jobs, including the one we just enqueued in this call.
      return true;
    }

    // We were able to acquire the strand. That means that no other thread is running the pending tasks and we need
    // to dispatch a job to run them.

    if (can_run_inline) {
      // Adapted executor gave us permission to run the task inline and we acquired the strand.
      run_enqueued_tasks();
      return true;
    }

    // The task has to be dispatched to the adapted executor.

    auto run_pending_and_release_strand = [this]() {
      // Run all enqueued tasks.
      run_enqueued_tasks();
    };

    if (not executor.defer(run_pending_and_release_strand)) {
      // Unable to dispatch executor job to run enqueued tasks.
      // Pop enqueued tasks until number of enqueued jobs is zero.
      // Note: Since we acquired the strand, the task enqueued in this call should always be the first being popped.
      // Note: If there is a single producer, only the task enqueued in this call will be popped.
      // Note: As we currently hold the strand, there is no concurrent thread popping tasks.
      uint32_t    queue_size = job_count.load(std::memory_order_acquire);
      unique_task dropped_task;
      while (queue_size > 0) {
        unsigned run_count = 0;
        for (; run_count != queue_size and queue.try_pop(dropped_task); ++run_count) {
          // do nothing with popped task.
        }
        queue_size = job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
      }
      return false;
    }
    return true;
  }

  void run_enqueued_tasks()
  {
    unique_task task;
    uint32_t    queue_size = job_count.load(std::memory_order_acquire);
    srsran_sanity_check(queue_size > 0, "This function should not have been called for an empty queue.");
    while (queue_size > 0) {
      unsigned run_count = 0;
      for (; run_count != queue_size and queue.try_pop(task); ++run_count) {
        task();
      }
      if (run_count != queue_size) {
        // Unexpected failure to pop enqueued tasks. It might be due to queue shutdown.
        srslog::fetch_basic_logger("ALL").info("Couldn't run all pending tasks in strand");
      }

      // We have run all the tasks that were enqueued since when we computed queue_size.
      // Recompute the queue_size to check if there are tasks that were enqueued in the meantime.
      queue_size = job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  // Executor to which tasks are dispatched in serialized manner.
  Executor executor;

  // Queue of pending tasks waiting for their turn.
  concurrent_queue<unique_task, queue_policy, concurrent_queue_wait_policy::non_blocking> queue;

  // Number of jobs currently enqueued.
  std::atomic<uint32_t> job_count{0};
};

template <typename Executor, concurrent_queue_policy queue_policy = concurrent_queue_policy::lockfree_mpmc>
inline std::unique_ptr<strand_executor<Executor, queue_policy>> make_strand_executor_ptr(Executor&& executor,
                                                                                         unsigned   strand_queue_size)
{
  return std::make_unique<strand_executor<Executor, queue_policy>>(std::forward<Executor>(executor), strand_queue_size);
}

template <typename Executor>
inline std::unique_ptr<task_executor>
make_strand_executor_ptr(Executor&& executor, concurrent_queue_policy queue_policy, unsigned strand_queue_size)
{
  switch (queue_policy) {
    case concurrent_queue_policy::lockfree_spsc:
      return make_strand_executor_ptr<Executor, concurrent_queue_policy::lockfree_spsc>(
          std::forward<Executor>(executor), strand_queue_size);
    case concurrent_queue_policy::lockfree_mpmc:
      return make_strand_executor_ptr<Executor, concurrent_queue_policy::lockfree_mpmc>(
          std::forward<Executor>(executor), strand_queue_size);
    case concurrent_queue_policy::locking_mpmc:
      break;
    case concurrent_queue_policy::locking_mpsc:
      break;
    default:
      break;
  }
  return nullptr;
}

} // namespace srsran