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

class detail::any_task_consumer
{
public:
  virtual ~any_task_consumer()                         = default;
  virtual bool   try_pop(unique_task& t)               = 0;
  virtual size_t try_pop_bulk(span<unique_task> batch) = 0;
};

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

  virtual void                               request_stop()                    = 0;
  virtual bool                               try_push(unique_task task)        = 0;
  virtual void                               push_blocking(unique_task task)   = 0;
  virtual bool                               try_pop(unique_task& t)           = 0;
  virtual size_t                             try_pop_bulk(span<unique_task> t) = 0;
  virtual size_t                             size() const                      = 0;
  virtual std::unique_ptr<any_task_consumer> create_consumer()                 = 0;

private:
  const size_t                       cap;
  const concurrent_queue_policy      qpolicy;
  const concurrent_queue_wait_policy wpolicy;
};

namespace {

template <concurrent_queue_policy      QueuePolicy,
          concurrent_queue_wait_policy WaitPolicy = concurrent_queue_wait_policy::non_blocking>
class any_task_queue_impl final : public detail::any_task_queue
{
  using base_type  = detail::any_task_queue;
  using queue_impl = concurrent_queue<unique_task, QueuePolicy, WaitPolicy>;

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
  void   push_blocking(unique_task task) override { q.push_blocking(std::move(task)); }
  bool   try_push(unique_task task) override { return q.try_push(std::move(task)); }
  bool   try_pop(unique_task& t) override { return q.try_pop(t); }
  size_t try_pop_bulk(span<unique_task> batch) override { return q.try_pop_bulk(batch); }
  size_t size() const override { return q.size(); }
  std::unique_ptr<detail::any_task_consumer> create_consumer() override
  {
    class any_task_consumer_impl final : public detail::any_task_consumer
    {
    public:
      any_task_consumer_impl(typename queue_impl::consumer_type&& consumer) : impl(std::move(consumer)) {}
      bool   try_pop(unique_task& t) override { return impl.try_pop(t); }
      size_t try_pop_bulk(span<unique_task> batch) override { return impl.try_pop_bulk(batch); }

    private:
      typename queue_impl::consumer_type impl;
    };

    return std::make_unique<any_task_consumer_impl>(q.create_consumer());
  }

protected:
  queue_impl q;
};

} // namespace

static std::unique_ptr<detail::any_task_queue> make_any_task_queue(const concurrent_queue_params& params)
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
      return std::make_unique<any_task_queue_impl<concurrent_queue_policy::moodycamel_lockfree_mpmc>>(
          params.size, params.nof_prereserved_producers);
    default:
      report_fatal_error("Unknown concurrent_queue_policy");
  }
  return nullptr;
}

detail::priority_task_queue::priority_task_queue(span<const concurrent_queue_params> queue_params,
                                                 std::chrono::microseconds           wait_if_empty) :
  policy(wait_if_empty)
{
  queues.resize(queue_params.size());
  for (unsigned i = 0; i != queues.size(); ++i) {
    queues[i] = make_any_task_queue(queue_params[i]);
  }
}

detail::priority_task_queue::~priority_task_queue() {}

void detail::priority_task_queue::request_stop()
{
  policy.request_stop();
  for (auto& q : queues) {
    q->request_stop();
  }
}

void detail::priority_task_queue::push_blocking(task_priority prio, unique_task task)
{
  const size_t idx = get_queue_idx(prio);
  queues[idx]->push_blocking(std::move(task));
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

size_t detail::priority_task_queue::try_pop_bulk(span<unique_task> batch)
{
  // Iterate through all priority levels, starting from the max priority.
  size_t total_popped = 0;
  for (unsigned prio_idx = 0, nof_queues = queues.size(); prio_idx != nof_queues; ++prio_idx) {
    total_popped += queues[prio_idx]->try_pop_bulk(batch.subspan(total_popped, batch.size() - total_popped));
    if (total_popped == batch.size()) {
      break;
    }
  }
  // Return the number of tasks popped.
  return total_popped;
}

bool detail::priority_task_queue::pop_blocking(unique_task& t)
{
  return detail::queue_helper::pop_blocking_generic(*this, t, policy);
}

bool detail::priority_task_queue::pop_blocking(unique_task& t, std::chrono::microseconds wait_time)
{
  return detail::queue_helper::pop_blocking_generic(*this, t, policy, wait_time);
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

detail::priority_task_queue::consumer_type::consumer_type(priority_task_queue& parent_) : parent(&parent_)
{
  consumers.reserve(parent->queues.size());
  for (const auto& q : parent->queues) {
    consumers.push_back(q->create_consumer());
  }
}

detail::priority_task_queue::consumer_type::~consumer_type() = default;

bool detail::priority_task_queue::consumer_type::try_pop(unique_task& t)
{
  // Iterate through all priority levels, starting from the max priority, and try to pop a task from the queue.
  for (unsigned prio_idx = 0, nof_queues = consumers.size(); prio_idx != nof_queues; ++prio_idx) {
    if (consumers[prio_idx]->try_pop(t)) {
      return true;
    }
  }
  return false;
}

size_t detail::priority_task_queue::consumer_type::try_pop_bulk(span<unique_task> batch)
{
  // Iterate through all priority levels, starting from the max priority, and try to pop a task from the queue.
  size_t total_popped = 0;
  for (unsigned prio_idx = 0, nof_queues = consumers.size(); prio_idx != nof_queues; ++prio_idx) {
    total_popped += consumers[prio_idx]->try_pop_bulk(batch.subspan(total_popped, batch.size() - total_popped));
    if (total_popped == batch.size()) {
      break;
    }
  }
  return total_popped;
}

bool detail::priority_task_queue::consumer_type::pop_blocking(unique_task& t)
{
  return detail::queue_helper::pop_blocking_generic(*this, t, parent->policy);
}

bool detail::priority_task_queue::consumer_type::pop_blocking(unique_task& t, std::chrono::microseconds wait_time)
{
  return detail::queue_helper::pop_blocking_generic(*this, t, parent->policy, wait_time);
}

detail::priority_task_queue::consumer_type detail::priority_task_queue::create_consumer()
{
  return consumer_type{*this};
}
