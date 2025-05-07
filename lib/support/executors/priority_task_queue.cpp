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

#include "srsran/support/executors/detail/priority_task_queue.h"
#include "srsran/adt/moodycamel_mpmc_queue.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/adt/mutexed_mpsc_queue.h"
#include "srsran/adt/spsc_queue.h"

using namespace srsran;

class detail::any_task_queue
{
public:
  any_task_queue(size_t cap_, concurrent_queue_policy qpolicy_, concurrent_queue_wait_policy wpolicy_) :
    cap(cap_), qpolicy(qpolicy_), wpolicy(wpolicy_)
  {
  }
  virtual ~any_task_queue() = default;

  size_t                       capacity() const { return cap; }
  concurrent_queue_policy      queue_policy() const { return qpolicy; }
  concurrent_queue_wait_policy wait_policy() const { return wpolicy; }

  virtual void   request_stop()                  = 0;
  virtual bool   push_blocking(unique_task task) = 0;
  virtual bool   try_push(unique_task task)      = 0;
  virtual bool   try_pop(unique_task& t)         = 0;
  virtual size_t size() const                    = 0;

private:
  const size_t                       cap;
  const concurrent_queue_policy      qpolicy;
  const concurrent_queue_wait_policy wpolicy;
};

namespace {

template <concurrent_queue_policy      QueuePolicy,
          concurrent_queue_wait_policy WaitPolicy = concurrent_queue_wait_policy::non_blocking>
class any_task_queue_impl : public detail::any_task_queue
{
  using base_type = detail::any_task_queue;

public:
  template <typename... Args>
  any_task_queue_impl(unsigned cap_, Args&&... args) :
    base_type(cap_, QueuePolicy, WaitPolicy), q(cap_, std::forward<Args>(args)...)
  {
  }

  void request_stop() override
  {
    if constexpr (WaitPolicy == concurrent_queue_wait_policy::condition_variable) {
      q.request_stop();
    }
  }
  bool push_blocking(unique_task task) override
  {
    if constexpr (WaitPolicy != concurrent_queue_wait_policy::non_blocking) {
      return q.push_blocking(std::move(task));
    }
    report_fatal_error("Blocking API not supported for this type");
    return false;
  }
  bool   try_push(unique_task task) override { return q.try_push(std::move(task)); }
  bool   try_pop(unique_task& t) override { return q.try_pop(t); }
  size_t size() const override { return q.size(); }

protected:
  concurrent_queue<unique_task, QueuePolicy, WaitPolicy> q;
};

template <typename... Args>
std::unique_ptr<detail::any_task_queue> make_any_task_queue(const concurrent_queue_params& params)
{
  switch (params.policy) {
    case concurrent_queue_policy::lockfree_mpmc:
      return std::make_unique<any_task_queue_impl<concurrent_queue_policy::lockfree_mpmc>>(params.size);
    case concurrent_queue_policy::lockfree_spsc:
      return std::make_unique<any_task_queue_impl<concurrent_queue_policy::lockfree_spsc>>(params.size);
    case concurrent_queue_policy::locking_mpsc:
      return std::make_unique<any_task_queue_impl<concurrent_queue_policy::locking_mpsc>>(params.size);
    case concurrent_queue_policy::locking_mpmc:
      return std::make_unique<
          any_task_queue_impl<concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>>(
          params.size);
    case concurrent_queue_policy::moodycamel_lockfree_mpmc:
      return std::make_unique<any_task_queue_impl<concurrent_queue_policy::moodycamel_lockfree_mpmc>>(params.size);
    default:
      report_fatal_error("Unknown concurrent_queue_policy");
  }
  return nullptr;
}

} // namespace

detail::priority_task_queue::priority_task_queue(span<const concurrent_queue_params> queue_params,
                                                 std::chrono::microseconds           wait_if_empty) :
  wait_on_empty(wait_if_empty)
{
  queues.resize(queue_params.size());
  for (unsigned i = 0; i != queues.size(); ++i) {
    queues[i] = make_any_task_queue(queue_params[i]);
  }
}

detail::priority_task_queue::~priority_task_queue() {}

void detail::priority_task_queue::request_stop()
{
  running = false;
  for (auto& q : queues) {
    q->request_stop();
  }
}

bool detail::priority_task_queue::push_blocking(task_priority prio, unique_task task)
{
  const size_t idx = get_queue_idx(prio);
  return queues[idx]->push_blocking(std::move(task));
}

bool detail::priority_task_queue::try_push(task_priority prio, unique_task task)
{
  const size_t idx = get_queue_idx(prio);
  return queues[idx]->try_push(std::move(task));
}

bool detail::priority_task_queue::try_pop(unique_task& t)
{
  // Iterate through all priority levels, starting from the max priority, and try to pop a task from the queue.
  for (unsigned prio_idx = 0, nof_queues = queues.size(); prio_idx != nof_queues; ++prio_idx) {
    if (queues[prio_idx]->try_pop(t)) {
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
    total_size += q->size();
  }
  return total_size;
}

size_t detail::priority_task_queue::queue_capacity(task_priority prio) const
{
  return queues[get_queue_idx(prio)]->size();
}
