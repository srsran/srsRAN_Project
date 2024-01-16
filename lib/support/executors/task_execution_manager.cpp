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

#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <map>

using namespace srsran;

namespace {

// Metafunction to derive parameter types based on the execution context type.
template <typename ExecutionContext>
struct execution_context_traits;
template <typename ExecutionContext>
using execution_context_params_t = typename execution_context_traits<ExecutionContext>::params;
template <concurrent_queue_policy QueuePolicy, concurrent_queue_wait_policy WaitPolicy>
struct execution_context_traits<general_task_worker<QueuePolicy, WaitPolicy>> {
  using params = execution_config_helper::worker_pool;
};
template <concurrent_queue_policy... QueuePolicies>
struct execution_context_traits<task_worker_pool<QueuePolicies...>> {
  using params = execution_config_helper::worker_pool;
};
template <concurrent_queue_policy... QueuePolicies>
struct execution_context_traits<priority_task_worker<QueuePolicies...>> {
  using params = execution_config_helper::priority_multiqueue_worker;
};

template <typename Exec>
std::unique_ptr<task_executor> exec_to_ptr(Exec exec)
{
  return std::make_unique<std::decay_t<Exec>>(std::move(exec));
}
template <typename Exec>
std::unique_ptr<task_executor> exec_to_ptr(std::unique_ptr<Exec> exec)
{
  return exec;
}

// Helper to instantiate decorated executors for different execution environments.
template <typename Exec>
std::unique_ptr<task_executor> decorate_executor(const execution_config_helper::executor& desc, Exec&& exec)
{
  std::unique_ptr<task_executor> ret;
  if (desc.strand_queue_size.has_value()) {
    ret = make_strand_executor_ptr<concurrent_queue_policy::lockfree_mpmc>(std::forward<Exec>(exec),
                                                                           desc.strand_queue_size.value());
  } else {
    ret = exec_to_ptr(std::forward<Exec>(exec));
  }
  if (desc.synchronous) {
    ret = make_sync_executor(std::move(ret));
  }
  return ret;
}
template <typename ExecConfig, typename Exec>
std::unique_ptr<task_executor>
decorate_executor(const ExecConfig& desc, Exec&& exec, file_event_tracer<true>* exec_tracer)
{
  // In case tracing is enabled, decorate the executor with a tracer.
  if (exec_tracer != nullptr) {
    return decorate_executor(desc, make_trace_executor(desc.name, std::forward<Exec>(exec), *exec_tracer));
  }
  return decorate_executor(desc, std::forward<Exec>(exec));
}

/// Functionality shared across single worker, prioritized multiqueue worker and worker pool task execution contexts.
template <typename WorkerType>
class common_task_execution_context : public task_execution_context
{
public:
  using worker_type   = WorkerType;
  using worker_params = typename execution_context_traits<worker_type>::params;

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
  using task_executor_list = std::vector<std::pair<std::string, std::unique_ptr<task_executor>>>;

  /// Create executor given the provided executor parameters.
  virtual std::unique_ptr<task_executor> create_executor(const execution_config_helper::executor& params) = 0;

  /// Create all the strand executors that connect to the provided executor.
  virtual task_executor_list create_strand_executors(const execution_config_helper::executor& params) = 0;

  bool add_executors(span<const execution_config_helper::executor> execs)
  {
    for (const auto& exec_params : execs) {
      auto exec = create_executor(exec_params);
      if (not store_executor(exec_params.name, std::move(exec))) {
        return false;
      }
      if (not exec_params.strands.empty()) {
        task_executor_list strand_execs              = create_strand_executors(exec_params);
        unsigned           nof_expected_strand_execs = 0;
        for (const auto& strand : exec_params.strands) {
          nof_expected_strand_execs += strand.queues.size();
        }
        if (nof_expected_strand_execs != strand_execs.size()) {
          executor_list.clear();
          return false;
        }
        for (auto& p : strand_execs) {
          if (not store_executor(p.first, std::move(p.second))) {
            return false;
          }
        }
      }
    }
    return true;
  }

  SRSRAN_NODISCARD bool store_executor(const std::string& exec_name, std::unique_ptr<task_executor> exec)
  {
    if (exec_name.empty()) {
      logger.error("Failed to create executor in context \"{}\". Cause: Executor name is emtpy.", name());
      executor_list.clear();
      return false;
    }
    if (exec == nullptr) {
      logger.error("Failed to create executor \"{}\" in context \"{}\". Cause: Executor parameters are invalid",
                   exec_name,
                   name());
      executor_list.clear();
      return false;
    }
    if (not executor_list.insert(std::make_pair(exec_name, std::move(exec))).second) {
      logger.error("Failed to create executor \"{}\" in context \"{}\". Cause: Executor with the same "
                   "name already exists in the context",
                   exec_name,
                   name());
      executor_list.clear();
      return false;
    }
    return true;
  }

  template <typename OutExec>
  task_executor_list create_strand_executors_helper(const execution_config_helper::executor& desc, OutExec&& basic_exec)
  {
    task_executor_list execs;
    for (const execution_config_helper::strand& strand_cfg : desc.strands) {
      std::vector<execution_config_helper::task_queue> queues(strand_cfg.queues.size());
      for (unsigned i = 0; i != strand_cfg.queues.size(); ++i) {
        queues[i] = execution_config_helper::task_queue{strand_cfg.queues[i].policy, strand_cfg.queues[i].size};
      }
      std::vector<std::unique_ptr<task_executor>> strand_execs;
      if (desc.strand_queue_size.has_value()) {
        strand_execs = make_strand_executor_ptrs(make_strand_executor_ptr<concurrent_queue_policy::lockfree_mpmc>(
                                                     std::forward<OutExec>(basic_exec), *desc.strand_queue_size),
                                                 span<const concurrent_queue_params>(queues));
      } else {
        strand_execs =
            make_strand_executor_ptrs(std::forward<OutExec>(basic_exec), span<const concurrent_queue_params>(queues));
      }
      if (strand_execs.size() != strand_cfg.queues.size()) {
        return {};
      }
      for (unsigned i = 0; i != strand_cfg.queues.size(); ++i) {
        execs.emplace_back(strand_cfg.queues[i].name, std::move(strand_execs[i]));
      }
    }
    return execs;
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
struct single_worker_context final
  : public common_task_execution_context<general_task_worker<QueuePolicy, WaitPolicy>> {
  using base_type     = common_task_execution_context<general_task_worker<QueuePolicy, WaitPolicy>>;
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
  typename base_type::task_executor_list create_strand_executors(const execution_config_helper::executor& desc) override
  {
    typename base_type::task_executor_list execs;

    return this->create_strand_executors_helper(desc, executor_type(this->worker));
  }

  std::unique_ptr<task_executor> create_executor(const execution_config_helper::executor& desc) override
  {
    return decorate_executor(desc, executor_type{this->worker}, this->task_tracer);
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

/// Execution context for a task worker pool with multiple QueuePolicies.
template <concurrent_queue_policy... QueuePolicies>
struct worker_pool_context final : public common_task_execution_context<task_worker_pool<QueuePolicies...>> {
  using worker_type = task_worker_pool<QueuePolicies...>;
  template <enqueue_priority Prio>
  using executor_type = priority_task_worker_pool_executor<get_priority_queue_policy<QueuePolicies...>(Prio)>;
  using base_type     = common_task_execution_context<worker_type>;

  worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(
        params.tracer,
        params.nof_workers,
        [&params]() {
          report_error_if_not(params.queues.size() == sizeof...(QueuePolicies),
                              "Invalid number of queues for the specified policies ({} != {})",
                              params.queues.size(),
                              sizeof...(QueuePolicies));
          std::array<unsigned, sizeof...(QueuePolicies)> qsizes;
          for (unsigned i = 0; i != params.queues.size(); ++i) {
            qsizes[i] = params.queues[i].size;
          }
          return qsizes;
        }(),
        params.name,
        params.sleep_time,
        params.prio,
        params.masks)
  {
  }

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<worker_pool_context<QueuePolicies...>>(params);
    if (ctxt == nullptr or not ctxt->add_executors(params.executors)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(const execution_config_helper::executor& desc) override
  {
    std::unique_ptr<task_executor> exec;
    visit_executor(this->worker, desc.priority, [this, &exec, &desc](auto&& prio_exec) {
      exec = decorate_executor(desc, prio_exec, this->task_tracer);
    });
    return exec;
  }

  typename base_type::task_executor_list create_strand_executors(const execution_config_helper::executor& desc) override
  {
    typename base_type::task_executor_list execs;

    visit_executor(this->worker, desc.priority, [this, &execs, &desc](auto prio_exec) {
      execs = this->create_strand_executors_helper(desc, std::move(prio_exec));
    });
    return execs;
  }
};

/// Execution context for a task worker pool.
template <concurrent_queue_policy QueuePolicy>
struct worker_pool_context<QueuePolicy> final : public common_task_execution_context<task_worker_pool<QueuePolicy>> {
  using worker_type   = task_worker_pool<QueuePolicy>;
  using executor_type = task_worker_pool_executor<QueuePolicy>;
  using base_type     = common_task_execution_context<worker_type>;

  worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(params.tracer,
              params.nof_workers,
              params.queues[0].size,
              params.name,
              params.sleep_time,
              params.prio,
              params.masks)
  {
    report_error_if_not(
        params.queues.size() == 1, "Invalid number of queues {} for the specified policies", params.queues.size());
  }

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<worker_pool_context<QueuePolicy>>(params);
    if (ctxt == nullptr or not ctxt->add_executors(params.executors)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(const execution_config_helper::executor& desc) override
  {
    return decorate_executor(desc, executor_type(this->worker), this->task_tracer);
  }

  typename base_type::task_executor_list create_strand_executors(const execution_config_helper::executor& desc) override
  {
    return this->create_strand_executors_helper(desc, executor_type(this->worker));
  }
};

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::worker_pool& params)
{
  report_error_if_not(params.queues.size() > 0 and params.queues.size() <= 2,
                      "Invalid number of prioritized queues {}",
                      params.queues.size());

  // TODO: Use visit.
  switch (params.queues[0].policy) {
    case concurrent_queue_policy::locking_mpmc:
      if (params.queues.size() == 1) {
        return worker_pool_context<concurrent_queue_policy::locking_mpmc>::create(params);
      } else {
        switch (params.queues[1].policy) {
          case concurrent_queue_policy::lockfree_mpmc:
            return worker_pool_context<concurrent_queue_policy::locking_mpmc,
                                       concurrent_queue_policy::lockfree_mpmc>::create(params);
          case concurrent_queue_policy::locking_mpmc:
            return worker_pool_context<concurrent_queue_policy::locking_mpmc,
                                       concurrent_queue_policy::locking_mpmc>::create(params);
          default:
            break;
        }
      }
      break;
    case concurrent_queue_policy::lockfree_mpmc:
      if (params.queues.size() == 1) {
        return worker_pool_context<concurrent_queue_policy::lockfree_mpmc>::create(params);
      } else {
        switch (params.queues[1].policy) {
          case concurrent_queue_policy::lockfree_mpmc:
            return worker_pool_context<concurrent_queue_policy::lockfree_mpmc,
                                       concurrent_queue_policy::lockfree_mpmc>::create(params);
          case concurrent_queue_policy::locking_mpmc:
            return worker_pool_context<concurrent_queue_policy::lockfree_mpmc,
                                       concurrent_queue_policy::locking_mpmc>::create(params);
          default:
            break;
        }
      }
    default:
      break;
  }
  srslog::fetch_basic_logger("ALL").error("Only MPMC queue policies are supported for worker pools");
  return nullptr;
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

namespace {

template <concurrent_queue_policy... QueuePolicies>
struct priority_multiqueue_worker_context
  : public common_task_execution_context<priority_task_worker<QueuePolicies...>> {
  using base_type = common_task_execution_context<priority_task_worker<QueuePolicies...>>;

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

  std::unique_ptr<task_executor> create_executor(const execution_config_helper::executor& desc) override
  {
    std::unique_ptr<task_executor> exec;
    visit_executor(this->worker, desc.priority, [this, &exec, &desc](auto&& prio_exec) {
      exec = decorate_executor(desc, prio_exec, this->task_tracer);
    });
    return exec;
  }

  typename base_type::task_executor_list create_strand_executors(const execution_config_helper::executor& desc) override
  {
    typename base_type::task_executor_list execs;

    visit_executor(this->worker, desc.priority, [this, &execs, &desc](auto prio_exec) {
      execs = this->create_strand_executors_helper(desc, std::move(prio_exec));
    });
    return execs;
  }
};

constexpr size_t MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER = 3U;

// Special case to stop recursion for task queue policies.
template <concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER<sizeof...(QueuePolicies), int> = 0>
              std::unique_ptr<task_execution_context> create_execution_context_helper(
                  const execution_config_helper::priority_multiqueue_worker& params)
{
  report_fatal_error("Workers with equal or more than {} queues are not supported",
                     MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER);
  return nullptr;
};

template <concurrent_queue_policy... QueuePolicies,
          std::enable_if_t<sizeof...(QueuePolicies) <= MAX_QUEUES_PER_PRIORITY_MULTIQUEUE_WORKER, int> = 0>
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
