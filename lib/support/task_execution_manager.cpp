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
#include <map>

using namespace srsran;

// Helper to instantiate decorated executors for different execution environments.
template <typename ExecConfig, typename Exec>
static std::unique_ptr<task_executor> decorate_executor(const ExecConfig& desc, Exec&& exec)
{
  std::unique_ptr<task_executor> ret;
  if (desc.tracer == nullptr) {
    // Trace executor enabled.
    ret = make_trace_executor_ptr(desc.name, std::forward<Exec>(exec), *desc.tracer);
  } else {
    ret = std::make_unique<std::decay_t<Exec>>(std::forward<Exec>(exec));
  }
  if (desc.synchronous) {
    ret = make_sync_executor(std::move(ret));
  }
  return ret;
}

template <typename WorkerType, typename WorkerParams>
class common_task_execution_context : public task_execution_context
{
public:
  using worker_type     = WorkerType;
  using executor_params = typename WorkerParams::executor;

  template <typename... Args>
  common_task_execution_context(Args&&... args) : worker(std::forward<Args>(args)...)
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

  worker_type worker;

  std::map<std::string, std::unique_ptr<task_executor>> executor_list;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
};

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
    base_type(name, params.queue.size, params.wait_sleep_time.value(), params.prio, params.mask)
  {
  }
  template <concurrent_queue_wait_policy W                                  = WaitPolicy,
            std::enable_if_t<W != concurrent_queue_wait_policy::sleep, int> = 0>
  single_worker_context(const std::string& name, const execution_config_helper::single_worker& params) :
    base_type(name, params.queue.size, params.prio, params.mask)
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
    return decorate_executor(desc, executor_type(this->worker, desc.report_on_failure));
  }
};

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
        srslog::fetch_basic_logger("ALL").error("Wait sleep time not supported for lockfree_spsc queue policy");
        return nullptr;
      }
      return single_worker_context<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>::create(
          params.name, params);
    } break;
    default:
      srslog::fetch_basic_logger("ALL").error("Unknown queue policy");
      break;
  }
  return nullptr;
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

/// Execution context for a task worker pool.
struct worker_pool_context final
  : public common_task_execution_context<task_worker_pool, execution_config_helper::worker_pool> {
  using base_type = common_task_execution_context<task_worker_pool, execution_config_helper::worker_pool>;

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<worker_pool_context>(params.nof_workers, params.queue.size, params.name, params.prio);
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
    return decorate_executor(desc, task_worker_pool_executor(this->worker));
  }
};

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::worker_pool& params)
{
  if (params.queue.policy != srsran::concurrent_queue_policy::locking_mpmc) {
    srslog::fetch_basic_logger("ALL").error(
        "Only locking_mpmc queue policy is supported for worker pools at the moment");
    return nullptr;
  }
  return worker_pool_context::create(params);
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

template <concurrent_queue_policy... QueuePolicies>
struct priority_multiqueue_worker_context
  : public common_task_execution_context<priority_multiqueue_task_worker<QueuePolicies...>,
                                         execution_config_helper::priority_multiqueue_worker> {
  using base_type = common_task_execution_context<priority_multiqueue_task_worker<QueuePolicies...>,
                                                  execution_config_helper::priority_multiqueue_worker>;

  static std::unique_ptr<task_execution_context>
  create(const execution_config_helper::priority_multiqueue_worker& params)
  {
    std::array<unsigned, sizeof...(QueuePolicies)> qsizes;
    for (unsigned i = 0; i != params.queues.size(); ++i) {
      qsizes[i] = params.queues[i].size;
    }
    auto ctxt = std::make_unique<priority_multiqueue_worker_context>(
        params.name, qsizes, params.spin_sleep_time, params.prio, params.mask);
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
    if (desc.priority > 0) {
      this->logger.error("Invalid priority {}. It must be zero (max) or a negative number");
      return nullptr;
    }
    const task_queue_priority prio = static_cast<task_queue_priority>(-desc.priority);

    std::unique_ptr<task_executor> exec;
    visit_executor(
        this->worker, prio, [&exec, &desc](auto&& prio_exec) { exec = decorate_executor(desc, std::move(prio_exec)); });
    return exec;
  }
};

// Special case to stop recursion for task queue policies.
template <concurrent_queue_policy... QueuePolicies, std::enable_if_t<sizeof...(QueuePolicies) >= 4, int> = 0>
static std::unique_ptr<task_execution_context>
create_execution_context_helper(const execution_config_helper::priority_multiqueue_worker& params)
{
  report_fatal_error("Workers with more than 3 queues are not supported");
  return nullptr;
};

template <concurrent_queue_policy... QueuePolicies, std::enable_if_t<sizeof...(QueuePolicies) < 4, int> = 0>
static std::unique_ptr<task_execution_context>
create_execution_context_helper(const execution_config_helper::priority_multiqueue_worker& params)
{
  static_assert(sizeof...(QueuePolicies) < 32, "Too many queue policies");
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
    default:
      srslog::fetch_basic_logger("ALL").error("Unknown queue policy");
      break;
  }
  return nullptr;
}

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::priority_multiqueue_worker& params)
{
  return create_execution_context_helper<>(params);
  //  return priority_multiqueue_worker_context<concurrent_queue_policy::locking_mpsc>::create(params);
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
