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
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <unordered_map>

using namespace srsran;

/// Execution context for a single task worker.
template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
struct single_worker_context final : public task_execution_context {
  general_task_worker<QueuePolicy, WaitPolicy>          worker;
  general_task_worker_executor<QueuePolicy, WaitPolicy> basic_executor;
  std::vector<std::unique_ptr<task_executor>>           other_executors;

  template <typename... Args>
  single_worker_context(Args&&... args) : worker(std::forward<Args>(args)...), basic_executor(worker)
  {
  }
  ~single_worker_context() override { stop(); }

  void stop() override { worker.stop(); }

  task_executor* add_executor(const task_executor_description& desc) override { return &basic_executor; }
};

/// Execution context for a task worker pool.
struct worker_pool_context : public task_execution_context {
  task_worker_pool          pool;
  task_worker_pool_executor basic_executor;

  worker_pool_context(const std::string&          name,
                      unsigned                    nof_workers,
                      unsigned                    queue_size,
                      os_thread_realtime_priority prio) :
    pool(nof_workers, queue_size, name, prio)
  {
  }
  ~worker_pool_context() override { stop(); }

  void stop() override { pool.stop(); }

  task_executor* add_executor(const task_executor_description& desc) override { return &basic_executor; }
};

static std::unique_ptr<task_execution_context> create_execution_context(const execution_context_description& desc)
{
  if (variant_holds_alternative<execution_context_description::single>(desc.type)) {
    const auto& single_desc = variant_get<execution_context_description::single>(desc.type);
    switch (single_desc.queue_policy) {
      case srsran::concurrent_queue_policy::locking_mpsc: {
        if (single_desc.wait_sleep_time.has_value()) {
          return std::make_unique<
              single_worker_context<concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>>(
              desc.name, desc.queue_size, single_desc.wait_sleep_time.value(), desc.prio, desc.mask);
        }
        return std::make_unique<single_worker_context<concurrent_queue_policy::locking_mpsc,
                                                      concurrent_queue_wait_policy::condition_variable>>(
            desc.name, desc.queue_size, desc.prio, desc.mask);
      } break;
      case srsran::concurrent_queue_policy::locking_mpmc: {
        if (single_desc.wait_sleep_time.has_value()) {
          srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for locking_mpmc queue policy");
          return nullptr;
        }
        return std::make_unique<single_worker_context<concurrent_queue_policy::locking_mpmc,
                                                      concurrent_queue_wait_policy::condition_variable>>(
            desc.name, desc.queue_size, desc.prio, desc.mask);
      } break;
      case srsran::concurrent_queue_policy::lockfree_spsc: {
        if (not single_desc.wait_sleep_time.has_value()) {
          srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for locking_mpmc queue policy");
          return nullptr;
        }
        return std::make_unique<
            single_worker_context<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>>(
            desc.name, desc.queue_size, single_desc.wait_sleep_time.value(), desc.prio, desc.mask);
      } break;
      default:
        srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for locking_mpmc queue policy");
        break;
    }
    return nullptr;
  }

  if (variant_holds_alternative<execution_context_description::pool>(desc.type)) {
    const auto& pool_desc = variant_get<execution_context_description::pool>(desc.type);
    return std::make_unique<worker_pool_context>(desc.name, pool_desc.nof_workers, desc.queue_size, desc.prio);
  }

  report_fatal_error("Unknown task worker type");
}

void task_execution_manager::stop()
{
  for (auto& ctxt : contexts) {
    ctxt.second->stop();
  }
}

bool task_execution_manager::add_execution_context(const std::string& name, std::unique_ptr<task_execution_context> mng)
{
  if (mng == nullptr or name.empty()) {
    return false;
  }
  auto ret = contexts.insert(std::make_pair(name, std::move(mng)));
  return ret.second;
}

bool task_execution_manager::add_executor(const std::string& context_name, const task_executor_description& exec_desc)
{
  auto ctxt_it = contexts.find(context_name);
  if (ctxt_it == contexts.end()) {
    return false;
  }
  if (exec_desc.name.empty()) {
    return false;
  }
  if (executor_list.count(exec_desc.name) > 0) {
    return false;
  }
  task_executor* exec = ctxt_it->second->add_executor(exec_desc);
  if (exec == nullptr) {
    return false;
  }
  executor_list.insert(std::make_pair(exec_desc.name, exec));
  return true;
}

std::unique_ptr<task_execution_manager> task_execution_manager::create(const task_worker_manager_config& config)
{
  std::unique_ptr<task_execution_manager> manager{new task_execution_manager{}};

  for (const execution_context_description& ctxt_desc : config.execution_contexts) {
    if (not manager->add_execution_context(ctxt_desc.name, create_execution_context(ctxt_desc))) {
      return nullptr;
    }

    for (const task_executor_description& exec_desc : ctxt_desc.executors) {
      if (not manager->add_executor(ctxt_desc.name, exec_desc)) {
        return nullptr;
      }
    }
  }

  return manager;
}
