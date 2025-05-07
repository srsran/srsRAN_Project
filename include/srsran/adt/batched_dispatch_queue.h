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

#include "../srslog/srslog.h"
#include "../support/executors/task_executor.h"
#include "mpmc_queue.h"
#include "span.h"
#include <vector>

namespace srsran {

/// Queue that batches all the pushed values and forwards them in a batch to a callable dispatched via the provided
/// task executor.
/// \remark This queue drops enqueued values on destruction.
/// \remark stop() and dtor needs to be called from the same execution context as the consumer side.
template <typename ValueType>
class batched_dispatch_queue
{
public:
  using value_type = ValueType;

  batched_dispatch_queue(unsigned                                    queue_size,
                         task_executor&                              serialized_exec_,
                         srslog::basic_logger&                       logger_,
                         const std::function<void(span<ValueType>)>& callback_,
                         unsigned batch_max_size_ = std::numeric_limits<unsigned>::max()) :
    exec(serialized_exec_),
    logger(logger_),
    callback(callback_),
    batch_max_size(batch_max_size_),
    queue(queue_size),
    stopped(std::make_shared<std::atomic<bool>>(false))
  {
    batched_values.reserve(queue_size);
  }
  ~batched_dispatch_queue() { stop(); }

  /// Must be called from the same task execution context as the consumer side, where the batch of values is handled.
  void stop()
  {
    if (stopped != nullptr && not stopped->exchange(true, std::memory_order_relaxed)) {
      drain_queue();
    }
  }

  template <typename U>
  bool try_push(U&& value)
  {
    srsran_sanity_check(not stopped->load(std::memory_order_relaxed), "Detected push after queue has been stopped");

    if (not queue.try_push(std::forward<ValueType>(value))) {
      // Queue is full. Return false.
      return false;
    }

    // If the queue transitions from empty to non-empty, it means that we need to dispatch a job to dequeue values to
    // the wrapped executor.
    bool edge_trigger = job_count.fetch_add(1U, std::memory_order_acq_rel) == 0U;
    if (not edge_trigger) {
      return true;
    }

    // Dispatch batch dequeue job.
    return dispatch_value_dequeue_task();
  }

  /// Get an approximation of the number of enqueued values in the queue.
  unsigned size_approx() const { return job_count.load(std::memory_order_relaxed); }

private:
  using queue_type =
      concurrent_queue<ValueType, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>;

  void handle_enqueued_values()
  {
    srsran_sanity_check(not stopped->load(std::memory_order_relaxed), "Detected concurrent stop and task dispatch");
    value_type val;
    unsigned   queue_size = this->job_count.load(std::memory_order_acquire);
    unsigned   max_pops   = batch_max_size;
    while (queue_size > 0) {
      // Note: We use a blocking pop because (in theory) at this point we have the guarantee that at least one task
      // is stored in the queue (job_count > 0). However, we still apply a timeout policy to catch unexpected
      // situations or invalid states of the strand. We could not use a non-blocking pop because some of the MPMC
      // queue implementations have spurious failures.
      unsigned run_count = 0;
      unsigned max_count = std::min(queue_size, max_pops);
      for (; run_count != max_count and pop(val); ++run_count) {
        batched_values.push_back(std::move(val));
      }
      if (run_count != max_count) {
        // We were not able to dequeue all values. Is stop() being called from a different execution context?
        logger.warning("Unable to dequeue all values from the queue");
        run_count = max_count;
      }

      // We have popped all the values that were enqueued since when we computed queue_size.
      // Recompute the queue_size to check if there are values that were enqueued in the meantime.
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;

      // Forward the values popped so far.
      callback(batched_values);
      batched_values.clear();

      max_pops -= run_count;
      // Check if queue should yield back control.
      if (queue_size > 0 and max_pops == 0) {
        dispatch_value_dequeue_task();
        break;
      }
    }
  }

  bool dispatch_value_dequeue_task()
  {
    // Dispatch batch dequeue job.
    if (not exec.defer([this, stop_copy = stopped]() {
          if (stop_copy->load(std::memory_order_relaxed)) {
            // The queue has been stopped. Discard dispatched task without accessing this pointer.
            return;
          }
          handle_enqueued_values();
        })) {
      logger.warning("Couldn't dispatch batch dequeue job to executor. Clearing all pending values");
      drain_queue();
      return false;
    }
    return true;
  }

  void drain_queue()
  {
    // Note: As we currently hold the strand, there is no concurrent thread popping values.
    // Note: In case of a single producer, only the task enqueued in this call will be popped.

    uint32_t   queue_size = this->job_count.load(std::memory_order_acquire);
    value_type dropped_val;
    while (queue_size > 0) {
      unsigned run_count = 0;
      for (; run_count != queue_size and pop(dropped_val); ++run_count) {
        // do nothing with popped task.
      }
      if (run_count != queue_size) {
        // We were not able to dequeue all values. Is stop() being called from a different execution context?
        logger.error("Couldn't run all pending values stored in strand");
        run_count = queue_size;
      }
      queue_size = this->job_count.fetch_sub(run_count, std::memory_order_acq_rel) - run_count;
    }
  }

  // Helper to block on pop. Returns false if the queue is indeed empty.
  // Note: While the pop should never be called on an empty queue, there are spurious failures in the used MPMC queue
  // which are circumvented by this function.
  bool pop(value_type& val)
  {
    static constexpr std::chrono::microseconds time_to_wait{1000};
    std::chrono::microseconds                  telapsed{0};
    do {
      if (queue.try_pop(val)) {
        return true;
      }
      std::this_thread::sleep_for(std::chrono::microseconds{1});
      telapsed += std::chrono::microseconds{1};
    } while (telapsed < time_to_wait);
    return false;
  }

  task_executor&                       exec;
  srslog::basic_logger&                logger;
  std::function<void(span<ValueType>)> callback;
  const unsigned                       batch_max_size;

  // Queue where the values get batched.
  queue_type queue;

  // Popped values that are dispatched as a batch.
  std::vector<ValueType> batched_values;

  // Number of jobs currently enqueued.
  std::atomic<uint32_t> job_count{0};

  // Flag used to signal that the queue has been stopped and to cancel any pending task.
  std::shared_ptr<std::atomic<bool>> stopped;
};

} // namespace srsran
