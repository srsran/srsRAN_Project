/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/detail/priority_task_queue.h"
#include "srsran/adt/concurrent_queue.h"

using namespace srsran;

template <typename QueueType>
class detail::any_task_concurrent_queue::queue_impl final : public base_queue
{
public:
  template <typename... Args>
  queue_impl(unsigned qsize, Args&&... other_args) : q(qsize, std::forward<Args>(other_args)...)
  {
  }

  void request_stop() override { q.request_stop(); }
  bool push_blocking(unique_task task) override { return q.push_blocking(std::move(task)); }
  bool try_push(unique_task task) override { return q.try_push(std::move(task)); }
  bool try_pop(unique_task& t) override { return q.try_pop(t); }

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
      break;
  }
  report_fatal_error("Invalid queue type");
}

// ---- priority_task_queue -----

detail::priority_task_queue::priority_task_queue(span<const concurrent_queue_params> queue_params)
{
  queues.resize(queues.size());

  for (unsigned i = 0; i != queues.size(); ++i) {
    queues[i].emplace(queue_params[i], std::chrono::microseconds{10});
  }
}

size_t detail::priority_task_queue::get_queue_idx(task_priority prio) const
{
  return detail::enqueue_priority_to_queue_index(prio, queues.size());
}

void detail::priority_task_queue::request_stop()
{
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
