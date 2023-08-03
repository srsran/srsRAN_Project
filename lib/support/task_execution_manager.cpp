/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/priority_multiqueue_task_worker.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"

using namespace srsran;

// Helper to instantiate decorated executors for different execution environments.
template <typename Exec>
static std::unique_ptr<task_executor> decorate_executor(const task_executor_description& desc, Exec&& exec)
{
  std::unique_ptr<task_executor> ret;
  if (desc.tracer == nullptr) {
    // Trace executor enabled.
    ret = make_trace_executor_ptr(desc.name, std::move(exec), *desc.tracer);
  } else {
    ret = std::make_unique<Exec>(std::move(exec));
  }
  if (desc.synchronous) {
    ret = make_sync_executor(std::move(ret));
  }
  return ret;
}

/// Execution context for the generate_task_worker class (single worker).
template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
struct single_worker_context final : public task_execution_context {
  using basic_executor = general_task_worker_executor<QueuePolicy, WaitPolicy>;

  general_task_worker<QueuePolicy, WaitPolicy> worker;

  template <typename... Args>
  single_worker_context(Args&&... args) : worker(std::forward<Args>(args)...)
  {
  }
  ~single_worker_context() override { stop(); }

  void stop() override { worker.stop(); }

  std::unique_ptr<task_executor> create_executor(const task_executor_description& desc) override
  {
    return decorate_executor(desc, basic_executor(worker));
  }
};

/// Execution context for a task worker pool.
struct worker_pool_context : public task_execution_context {
  task_worker_pool pool;

  worker_pool_context(const std::string&          name,
                      unsigned                    nof_workers,
                      unsigned                    queue_size,
                      os_thread_realtime_priority prio) :
    pool(nof_workers, queue_size, name, prio)
  {
  }
  ~worker_pool_context() override { stop(); }

  void stop() override { pool.stop(); }

  std::unique_ptr<task_executor> create_executor(const task_executor_description& desc) override
  {
    return decorate_executor(desc, task_worker_pool_executor(pool));
  }
};

template <concurrent_queue_policy... QueuePolicies>
struct priority_multiqueue_worker_context : public task_execution_context {
  priority_multiqueue_task_worker<QueuePolicies...> worker;

  template <typename... Args>
  priority_multiqueue_worker_context(Args&&... args) : worker(std::forward<Args>(args)...)
  {
  }
  ~priority_multiqueue_worker_context() override { stop(); }

  void stop() override { worker.stop(); }

  std::unique_ptr<task_executor> create_executor(const task_executor_description& desc) override
  {
    return decorate_executor(desc, priority_task_worker_executor<task_queue_priority::max, QueuePolicies...>(worker));
  }
};

template <concurrent_queue_policy... QueuePolicies>
std::unique_ptr<task_execution_context>
create_priority_multiqueue_worker_context(const execution_context_description& desc)
{
  static_assert(sizeof...(QueuePolicies) > 0, "Invalid number of queues");
  auto& mq_desc = variant_get<execution_context_description::priority_multiqueue>(desc.type);
  std::array<unsigned, sizeof...(QueuePolicies)> qsizes;
  for (unsigned i = 0; i != mq_desc.queues.size(); ++i) {
    qsizes[i] = mq_desc.queues[i].size;
  }
  return std::make_unique<priority_multiqueue_worker_context<QueuePolicies...>>(
      desc.name, qsizes, mq_desc.spin_sleep_time, desc.prio, desc.mask);
}

static std::unique_ptr<task_execution_context> create_execution_context(const execution_context_description& desc)
{
  if (variant_holds_alternative<execution_context_description::single>(desc.type)) {
    const auto& single_desc = variant_get<execution_context_description::single>(desc.type);
    switch (single_desc.queue.policy) {
      case srsran::concurrent_queue_policy::locking_mpsc: {
        if (single_desc.wait_sleep_time.has_value()) {
          return std::make_unique<
              single_worker_context<concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>>(
              desc.name, single_desc.queue.size, single_desc.wait_sleep_time.value(), desc.prio, desc.mask);
        }
        return std::make_unique<single_worker_context<concurrent_queue_policy::locking_mpsc,
                                                      concurrent_queue_wait_policy::condition_variable>>(
            desc.name, single_desc.queue.size, desc.prio, desc.mask);
      } break;
      case srsran::concurrent_queue_policy::locking_mpmc: {
        if (single_desc.wait_sleep_time.has_value()) {
          srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for locking_mpmc queue policy");
          return nullptr;
        }
        return std::make_unique<single_worker_context<concurrent_queue_policy::locking_mpmc,
                                                      concurrent_queue_wait_policy::condition_variable>>(
            desc.name, single_desc.queue.size, desc.prio, desc.mask);
      } break;
      case srsran::concurrent_queue_policy::lockfree_spsc: {
        if (not single_desc.wait_sleep_time.has_value()) {
          srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for lockfree_spsc queue policy");
          return nullptr;
        }
        return std::make_unique<
            single_worker_context<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>>(
            desc.name, single_desc.queue.size, single_desc.wait_sleep_time.value(), desc.prio, desc.mask);
      } break;
      default:
        srslog::fetch_basic_logger("ALL").error("Unknown queue policy");
        break;
    }
    return nullptr;
  }

  if (variant_holds_alternative<execution_context_description::pool>(desc.type)) {
    const auto& pool_desc = variant_get<execution_context_description::pool>(desc.type);
    if (pool_desc.queue.policy != srsran::concurrent_queue_policy::locking_mpmc) {
      srslog::fetch_basic_logger("ALL").error(
          "Only locking_mpmc queue policy is supported for worker pools at the moment");
      return nullptr;
    }
    return std::make_unique<worker_pool_context>(desc.name, pool_desc.nof_workers, pool_desc.queue.size, desc.prio);
  }

  if (variant_holds_alternative<execution_context_description::priority_multiqueue>(desc.type)) {
    // TODO: Add support for multiple queues.
    return create_priority_multiqueue_worker_context<concurrent_queue_policy::locking_mpsc>(desc);
  }

  report_fatal_error("Unknown task worker type");
}

task_execution_manager::task_execution_manager() : logger(srslog::fetch_basic_logger("ALL")) {}

void task_execution_manager::stop()
{
  for (auto& ctxt : contexts) {
    ctxt.second->stop();
  }
}

bool task_execution_manager::store_execution_context(const std::string&                      name,
                                                     std::unique_ptr<task_execution_context> mng)
{
  if (name.empty()) {
    logger.error("Unable to create execution context. Cause: Invalid name");
    return false;
  }
  if (mng == nullptr) {
    logger.error("Unable to create execution context with name \"{}\". Cause: Invalid parameters", name);
    return false;
  }
  if (not contexts.insert(std::make_pair(name, std::move(mng))).second) {
    logger.error("Unable to create execution context with name \"{}\". Cause: A execution context with the same name "
                 "already exists.",
                 name);
    return false;
  }
  return true;
}

bool task_execution_manager::add_executor(const std::string& context_name, const task_executor_description& exec_desc)
{
  auto ctxt_it = contexts.find(context_name);
  if (ctxt_it == contexts.end()) {
    logger.error("Unable to create executor with name \"{}\". Cause: Respective execution context \"{}\" not found",
                 exec_desc.name,
                 context_name);
    return false;
  }
  if (exec_desc.name.empty()) {
    logger.error("Unable to create executor for execution context \"{}\". Cause: Invalid executor name.", context_name);
    return false;
  }
  if (executor_list.count(exec_desc.name) > 0) {
    logger.error("Unable to create executor with name \"{}\". Cause: An executor with the same name already exists",
                 exec_desc.name);
    return false;
  }
  std::unique_ptr<task_executor> exec = ctxt_it->second->create_executor(exec_desc);
  if (exec == nullptr) {
    logger.error("Unable to create executor with name \"{}\". Cause: Invalid parameters", exec_desc.name);
    return false;
  }
  return executor_list.insert(std::make_pair(exec_desc.name, std::move(exec))).second;
}

SRSRAN_NODISCARD bool task_execution_manager::add_execution_context(const execution_context_description& desc)
{
  if (not store_execution_context(desc.name, create_execution_context(desc))) {
    return false;
  }

  for (auto it = desc.executors.begin(); it != desc.executors.end(); ++it) {
    if (not add_executor(desc.name, *it)) {
      // Remove inserted executors and execution context.
      for (auto it2 = desc.executors.begin(); it2 != it; ++it2) {
        executor_list.erase(it2->name);
      }
      contexts.erase(desc.name);
      return false;
    }
  }

  return true;
}
