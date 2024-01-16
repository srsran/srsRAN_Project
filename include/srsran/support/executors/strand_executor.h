/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

  template <enqueue_priority Priority>
  auto get_enqueuer()
  {
    return queue.template get_non_blocking_enqueuer<Priority>();
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

  template <enqueue_priority Priority>
  auto get_enqueuer()
  {
    return non_blocking_enqueuer<unique_task, QueuePolicy>{queue};
  }

  concurrent_queue<unique_task, QueuePolicy, concurrent_queue_wait_policy::non_blocking> queue;
};

} // namespace detail

/// \brief Base class for task strand implementations.
class base_task_strand
{
public:
  virtual ~base_task_strand() = default;

  /// Called once task is enqueued in the strand queue to assess whether the strand should be locked and dispatched.
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
    return this->dispatch_strand(is_execute);
  }

protected:
  virtual bool dispatch_strand(bool is_execute) = 0;

  // Number of jobs currently enqueued in the strand.
  std::atomic<uint32_t> job_count{0};
};

namespace detail {

template <concurrent_queue_policy QueuePolicy, typename StrandPtr>
class task_strand_executor_impl final : public task_executor
{
public:
  template <typename U>
  task_strand_executor_impl(non_blocking_enqueuer<unique_task, QueuePolicy> enqueuer_, U&& strand_) :
    enqueuer(enqueuer_), strand(std::forward<U>(strand_))
  {
  }

  SRSRAN_NODISCARD bool execute(unique_task task) override
  {
    // Enqueue task in task_strand queue.
    if (not enqueuer.try_push(std::move(task))) {
      // strand queue is full.
      return false;
    }
    return strand->handle_enqueued_task(true);
  }

  SRSRAN_NODISCARD bool defer(unique_task task) override
  {
    // Enqueue task in task_strand queue.
    if (not enqueuer.try_push(std::move(task))) {
      // strand queue is full.
      return false;
    }
    return strand->handle_enqueued_task(false);
  }

private:
  non_blocking_enqueuer<unique_task, QueuePolicy> enqueuer;
  StrandPtr                                       strand;
};

template <concurrent_queue_policy... QueuePolicies>
class task_strand_with_queue : public base_task_strand
{
public:
  /// Queue Policies of this strand.
  static constexpr std::array<concurrent_queue_policy, sizeof...(QueuePolicies)> queue_policies{QueuePolicies...};

  /// \brief Executor with a compile-time defined task priority that dispatches tasks to the associated strand.
  ///
  /// This executor does not control the lifetime of the strand.
  template <enqueue_priority Priority>
  using executor =
      detail::task_strand_executor_impl<get_priority_queue_policy<QueuePolicies...>(Priority), base_task_strand*>;

  template <typename... Args>
  explicit task_strand_with_queue(Args&&... queue_params) : queue(std::forward<Args>(queue_params)...)
  {
  }

  /// Number of priority levels supported by this strand.
  static size_t nof_priority_levels() { return sizeof...(QueuePolicies); }

  template <enqueue_priority Priority>
  SRSRAN_NODISCARD bool execute(unique_task task)
  {
    // Enqueue task in task_strand queue.
    if (not this->queue.template try_push<Priority>(std::move(task))) {
      return false;
    }
    return handle_enqueued_task(true);
  }

  template <enqueue_priority Priority>
  SRSRAN_NODISCARD bool defer(unique_task task)
  {
    // Enqueue task in task_strand queue.
    if (not this->queue.template try_push<Priority>(std::move(task))) {
      return false;
    }
    return handle_enqueued_task(false);
  }

  template <enqueue_priority Priority = enqueue_priority::min>
  executor<Priority> get_executor()
  {
    return executor<Priority>{this->queue.template get_enqueuer(), *this};
  }

  std::unique_ptr<task_executor> get_executor_ptr(enqueue_priority prio)
  {
    return get_executor_ptr_impl(prio, std::make_index_sequence<sizeof...(QueuePolicies)>{});
  }

  template <enqueue_priority Priority>
  auto get_enqueuer()
  {
    return this->queue.template get_enqueuer<Priority>();
  }

protected:
  template <size_t... Is>
  std::unique_ptr<task_executor> get_executor_ptr_impl(enqueue_priority priority, std::index_sequence<Is...> /*unused*/)
  {
    const size_t                   idx = detail::enqueue_priority_to_queue_index(priority, sizeof...(QueuePolicies));
    std::unique_ptr<task_executor> exec;
    (void)std::initializer_list<int>{[this, idx, &exec]() {
      if (idx == Is) {
        exec = std::make_unique<executor<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies))>>(
            this->queue.template get_enqueuer(), *this);
      }
      return 0;
    }()...};
    return exec;
  }

  // Run all tasks that have been enqueued in the strand, and unlocks the strand.
  void run_enqueued_tasks()
  {
    unique_task task;
    uint32_t    queue_size = this->job_count.load(std::memory_order_acquire);
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
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  void handle_failed_task_dispatch()
  {
    // Pop enqueued tasks until number of enqueued jobs is zero.
    // Note: Since we acquired the task_strand, the task enqueued in this call should always be the first being
    // popped. Note: If there is a single producer, only the task enqueued in this call will be popped. Note: As we
    // currently hold the strand, there is no concurrent thread popping tasks.
    uint32_t    queue_size = this->job_count.load(std::memory_order_acquire);
    unique_task dropped_task;
    while (queue_size > 0) {
      unsigned run_count = 0;
      for (; run_count != queue_size and this->queue.queue.try_pop(dropped_task); ++run_count) {
        // do nothing with popped task.
      }
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  unique_task get_run_strand_task()
  {
    return [this]() { run_enqueued_tasks(); };
  }

  // Queue of pending tasks waiting for their turn.
  detail::strand_queue<QueuePolicies...> queue;
};

} // namespace detail

/// \brief This class implements a strand of one or more enqueueing policies, each with a different priority.
///
/// \tparam Executor Executor that the strands dispatches tasks to.
/// \tparam QueuePolicies Enqueueing policies for each of the task priority levels supported by the strand.
template <typename Executor, concurrent_queue_policy... QueuePolicies>
class task_strand final : public detail::task_strand_with_queue<QueuePolicies...>
{
  using base_type = detail::task_strand_with_queue<QueuePolicies...>;

public:
  template <enqueue_priority Priority>
  using executor = typename base_type::template executor<Priority>;

  template <typename Exec, typename... Args>
  explicit task_strand(Exec&& executor_, Args&&... queue_params) :
    base_type(std::forward<Args>(queue_params)...), out_exec(std::forward<Exec>(executor_))
  {
  }

private:
  bool dispatch_strand(bool is_execute) final
  {
    // Check if the adapted executor gives us permission to run pending tasks inline. An example of when this may
    // happen is when the caller is running in the same execution context of the underlying task worker or task worker
    // pool. However, this permission is still not a sufficient condition to simply call the task inline. For
    // instance, if the execution context is a thread pool, we have to ensure that the task_strand is not already
    // acquired by another thread of the same pool. If we do not do this, running the task inline would conflict with
    // the task_strand strict serialization requirements. For this reason, we will always enqueue the task and try to
    // acquire the task_strand.
    bool dispatch_successful;
    if (is_execute) {
      dispatch_successful = detail::get_task_executor_ref(out_exec).execute(this->get_run_strand_task());
    } else {
      dispatch_successful = detail::get_task_executor_ref(out_exec).defer(this->get_run_strand_task());
    }
    if (not dispatch_successful) {
      // Unable to dispatch executor job to run enqueued tasks.
      this->handle_failed_task_dispatch();
      return false;
    }
    return true;
  }

  // Executor to which tasks are dispatched in serialized manner.
  Executor out_exec;
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
template <concurrent_queue_policy QueuePolicy>
using shared_strand_executor = detail::task_strand_executor_impl<QueuePolicy, std::shared_ptr<base_task_strand>>;

/// \brief Make a strand executor that manages the lifetime of a strand with a single queue.
template <concurrent_queue_policy QueuePolicy, typename OutExec>
std::unique_ptr<task_executor> make_strand_executor_ptr(OutExec&& out_exec, unsigned queue_size)
{
  auto strand   = make_task_strand_ptr<QueuePolicy, OutExec>(std::forward<OutExec>(out_exec), queue_size);
  auto enqueuer = strand->template get_enqueuer<enqueue_priority::max>();

  return std::make_unique<shared_strand_executor<QueuePolicy>>(enqueuer, std::move(strand));
}

namespace detail {

template <concurrent_queue_policy... QueuePolicies, typename OutExec, size_t... Is>
std::vector<std::unique_ptr<task_executor>>
make_strand_executor_ptrs_helper(std::shared_ptr<task_strand<OutExec, QueuePolicies...>>& strand_shared,
                                 std::index_sequence<Is...> /*unused*/)
{
  std::array<std::unique_ptr<task_executor>, sizeof...(QueuePolicies)> execs{
      std::make_unique<shared_strand_executor<QueuePolicies>>(
          strand_shared->template get_enqueuer<detail::queue_index_to_enqueue_priority(Is, sizeof...(QueuePolicies))>(),
          strand_shared)...};

  std::vector<std::unique_ptr<task_executor>> created_execs;
  created_execs.resize(execs.size());
  for (unsigned i = 0; i != created_execs.size(); ++i) {
    created_execs[i] = std::move(execs[i]);
  }
  return created_execs;
}

} // namespace detail

/// \brief Create all executors associated with a given strand. The executors will manage the strand lifetime via
/// reference counting.
template <concurrent_queue_policy... QueuePolicies, typename OutExec>
std::vector<std::unique_ptr<task_executor>> make_strand_executor_ptrs(OutExec&& out_exec, span<const unsigned> qsizes)
{
  std::shared_ptr<task_strand<OutExec, QueuePolicies...>> strand_shared =
      make_task_strand_ptr<QueuePolicies...>(std::forward<OutExec>(out_exec), qsizes);

  return detail::make_strand_executor_ptrs_helper(strand_shared, std::make_index_sequence<sizeof...(QueuePolicies)>{});
}

namespace detail {

constexpr size_t MAX_QUEUES_PER_STRAND = 3;

// Special case to stop recursion for task queue policies.
template <typename OutExec,
          concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<(sizeof...(QueuePolicies) > MAX_QUEUES_PER_STRAND), int> = 0>
std::vector<std::unique_ptr<task_executor>>
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
  if (strand_queues.empty() or Is > strand_queues.size()) {
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
    case concurrent_queue_policy::lockfree_mpmc: {
      return make_strand_executors_iter_helper<OutExec, QueuePolicies..., concurrent_queue_policy::lockfree_mpmc>(
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