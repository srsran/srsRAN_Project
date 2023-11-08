/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <map>

using namespace srsran;

namespace {

// Helper to instantiate decorated executors for different execution environments.
template <typename ExecConfig, typename Exec>
std::unique_ptr<task_executor> decorate_executor(const ExecConfig& desc, Exec&& exec)
{
  std::unique_ptr<task_executor> ret = std::make_unique<std::decay_t<Exec>>(std::forward<Exec>(exec));
  if (desc.synchronous) {
    ret = make_sync_executor(std::move(ret));
  }
  return ret;
}

/// Functionality shared across single worker, prioritized multiqueue worker and worker pool task execution contexts.
template <typename WorkerType, typename WorkerParams>
class common_task_execution_context : public task_execution_context
{
public:
  using worker_type     = WorkerType;
  using executor_params = typename WorkerParams::executor;

  template <typename... Args>
  common_task_execution_context(file_event_tracer<true>* tracer, Args&&... args) :
    worker(std::forward<Args>(args)...), task_tracer(tracer)
  {
  }

  ~common_task_execution_context() override { this->worker.stop(); }

  std::vector<std::pair<std::string, task_executor*>> executors() const final
  {
    std::vector<std::pair<std::string, task_executor*>> ret;
    for (const auto& pair : executor_list) {
      ret.push_back(std::make_pair(pair.first, pair.second.get()));
    }
    return ret;
  }

  void stop() final { this->worker.stop(); }

protected:
  virtual std::unique_ptr<task_executor> create_executor(const executor_params& params) = 0;

  bool add_executors(span<const executor_params> execs)
  {
    for (const auto& exec_params : execs) {
      if (exec_params.name.empty()) {
        logger.error("Failed to create executor in context \"{}\". Cause: Executor name is emtpy");
        executor_list.clear();
        return false;
      }
      auto exec = create_executor(exec_params);
      if (exec == nullptr or not executor_list.insert(std::make_pair(exec_params.name, std::move(exec))).second) {
        if (exec == nullptr) {
          logger.error("Failed to create executor \"{}\" in context \"{}\". Cause: Executor parameters are invalid",
                       exec_params.name,
                       name());
        } else {
          logger.error("Failed to create executor \"{}\" in context \"{}\". Cause: Executor with the same "
                       "name already exists in the context",
                       exec_params.name,
                       name());
        }
        executor_list.clear();
        return false;
      }
    }
    return true;
  }

  // Worker type used in this execution context.
  worker_type worker;

  // List of execution contexts.
  std::map<std::string, std::unique_ptr<task_executor>> executor_list;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");

  // Tracer of execution context activity.
  file_event_tracer<true>* task_tracer = nullptr;
};

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

/// Execution context for the generate_task_worker class (single worker).
template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
struct single_worker_context final : public common_task_execution_context<general_task_worker<QueuePolicy, WaitPolicy>,
                                                                          execution_config_helper::single_worker> {
  using base_type     = common_task_execution_context<general_task_worker<QueuePolicy, WaitPolicy>,
                                                  execution_config_helper::single_worker>;
  using context_type  = single_worker_context<QueuePolicy, WaitPolicy>;
  using executor_type = general_task_worker_executor<QueuePolicy, WaitPolicy>;

  template <concurrent_queue_wait_policy W                                  = WaitPolicy,
            std::enable_if_t<W == concurrent_queue_wait_policy::sleep, int> = 0>
  single_worker_context(const std::string& name, const execution_config_helper::single_worker& params) :
    base_type(params.tracer, name, params.queue.size, params.wait_sleep_time.value(), params.prio, params.mask)
  {
  }
  template <concurrent_queue_wait_policy W                                  = WaitPolicy,
            std::enable_if_t<W != concurrent_queue_wait_policy::sleep, int> = 0>
  single_worker_context(const std::string& name, const execution_config_helper::single_worker& params) :
    base_type(params.tracer, name, params.queue.size, params.prio, params.mask)
  {
  }

  static std::unique_ptr<task_execution_context> create(const std::string&                            name,
                                                        const execution_config_helper::single_worker& params)
  {
    auto ctxt = std::make_unique<context_type>(name, params);
    if (ctxt == nullptr or not ctxt->add_executors(params.executors)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.worker_name(); }

private:
  std::unique_ptr<task_executor> create_executor(const execution_config_helper::single_worker::executor& desc) override
  {
    executor_type exec(this->worker, desc.report_on_failure);
    return this->task_tracer == nullptr
               ? decorate_executor(desc, std::move(exec))
               : decorate_executor(desc, make_trace_executor(desc.name, std::move(exec), *this->task_tracer));
  }
};

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::single_worker& params)
{
  switch (params.queue.policy) {
    case concurrent_queue_policy::locking_mpsc:
      if (params.wait_sleep_time.has_value()) {
        return single_worker_context<concurrent_queue_policy::locking_mpsc,
                                     concurrent_queue_wait_policy::sleep>::create(params.name, params);
      }
      return single_worker_context<concurrent_queue_policy::locking_mpsc,
                                   concurrent_queue_wait_policy::condition_variable>::create(params.name, params);
    case concurrent_queue_policy::locking_mpmc:
      if (params.wait_sleep_time.has_value()) {
        srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for locking_mpmc queue policy");
        return nullptr;
      }
      return single_worker_context<concurrent_queue_policy::locking_mpmc,
                                   concurrent_queue_wait_policy::condition_variable>::create(params.name, params);
      break;
    case concurrent_queue_policy::lockfree_spsc: {
      if (not params.wait_sleep_time.has_value()) {
        srslog::fetch_basic_logger("ALL").error("Wait sleep time is required for lockfree_spsc queue policy");
        return nullptr;
      }
      return single_worker_context<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>::create(
          params.name, params);
    } break;
    case concurrent_queue_policy::lockfree_mpmc: {
      if (not params.wait_sleep_time.has_value()) {
        srslog::fetch_basic_logger("ALL").error("Wait sleep time is required for lockfree_mpmc queue policy");
        return nullptr;
      }
      return single_worker_context<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>::create(
          params.name, params);
    } break;
    default:
      srslog::fetch_basic_logger("ALL").error("Unknown queue policy");
      break;
  }
  return nullptr;
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

namespace {

/// Execution context for a task worker pool.
template <concurrent_queue_policy QueuePolicy>
struct worker_pool_context final
  : public common_task_execution_context<task_worker_pool<QueuePolicy == concurrent_queue_policy::lockfree_mpmc>,
                                         execution_config_helper::worker_pool> {
  static_assert(QueuePolicy == concurrent_queue_policy::lockfree_mpmc or
                    QueuePolicy == concurrent_queue_policy::locking_mpmc,
                "Invalid queue policy");

  using worker_type   = task_worker_pool<QueuePolicy == concurrent_queue_policy::lockfree_mpmc>;
  using executor_type = task_worker_pool_executor<QueuePolicy == concurrent_queue_policy::lockfree_mpmc>;
  using base_type     = common_task_execution_context<worker_type, execution_config_helper::worker_pool>;

  template <concurrent_queue_policy Q                                          = QueuePolicy,
            std::enable_if_t<Q == concurrent_queue_policy::lockfree_mpmc, int> = 0>
  worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(params.tracer,
              params.nof_workers,
              params.queue.size,
              params.name,
              params.sleep_time.value(),
              params.prio,
              params.masks)
  {
  }
  template <concurrent_queue_policy Q                                          = QueuePolicy,
            std::enable_if_t<Q != concurrent_queue_policy::lockfree_mpmc, int> = 0>
  worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(params.tracer, params.nof_workers, params.queue.size, params.name, params.prio, params.masks)
  {
  }

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<worker_pool_context>(params);
    if (ctxt == nullptr or not ctxt->add_executors(params.executors)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(const execution_config_helper::worker_pool::executor& desc) override
  {
    executor_type exec(this->worker);
    return this->task_tracer == nullptr
               ? decorate_executor(desc, std::move(exec))
               : decorate_executor(desc, make_trace_executor(desc.name, std::move(exec), *this->task_tracer));
  }
};

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::worker_pool& params)
{
  switch (params.queue.policy) {
    case concurrent_queue_policy::locking_mpmc:
      if (params.sleep_time.has_value()) {
        srslog::fetch_basic_logger("ALL").error("Wait sleep time is not supported for locking_mpmc queue policy");
      }
      return worker_pool_context<concurrent_queue_policy::locking_mpmc>::create(params);
    case concurrent_queue_policy::lockfree_mpmc:
      if (not params.sleep_time.has_value()) {
        srslog::fetch_basic_logger("ALL").error("Wait sleep time is required for lockfree_mpmc queue policy");
      }
      return worker_pool_context<concurrent_queue_policy::lockfree_mpmc>::create(params);
    default:
      srslog::fetch_basic_logger("ALL").error("Only MPMC queue policies are supported for worker pools");
      break;
  }
  return nullptr;
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

namespace {

template <concurrent_queue_policy... QueuePolicies>
struct priority_multiqueue_worker_context
  : public common_task_execution_context<priority_task_worker<QueuePolicies...>,
                                         execution_config_helper::priority_multiqueue_worker> {
  using base_type = common_task_execution_context<priority_task_worker<QueuePolicies...>,
                                                  execution_config_helper::priority_multiqueue_worker>;

  static std::unique_ptr<task_execution_context>
  create(const execution_config_helper::priority_multiqueue_worker& params)
  {
    std::array<unsigned, sizeof...(QueuePolicies)> qsizes;
    for (unsigned i = 0; i != params.queues.size(); ++i) {
      qsizes[i] = params.queues[i].size;
    }
    auto ctxt = std::make_unique<priority_multiqueue_worker_context>(
        params.tracer, params.name, qsizes, params.spin_sleep_time, params.prio, params.mask);
    if (ctxt == nullptr or not ctxt->add_executors(params.executors)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.worker_name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor>
  create_executor(const execution_config_helper::priority_multiqueue_worker::executor& desc) override
  {
    std::unique_ptr<task_executor> exec;
    visit_executor(this->worker, desc.priority, [this, &exec, &desc](auto&& prio_exec) {
      exec = this->task_tracer == nullptr
                 ? decorate_executor(desc, std::move(prio_exec))
                 : decorate_executor(desc, make_trace_executor(desc.name, std::move(prio_exec), *this->task_tracer));
    });
    return exec;
  }
};

static constexpr size_t MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER = 4U;

// Special case to stop recursion for task queue policies.
template <concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<sizeof...(QueuePolicies) >= MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER, int> = 0>
std::unique_ptr<task_execution_context>
create_execution_context_helper(const execution_config_helper::priority_multiqueue_worker& params)
{
  report_fatal_error("Workers with equal or more than {} queues are not supported",
                     MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER);
  return nullptr;
};

template <concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<sizeof...(QueuePolicies) < MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER, int> = 0>
std::unique_ptr<task_execution_context>
create_execution_context_helper(const execution_config_helper::priority_multiqueue_worker& params)
{
  size_t vec_size = sizeof...(QueuePolicies);
  if (vec_size > params.queues.size()) {
    return nullptr;
  }
  if (vec_size == params.queues.size()) {
    return priority_multiqueue_worker_context<QueuePolicies...>::create(params);
  }
  switch (params.queues[vec_size].policy) {
    case concurrent_queue_policy::locking_mpmc:
      srslog::fetch_basic_logger("ALL").error("Unsupported MPMC queue policy");
      return nullptr;
    case concurrent_queue_policy::lockfree_spsc:
      return create_execution_context_helper<QueuePolicies..., concurrent_queue_policy::lockfree_spsc>(params);
    case concurrent_queue_policy::locking_mpsc:
      return create_execution_context_helper<QueuePolicies..., concurrent_queue_policy::locking_mpsc>(params);
    case concurrent_queue_policy::lockfree_mpmc:
      return create_execution_context_helper<QueuePolicies..., concurrent_queue_policy::lockfree_mpmc>(params);
    default:
      srslog::fetch_basic_logger("ALL").error("Unknown queue policy");
      break;
  }
  return nullptr;
}

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::priority_multiqueue_worker& params)
{
  return create_execution_context_helper<>(params);
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

task_execution_manager::task_execution_manager() : logger(srslog::fetch_basic_logger("ALL")) {}

void task_execution_manager::stop()
{
  for (auto& ctxt : contexts) {
    ctxt.second->stop();
  }
}

bool task_execution_manager::add_execution_context(std::unique_ptr<task_execution_context> ctxt_to_insert)
{
  if (ctxt_to_insert == nullptr) {
    logger.error("Unable to create execution context. Cause: Context was not created successfully");
    return false;
  }
  if (ctxt_to_insert->name().empty()) {
    logger.error("Unable to create execution context. Cause: Invalid name");
    return false;
  }
  auto ret = contexts.insert(std::make_pair(ctxt_to_insert->name(), std::move(ctxt_to_insert)));
  if (not ret.second) {
    logger.error("Unable to create execution context with name \"{}\". Cause: A execution context with the same name "
                 "already exists.",
                 ctxt_to_insert->name());
    return false;
  }
  task_execution_context& ctxt = *ret.first->second;

  // Insert the created executors by the task execution context in the manager lookup table.
  std::vector<std::pair<std::string, task_executor*>> ctxt_exec_list = ctxt.executors();
  for (auto it = ctxt_exec_list.begin(); it != ctxt_exec_list.end(); ++it) {
    if (it->first.empty() or not executor_list.insert(std::make_pair(it->first, it->second)).second) {
      // Revert back the insertion of executors and execution context.
      logger.error("Unable to create executor with name \"{}\". Cause: A executor with the same name already exists");
      for (auto it2 = ctxt_exec_list.begin(); it2 != it; ++it2) {
        executor_list.erase(it2->first);
      }
      contexts.erase(ctxt.name());
      return false;
    }
  }

  return true;
}
