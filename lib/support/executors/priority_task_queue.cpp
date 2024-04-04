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

#include "srsran/support/executors/detail/priority_task_queue.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/adt/mutexed_mpsc_queue.h"
#include "srsran/adt/spsc_queue.h"

using namespace srsran;

template <typename QueueType>
class detail::any_task_concurrent_queue::queue_impl final : public base_queue
{
public:
  template <typename... Args>
  queue_impl(unsigned qsize, Args&&... other_args) : q(qsize, std::forward<Args>(other_args)...)
  {
  }

  void   request_stop() override { q.request_stop(); }
  bool   push_blocking(unique_task task) override { return q.push_blocking(std::move(task)); }
  bool   try_push(unique_task task) override { return q.try_push(std::move(task)); }
  bool   try_pop(unique_task& t) override { return q.try_pop(t); }
  size_t size() const override { return q.size(); }

private:
  QueueType q;
};

template <concurrent_queue_policy      QueuePolicy,
          concurrent_queue_wait_policy WaitPolicy = concurrent_queue_wait_policy::sleep>
using queue_type =
    detail::any_task_concurrent_queue::queue_impl<concurrent_queue<unique_task, QueuePolicy, WaitPolicy>>;

template <typename... Args>
void detail::any_task_concurrent_queue::emplace(const concurrent_queue_params& params, Args&&... other_params)
{
  cap = params.size;
  switch (params.policy) {
    case concurrent_queue_policy::lockfree_mpmc:
      q = std::make_unique<queue_type<concurrent_queue_policy::lockfree_mpmc>>(params.size,
                                                                               std::forward<Args>(other_params)...);
      break;
    case concurrent_queue_policy::lockfree_spsc:
      q = std::make_unique<queue_type<concurrent_queue_policy::lockfree_spsc>>(params.size,
                                                                               std::forward<Args>(other_params)...);
      break;
    case concurrent_queue_policy::locking_mpmc:
      q = std::make_unique<
          queue_type<concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>>(
          params.size);
      break;
    case concurrent_queue_policy::locking_mpsc:
      q = std::make_unique<queue_type<concurrent_queue_policy::locking_mpsc>>(params.size,
                                                                              std::forward<Args>(other_params)...);
      break;
    default:
      report_fatal_error("Invalid queue type");
      break;
  }
}

// ---- priority_task_queue -----

detail::priority_task_queue::priority_task_queue(span<const concurrent_queue_params> queue_params,
                                                 std::chrono::microseconds           wait_if_empty) :
  wait_on_empty(wait_if_empty)
{
  queues.resize(queue_params.size());

  for (unsigned i = 0; i != queues.size(); ++i) {
    queues[i].emplace(queue_params[i], std::chrono::microseconds{10});
  }
}

void detail::priority_task_queue::request_stop()
{
  running = false;
  for (auto& q : queues) {
    q.request_stop();
  }
}

bool detail::priority_task_queue::push_blocking(task_priority prio, unique_task task)
{
  const size_t idx = get_queue_idx(prio);
  return queues[idx].push_blocking(std::move(task));
}

bool detail::priority_task_queue::try_push(task_priority prio, unique_task task)
{
  const size_t idx = get_queue_idx(prio);
  return queues[idx].try_push(std::move(task));
}

bool detail::priority_task_queue::try_pop(unique_task& t)
{
  // Iterate through all priority levels, starting from the max priority, and try to pop a task from the queue.
  for (unsigned prio_idx = 0; prio_idx != queues.size(); ++prio_idx) {
    if (queues[prio_idx].try_pop(t)) {
      return true;
    }
  }
  return false;
}

bool detail::priority_task_queue::pop_blocking(unique_task& t)
{
  while (running.load(std::memory_order_relaxed)) {
    if (try_pop(t)) {
      return true;
    }
    std::this_thread::sleep_for(wait_on_empty);
  }
  return false;
}

size_t detail::priority_task_queue::size() const
{
  size_t total_size = 0;
  for (const auto& q : queues) {
    total_size += q.size();
  }
  return total_size;
}
