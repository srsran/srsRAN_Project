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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/detail/priority_task_queue.h"
#include "srsran/support/executors/detail/task_executor_utils.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/executors/thread_utils.h"
#include <thread>

namespace srsran {

namespace detail {

// Specialization for strand with multiple queues of different priorities.
struct priority_strand_queue {
  explicit priority_strand_queue(span<const concurrent_queue_params> strand_queue_params) :
    queue(strand_queue_params, std::chrono::microseconds{0})
  {
  }

  bool try_push(enqueue_priority prio, unique_task task) { return queue.try_push(prio, std::move(task)); }

  bool pop(unique_task& task)
  {
    constexpr std::chrono::microseconds time_to_wait{1000};
    std::chrono::microseconds           telapsed{0};
    do {
      if (queue.try_pop(task)) {
        return true;
      }
      std::this_thread::sleep_for(std::chrono::microseconds{1});
      telapsed += std::chrono::microseconds{1};
    } while (telapsed < time_to_wait);
    return false;
  }

  priority_task_queue queue;
};

// Specialization for strand with single queue.
template <concurrent_queue_policy QueuePolicy>
struct strand_queue {
  explicit strand_queue(unsigned strand_queue_size) : queue(strand_queue_size) {}

  bool try_push(unique_task task) { return queue.try_push(std::move(task)); }

  bool pop(unique_task& task)
  {
    constexpr std::chrono::microseconds time_to_wait{1000};
    std::chrono::microseconds           telapsed{0};
    do {
      if (queue.try_pop(task)) {
        return true;
      }
      std::this_thread::sleep_for(std::chrono::microseconds{1});
      telapsed += std::chrono::microseconds{1};
    } while (telapsed < time_to_wait);
    return false;
  }

  concurrent_queue<unique_task, QueuePolicy, concurrent_queue_wait_policy::non_blocking> queue;
};

} // namespace detail

namespace detail {

template <typename Executor, typename QueueType>
class task_strand_impl
{
public:
  template <typename ExecType, typename... QueueParams>
  explicit task_strand_impl(ExecType&& exec_, QueueParams&&... queue_params) :
    out_exec(std::forward<ExecType>(exec_)), queue(std::forward<QueueParams>(queue_params)...)
  {
  }

  // Called once task is enqueued in the strand queue to assess whether the strand should be locked and dispatched.
  bool handle_enqueued_task(bool is_execute)
  {
    // If the task_strand is acquired, it means that no other thread is running the pending tasks and we need to
    // dispatch a job to run them to the wrapped executor.
    bool strand_acquired = job_count.fetch_add(1U, std::memory_order_acq_rel) == 0U;
    if (not strand_acquired) {
      // If we failed to acquire the task_strand, it means that another call has already acquired it and scheduled
      // the running of all enqueued jobs, including the one we just enqueued in this call.
      return true;
    }

    // Check if the adapted executor gives us permission to run pending tasks inline. An example of when this may
    // happen is when the caller is running in the same execution context of the underlying task worker or task worker
    // pool. However, this permission is still not a sufficient condition to simply call the task inline. For
    // instance, if the execution context is a thread pool, we have to ensure that the task_strand is not already
    // acquired by another thread of the same pool. If we do not do this, running the task inline would conflict with
    // the task_strand strict serialization requirements. For this reason, we will always enqueue the task and try to
    // acquire the task_strand.
    bool dispatch_successful;
    if (is_execute) {
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute([this]() { run_enqueued_tasks(); });
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer([this]() { run_enqueued_tasks(); });
    }
    if (not dispatch_successful) {
      // Unable to dispatch executor job to run enqueued tasks.
      this->handle_failed_task_dispatch();
      return false;
    }
    return true;
  }

  // Run all tasks that have been enqueued in the strand, and unlocks the strand.
  void run_enqueued_tasks()
  {
    unique_task task;
    uint32_t    queue_size = this->job_count.load(std::memory_order_acquire);
    while (queue_size > 0) {
      // Note: We use a blocking pop because (in theory) at this point we have the guarantee that at least one task
      // is stored in the queue (job_count > 0). However, we still apply a timeout policy to catch unexpected
      // situations or invalid states of the strand. We could not use a non-blocking pop because some of the MPMC
      // queue implementations have spurious failures.
      unsigned run_count = 0;
      for (; run_count != queue_size and queue.pop(task); ++run_count) {
        task();
      }
      if (run_count != queue_size) {
        // Unexpected failure to pop enqueued tasks. Possible reason: Are you using an SPSC queue with multiple
        // producers?
        srslog::fetch_basic_logger("ALL").error(
            "Couldn't run all pending tasks stored in strand in the thread {} (popped tasks={} < queue_size={}).",
            this_thread_name(),
            run_count,
            queue_size);
        run_count = queue_size;
      }

      // We have run all the tasks that were enqueued since when we computed queue_size.
      // Recompute the queue_size to check if there are tasks that were enqueued in the meantime.
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  void handle_failed_task_dispatch()
  {
    // Pop enqueued tasks until number of enqueued jobs is zero.
    // Note: Since we acquired the task_strand, the task enqueued in this call should always be the first being
    // popped. Note: If there is a single producer, only the task enqueued in this call will be popped. Note: As we
    // currently hold the strand, there is no concurrent thread popping tasks.
    uint32_t queue_size = this->job_count.load(std::memory_order_acquire);

    srslog::fetch_basic_logger("ALL").warning("Failed to dispatch {} tasks stored in strand. Discarding them...",
                                              queue_size);

    unique_task dropped_task;
    while (queue_size > 0) {
      unsigned run_count = 0;
      for (; run_count != queue_size and this->queue.pop(dropped_task); ++run_count) {
        // do nothing with popped task.
        if (run_count != queue_size) {
          // Unexpected failure to pop enqueued tasks. Possible reason: Are you using an SPSC queue with multiple
          // producers?
          srslog::fetch_basic_logger("ALL").error(
              "Couldn't run all pending tasks stored in strand in the thread {} (popped tasks={} < queue_size={}).",
              this_thread_name(),
              run_count,
              queue_size);
        }
      }
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  // Executor to which tasks are dispatched in serialized manner.
  Executor out_exec;

  // Queue of pending tasks.
  QueueType queue;

  // Number of jobs currently enqueued in the strand.
  std::atomic<uint32_t> job_count{0};
};

} // namespace detail

template <typename OutExec, concurrent_queue_policy QueuePolicy>
class task_strand;

/// \brief Executor for a task strand with a single priority level and queueing policy \c QueuePolicy.
template <typename OutExec, concurrent_queue_policy QueuePolicy>
class task_strand_executor final : public task_executor
{
public:
  task_strand_executor() = default;
  task_strand_executor(task_strand<OutExec, QueuePolicy>& strand_) : strand(&strand_) {}

  [[nodiscard]] bool execute(unique_task task) override { return strand->execute(std::move(task)); }

  [[nodiscard]] bool defer(unique_task task) override { return strand->defer(std::move(task)); }

private:
  task_strand<OutExec, QueuePolicy>* strand = nullptr;
};

/// \brief This class implements a strand with a specified enqueueing policy, with no prioritization.
///
/// \tparam Executor Executor that the strands dispatches tasks to.
/// \tparam QueuePolicy Enqueueing policy to dispatch tasks to the strand.
template <typename OutExec, concurrent_queue_policy QueuePolicy>
class task_strand final : public task_executor
{
public:
  using executor_type = task_strand_executor<OutExec, QueuePolicy>;

  template <typename ExecType>
  task_strand(ExecType&& out_exec, unsigned qsize) : impl(std::forward<ExecType>(out_exec), qsize), exec(*this)
  {
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    // Enqueue task in task_strand queue.
    if (not impl.queue.try_push(std::move(task))) {
      return false;
    }
    return impl.handle_enqueued_task(true);
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    // Enqueue task in task_strand queue.
    if (not impl.queue.try_push(std::move(task))) {
      return false;
    }
    return impl.handle_enqueued_task(false);
  }

  /// Number of priority levels supported by this strand.
  size_t nof_priority_levels() { return 1; }

  /// Get a view to the basic executor of the strand.
  executor_type& get_executor() { return exec; }

private:
  detail::task_strand_impl<OutExec, detail::strand_queue<QueuePolicy>> impl;
  executor_type                                                        exec;
};

/// \brief Executor that dispatches tasks with a specified priority to a strand that supports multiple priority levels.
template <typename StrandType>
class priority_task_strand_executor final : public task_executor
{
public:
  template <typename StrandRef>
  priority_task_strand_executor(enqueue_priority prio_, StrandRef&& strand_ref) :
    prio(prio_), strand(std::forward<StrandRef>(strand_ref))
  {
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    return detail::invoke_execute(strand, prio, std::move(task));
  }
  [[nodiscard]] bool defer(unique_task task) override { return detail::invoke_defer(strand, prio, std::move(task)); }

private:
  enqueue_priority prio;
  StrandType       strand;
};

/// \brief Task strand that supports multiple priority levels for the dispatched tasks.
///
/// \tparam OutExec Executor that the strands dispatches tasks to.
template <typename OutExec>
class priority_task_strand
{
public:
  using strand_type   = priority_task_strand<OutExec>;
  using executor_type = priority_task_strand_executor<strand_type&>;

  template <typename ExecType, typename ArrayOfQueueParams>
  priority_task_strand(ExecType&& out_exec, const ArrayOfQueueParams& strand_queue_params) :
    impl(std::forward<ExecType>(out_exec), strand_queue_params)
  {
    static_assert(std::is_same_v<typename std::decay_t<ArrayOfQueueParams>::value_type, concurrent_queue_params>,
                  "Invalid queue params type");
    exec_list.reserve(nof_priority_levels());
    for (unsigned i = 0; i != strand_queue_params.size(); ++i) {
      exec_list.emplace_back(executor_type{detail::queue_index_to_enqueue_priority(i, nof_priority_levels()), *this});
    }
  }

  /// \brief Dispatch task with priority \c prio. If possible, the task can be run inline.
  [[nodiscard]] bool execute(enqueue_priority prio, unique_task task)
  {
    // Enqueue task in task_strand queue.
    if (not impl.queue.try_push(prio, std::move(task))) {
      return false;
    }
    return impl.handle_enqueued_task(true);
  }

  /// \brief Dispatch task with priority \c prio. The task is never run inline.
  [[nodiscard]] bool defer(enqueue_priority prio, unique_task task)
  {
    // Enqueue task in task_strand queue.
    if (not impl.queue.try_push(prio, std::move(task))) {
      return false;
    }
    return impl.handle_enqueued_task(false);
  }

  /// Number of priority levels supported by this strand.
  size_t nof_priority_levels() { return impl.queue.queue.nof_priority_levels(); }

  /// \brief Get a view of the basic executors for the different task priorities of the strand.
  ///
  /// The first is the highest priority, and the last, the lowest.
  span<executor_type> get_executors() { return exec_list; }

private:
  detail::task_strand_impl<OutExec, detail::priority_strand_queue> impl;
  std::vector<executor_type>                                       exec_list;
};

/// \brief Creates a task strand instance with a single priority task level.
template <typename OutExec>
std::unique_ptr<task_executor> make_task_strand_ptr(OutExec&& out_exec, const concurrent_queue_params& qparams)
{
  switch (qparams.policy) {
    case concurrent_queue_policy::lockfree_mpmc:
      return std::make_unique<task_strand<OutExec, concurrent_queue_policy::lockfree_mpmc>>(
          std::forward<OutExec>(out_exec), qparams.size);
    case concurrent_queue_policy::lockfree_spsc:
      return std::make_unique<task_strand<OutExec, concurrent_queue_policy::lockfree_spsc>>(
          std::forward<OutExec>(out_exec), qparams.size);
    case concurrent_queue_policy::locking_mpmc:
      return std::make_unique<task_strand<OutExec, concurrent_queue_policy::locking_mpmc>>(
          std::forward<OutExec>(out_exec), qparams.size);
    case concurrent_queue_policy::locking_mpsc:
      return std::make_unique<task_strand<OutExec, concurrent_queue_policy::locking_mpsc>>(
          std::forward<OutExec>(out_exec), qparams.size);
    default:
      break;
  }
  report_fatal_error("Unsupported queue policy.");
  return nullptr;
}

/// \brief Creates a task strand instance with a single priority task level.
template <concurrent_queue_policy QueuePolicy, typename OutExec>
std::unique_ptr<task_executor> make_task_strand_ptr(OutExec&& out_exec, unsigned strand_queue_size)
{
  return std::make_unique<task_strand<OutExec, QueuePolicy>>(std::forward<OutExec>(out_exec), strand_queue_size);
}

/// \brief Creates a task strand instance with several priority task levels.
template <typename OutExec>
std::unique_ptr<priority_task_strand<OutExec>>
make_priority_task_strand_ptr(OutExec&& out_exec, span<const concurrent_queue_params> qparams)
{
  return std::make_unique<priority_task_strand<OutExec>>(std::forward<OutExec>(out_exec), qparams);
}

/// \brief Creates a task executor with a given priority for a strand that supports multiple priorities.
template <typename StrandPtr>
priority_task_strand_executor<StrandPtr> make_priority_task_executor(enqueue_priority prio, StrandPtr strand_ptr)
{
  return {prio, std::move(strand_ptr)};
}
template <typename StrandPtr>
std::unique_ptr<task_executor> make_priority_task_executor_ptr(enqueue_priority prio, StrandPtr strand_ptr)
{
  return std::make_unique<priority_task_strand_executor<StrandPtr>>(prio, std::move(strand_ptr));
}

template <typename StrandPtr>
std::vector<priority_task_strand_executor<StrandPtr>> make_priority_task_executors(StrandPtr strand_ptr)
{
  std::vector<priority_task_strand_executor<StrandPtr>> ret;
  const unsigned                                        nof_levels = strand_ptr->nof_priority_levels();
  ret.reserve(nof_levels);
  for (unsigned i = 0; i != nof_levels; ++i) {
    ret.push_back(make_priority_task_executor(detail::queue_index_to_enqueue_priority(i, nof_levels), strand_ptr));
  }
  return ret;
}

template <typename StrandPtr>
std::vector<std::unique_ptr<task_executor>> make_priority_task_executor_ptrs(StrandPtr strand_ptr)
{
  std::vector<std::unique_ptr<task_executor>> ret;
  const unsigned                              nof_levels = strand_ptr->nof_priority_levels();
  ret.reserve(nof_levels);
  for (unsigned i = 0; i != nof_levels; ++i) {
    ret.push_back(make_priority_task_executor_ptr(detail::queue_index_to_enqueue_priority(i, nof_levels), strand_ptr));
  }
  return ret;
}

} // namespace srsran
