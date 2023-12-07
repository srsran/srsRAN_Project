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

namespace detail {

// Specialization for strand with multiple queues of different priorities.
template <concurrent_queue_policy... QueuePolicies>
struct strand_queue {
  explicit strand_queue(const std::array<unsigned, sizeof...(QueuePolicies)>& strand_queue_sizes) :
    queue(strand_queue_sizes, std::chrono::microseconds{0})
  {
  }
  explicit strand_queue(span<const unsigned> strand_queue_sizes) :
    queue(
        [&strand_queue_sizes]() {
          report_error_if_not(strand_queue_sizes.size() == sizeof...(QueuePolicies),
                              "Number of queue sizes must match number of policies");
          std::array<unsigned, sizeof...(QueuePolicies)> sizes;
          for (size_t i = 0; i < sizeof...(QueuePolicies); ++i) {
            sizes[i] = strand_queue_sizes[i];
          }
          return sizes;
        }(),
        std::chrono::microseconds{0})
  {
  }

  template <enqueue_priority Priority>
  bool try_push(unique_task task)
  {
    return queue.template try_push<Priority>(std::move(task));
  }

  concurrent_priority_queue<unique_task, QueuePolicies...> queue;
};

// Specialization for strand with single queue.
template <concurrent_queue_policy QueuePolicy>
struct strand_queue<QueuePolicy> {
  explicit strand_queue(unsigned strand_queue_size) : queue(strand_queue_size) {}
  explicit strand_queue(span<const unsigned> strand_queue_sizes) :
    queue([&strand_queue_sizes]() {
      report_error_if_not(strand_queue_sizes.size() == 1, "Number of queue sizes must match number of policies");
      return strand_queue_sizes[0];
    }())
  {
  }

  template <enqueue_priority Priority>
  bool try_push(unique_task task)
  {
    return queue.try_push(std::move(task));
  }

  concurrent_queue<unique_task, QueuePolicy, concurrent_queue_wait_policy::non_blocking> queue;
};

} // namespace detail

/// \brief This class implements a strand of one or more enqueueing policies, each with a different priority.
///
/// \tparam Executor Executor that the strands dispatches tasks to.
/// \tparam QueuePolicies Enqueueing policies for each of the task priority levels supported by the strand.
template <typename Executor, concurrent_queue_policy... QueuePolicies>
class task_strand
{
public:
  static constexpr std::array<concurrent_queue_policy, sizeof...(QueuePolicies)> queue_policies{QueuePolicies...};

  /// \brief Executor with a compile-time defined task priority that dispatches tasks to the associated strand.
  ///
  /// This executor does not control the lifetime of the strand.
  template <enqueue_priority Priority>
  class executor final : public task_executor
  {
  public:
    explicit executor(task_strand* parent_) : parent(parent_) {}

    SRSRAN_NODISCARD bool execute(unique_task task) override { return parent->execute<Priority>(std::move(task)); }

    SRSRAN_NODISCARD bool defer(unique_task task) override { return parent->defer<Priority>(std::move(task)); }

  private:
    task_strand* parent = nullptr;
  };

  template <typename Exec, typename... Args>
  explicit task_strand(Exec&& executor_, Args&&... queue_params) :
    out_exec(std::forward<Exec>(executor_)), queue(std::forward<Args>(queue_params)...)
  {
  }

  template <enqueue_priority Priority>
  bool execute(unique_task task)
  {
    return dispatch_impl<Priority>(std::move(task), true);
  }

  template <enqueue_priority Priority>
  bool defer(unique_task task)
  {
    return dispatch_impl<Priority>(std::move(task), false);
  }

  template <enqueue_priority Priority = enqueue_priority::min>
  executor<Priority> get_executor()
  {
    return executor<Priority>{this};
  }

  std::unique_ptr<task_executor> get_executor_ptr(enqueue_priority prio)
  {
    return get_executor_ptr_impl(prio, std::make_index_sequence<sizeof...(QueuePolicies)>{});
  }

  static size_t nof_priority_levels() { return sizeof...(QueuePolicies); }

private:
  template <typename Func, size_t... Is>
  std::unique_ptr<task_executor> get_executor_ptr_impl(enqueue_priority priority, std::index_sequence<Is...> /*unused*/)
  {
    const size_t                   idx = detail::enqueue_priority_to_queue_index(priority, sizeof...(QueuePolicies));
    std::unique_ptr<task_executor> exec;
    (void)std::initializer_list<int>{[this, idx, &exec]() {
      if (idx == Is) {
        exec = std::make_unique<executor<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies))>>(this);
      }
      return 0;
    }()...};
    return exec;
  }

  template <enqueue_priority Priority>
  bool dispatch_impl(unique_task task, bool is_execute)
  {
    // Enqueue task in task_strand queue.
    if (not queue.template try_push<Priority>(std::move(task))) {
      return false;
    }

    // Try to acquire the task_strand.
    bool strand_acquired = job_count.fetch_add(1U, std::memory_order_acq_rel) == 0U;
    if (not strand_acquired) {
      // If we failed to acquire the task_strand, it means that another call has already acquired it and scheduled
      // the running of all enqueued jobs, including the one we just enqueued in this call.
      return true;
    }

    // We were able to acquire the task_strand. That means that no other thread is running the pending tasks and we need
    // to dispatch a job to run them.

    // Check if the adapted executor gives us permission to run pending tasks inline. An example of when this may happen
    // is when the caller is running in the same execution context of the underlying task worker or task worker pool.
    // However, this permission is still not a sufficient condition to simply call the task inline. For instance, if the
    // execution context is a thread pool, we have to ensure that the task_strand is not already acquired by another
    // thread of the same pool. If we do not do this, running the task inline would conflict with the task_strand strict
    // serialization requirements. For this reason, we will always enqueue the task and try to acquire the task_strand.
    bool can_run_inline = is_execute and detail::executor_lets_run_task_inline(out_exec);

    if (can_run_inline) {
      // Adapted executor gave us permission to run the task inline and we acquired the task_strand.
      run_enqueued_tasks();
      return true;
    }

    // The task has to be dispatched to the adapted executor.

    auto run_pending_and_release_strand = [this]() {
      // Run all enqueued tasks.
      run_enqueued_tasks();
    };

    if (not out_exec.defer(run_pending_and_release_strand)) {
      // Unable to dispatch executor job to run enqueued tasks.
      // Pop enqueued tasks until number of enqueued jobs is zero.
      // Note: Since we acquired the task_strand, the task enqueued in this call should always be the first being
      // popped. Note: If there is a single producer, only the task enqueued in this call will be popped. Note: As we
      // currently hold the strand, there is no concurrent thread popping tasks.
      uint32_t    queue_size = job_count.load(std::memory_order_acquire);
      unique_task dropped_task;
      while (queue_size > 0) {
        unsigned run_count = 0;
        for (; run_count != queue_size and queue.queue.try_pop(dropped_task); ++run_count) {
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
      for (; run_count != queue_size and queue.queue.try_pop(task); ++run_count) {
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
  Executor out_exec;

  // Queue of pending tasks waiting for their turn.
  detail::strand_queue<QueuePolicies...> queue;

  // Number of jobs currently enqueued.
  std::atomic<uint32_t> job_count{0};
};

/// \brief Creates a task strand instance given a list of parameters
template <concurrent_queue_policy QueuePolicy, typename OutExec>
std::unique_ptr<task_strand<OutExec, QueuePolicy>> make_task_strand_ptr(OutExec&& out_exec, unsigned strand_queue_size)
{
  return std::make_unique<task_strand<OutExec, QueuePolicy>>(std::forward<OutExec>(out_exec), strand_queue_size);
}
template <concurrent_queue_policy... QueuePolicies, typename OutExec>
std::unique_ptr<task_strand<OutExec, QueuePolicies...>> make_task_strand_ptr(OutExec&&            out_exec,
                                                                             span<const unsigned> strand_queue_sizes)
{
  report_error_if_not(strand_queue_sizes.size() == sizeof...(QueuePolicies),
                      "Number of queue sizes must match number of policies ({}!={})",
                      strand_queue_sizes.size(),
                      sizeof...(QueuePolicies));
  return std::make_unique<task_strand<OutExec, QueuePolicies...>>(std::forward<OutExec>(out_exec), strand_queue_sizes);
}

/// \brief Executor with a compile-time defined task priority, that controls the lifetime of the associated strand.
template <enqueue_priority StrandExecPriority, typename OutExec, concurrent_queue_policy... QueuePolicies>
class shared_strand_executor final : public task_executor
{
  using strand_type = task_strand<OutExec, QueuePolicies...>;

public:
  explicit shared_strand_executor(std::shared_ptr<strand_type> ptr_) : ptr(std::move(ptr_)) {}

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    return ptr->template execute<StrandExecPriority>(std::move(task));
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override
  {
    return ptr->template execute<StrandExecPriority>(std::move(task));
  }

private:
  std::shared_ptr<strand_type> ptr;
};

/// \brief Make a strand executor that manages the lifetime of a strand with a single queue.
template <concurrent_queue_policy QueuePolicy, typename OutExec>
std::unique_ptr<task_executor> make_strand_executor_ptr(OutExec&& out_exec, unsigned queue_size)
{
  return std::make_unique<shared_strand_executor<enqueue_priority::max, OutExec, QueuePolicy>>(
      make_task_strand_ptr<QueuePolicy, OutExec>(std::forward<OutExec>(out_exec), queue_size));
}

namespace detail {

template <typename OutExec, concurrent_queue_policy... QueuePolicies, size_t... Is>
std::vector<std::unique_ptr<task_executor>>
make_shared_execs_helper(std::unique_ptr<task_strand<OutExec, QueuePolicies...>> strand_ptr,
                         std::index_sequence<Is...> /*unused*/)
{
  std::shared_ptr<task_strand<OutExec, QueuePolicies...>> strand_shared{std::move(strand_ptr)};

  std::array<std::unique_ptr<task_executor>, sizeof...(QueuePolicies)> created_execs{
      std::make_unique<shared_strand_executor<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies)),
                                              OutExec,
                                              QueuePolicies...>>(strand_shared)...};

  std::vector<std::unique_ptr<task_executor>> execs;
  execs.reserve(sizeof...(QueuePolicies));
  for (auto& exec : created_execs) {
    execs.push_back(std::move(exec));
  }
  return execs;
}

} // namespace detail

/// \brief Create all executors associated with a given strand. The executors will manage the strand lifetime via
/// reference counting.
template <concurrent_queue_policy... QueuePolicies, typename OutExec>
std::vector<std::unique_ptr<task_executor>> make_strand_executor_ptrs(OutExec&& out_exec, span<const unsigned> qsizes)
{
  return detail::make_shared_execs_helper(
      make_task_strand_ptr<QueuePolicies...>(std::forward<OutExec>(out_exec), qsizes),
      std::make_index_sequence<sizeof...(QueuePolicies)>{});
}

namespace detail {

constexpr size_t MAX_QUEUES_PER_STRAND = 1;

// Special case to stop recursion for task queue policies.
template <typename OutExec,
          concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<MAX_QUEUES_PER_STRAND<sizeof...(QueuePolicies), int> =
                               0> std::vector<std::unique_ptr<task_executor>>
              make_strand_executors_iter_helper(OutExec&& out_exec, span<const concurrent_queue_params> strand_queues)
{
  report_fatal_error("Strands with equal or more than {} queues are not supported", MAX_QUEUES_PER_STRAND);
  return {};
};

template <typename OutExec,
          concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<sizeof...(QueuePolicies) <= MAX_QUEUES_PER_STRAND, int> = 0>
std::vector<std::unique_ptr<task_executor>>
make_strand_executors_iter_helper(OutExec&& out_exec, span<const concurrent_queue_params> strand_queues)
{
  constexpr static size_t Is = sizeof...(QueuePolicies);
  if (strand_queues.empty()) {
    return {};
  }
  if (Is == strand_queues.size()) {
    std::array<unsigned, Is> strand_queue_sizes;
    for (unsigned i = 0; i != strand_queues.size(); ++i) {
      strand_queue_sizes[i] = strand_queues[i].size;
    }
    return make_strand_executor_ptrs<QueuePolicies...>(std::forward<OutExec>(out_exec),
                                                       span<const unsigned>(strand_queue_sizes));
  }

  switch (strand_queues[Is].policy) {
    case concurrent_queue_policy::locking_mpmc: {
      return make_strand_executors_iter_helper<OutExec, QueuePolicies..., concurrent_queue_policy::locking_mpmc>(
          std::forward<OutExec>(out_exec), strand_queues);
    } break;
    case concurrent_queue_policy::lockfree_mpmc: {
      return make_strand_executors_iter_helper<OutExec, QueuePolicies..., concurrent_queue_policy::lockfree_mpmc>(
          std::forward<OutExec>(out_exec), strand_queues);
    } break;
    case concurrent_queue_policy::locking_mpsc: {
      return make_strand_executors_iter_helper<OutExec, QueuePolicies..., concurrent_queue_policy::locking_mpsc>(
          std::forward<OutExec>(out_exec), strand_queues);
    } break;
    case concurrent_queue_policy::lockfree_spsc: {
      return make_strand_executors_iter_helper<OutExec, QueuePolicies..., concurrent_queue_policy::lockfree_spsc>(
          std::forward<OutExec>(out_exec), strand_queues);
    } break;
    default:
      report_fatal_error("Unsupported queue policy.");
  }
  return {};
}

} // namespace detail

/// \brief Creates a list of task executors associated with a strand that points to the provided out executor.
template <typename OutExec>
std::vector<std::unique_ptr<task_executor>> make_strand_executor_ptrs(OutExec&&                           out_exec,
                                                                      span<const concurrent_queue_params> strand_queues)
{
  return detail::make_strand_executors_iter_helper<OutExec>(std::forward<OutExec>(out_exec), strand_queues);
}

} // namespace srsran