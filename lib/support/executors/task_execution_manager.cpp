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

#include "srsran/support/executors/task_execution_manager.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <map>

using namespace srsran;

/// Helper to convert execution_config_helper::task_queue into concurrent_queue_params.
static std::vector<concurrent_queue_params> convert_to_qparams(span<const execution_config_helper::task_queue> queues)
{
  std::vector<concurrent_queue_params> ret;
  ret.reserve(queues.size());
  for (const auto& q : queues) {
    ret.push_back(concurrent_queue_params{q.policy, q.size, q.nof_prereserved_producers});
  }
  return ret;
}

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
template <concurrent_queue_policy QueuePolicy>
struct execution_context_traits<task_worker_pool<QueuePolicy>> {
  using params = execution_config_helper::worker_pool;
};
template <>
struct execution_context_traits<priority_task_worker_pool> {
  using params = execution_config_helper::worker_pool;
};
template <>
struct execution_context_traits<priority_task_worker> {
  using params = execution_config_helper::priority_multiqueue_worker;
};

/// Functionality shared across single worker, prioritized multiqueue worker and worker pool task execution contexts.
template <typename WorkerType>
class common_task_execution_context : public task_execution_context
{
public:
  using worker_type   = WorkerType;
  using worker_params = typename execution_context_traits<worker_type>::params;

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
  using task_executor_list = std::vector<std::pair<std::string, std::unique_ptr<task_executor>>>;

  /// Create executor given the provided executor parameters.
  virtual std::unique_ptr<task_executor> create_executor(task_priority prio) = 0;

  bool add_executors(const std::vector<execution_config_helper::task_queue>& queues)
  {
    unsigned queue_idx = 0;
    for (const auto& queue : queues) {
      task_priority prio = task_priority::max - queue_idx;
      auto          exec = create_executor(prio);
      if (not store_executor(queue.name, std::move(exec))) {
        return false;
      }
      ++queue_idx;
    }
    return true;
  }

  [[nodiscard]] bool store_executor(const std::string& exec_name, std::unique_ptr<task_executor> exec)
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

  // Worker type used in this execution context.
  worker_type worker;

  // List of execution contexts.
  std::map<std::string, std::unique_ptr<task_executor>> executor_list;

  srslog::basic_logger& logger = srslog::fetch_basic_logger("ALL");
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
    auto                                             ctxt = std::make_unique<context_type>(name, params);
    std::vector<execution_config_helper::task_queue> queues{params.queue};
    if (ctxt == nullptr or not ctxt->add_executors(queues)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.worker_name(); }

private:
  std::unique_ptr<task_executor> create_executor(task_priority /* unused */) override
  {
    return make_task_executor_ptr(this->worker);
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

/// Execution context for a task worker pool with single Queue.
template <concurrent_queue_policy QueuePolicy>
struct worker_pool_context final : public common_task_execution_context<task_worker_pool<QueuePolicy>> {
  using worker_type   = task_worker_pool<QueuePolicy>;
  using executor_type = task_worker_pool_executor<QueuePolicy>;
  using base_type     = common_task_execution_context<worker_type>;

  worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(
        params.name,
        params.nof_workers,
        [&params]() {
          report_error_if_not(params.queues.size() == 1, "Invalid number of queues for the type of worker");
          unsigned qsize = params.queues[0].size;
          return qsize;
        }(),
        params.sleep_time,
        params.queues[0].nof_prereserved_producers,
        params.prio,
        params.masks)
  {
  }

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<worker_pool_context<QueuePolicy>>(params);
    if (ctxt == nullptr or not ctxt->add_executors(params.queues)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(task_priority /* unused */) override
  {
    return make_task_worker_pool_executor_ptr(this->worker);
  }
};

/// Execution context for a task worker pool.
struct priority_worker_pool_context final : public common_task_execution_context<priority_task_worker_pool> {
  using worker_type   = priority_task_worker_pool;
  using executor_type = priority_task_worker_pool_executor;
  using base_type     = common_task_execution_context<worker_type>;

  priority_worker_pool_context(const execution_config_helper::worker_pool& params) :
    base_type(params.name,
              params.nof_workers,
              convert_to_qparams(params.queues),
              params.sleep_time,
              params.prio,
              params.masks)
  {
    report_error_if_not(
        params.queues.size() > 1, "Invalid number of queues {} for a priority task worker pool", params.queues.size());
    for (unsigned i = 0; i != params.queues.size(); ++i) {
      report_error_if_not(params.queues[i].policy != concurrent_queue_policy::lockfree_mpmc or
                              params.queues[i].policy != concurrent_queue_policy::locking_mpmc or
                              params.queues[i].policy != concurrent_queue_policy::moodycamel_lockfree_mpmc,
                          "Only MPMC queues are supported for worker pools");
    }
  }

  static std::unique_ptr<task_execution_context> create(const execution_config_helper::worker_pool& params)
  {
    auto ctxt = std::make_unique<priority_worker_pool_context>(params);
    if (ctxt == nullptr or not ctxt->add_executors(params.queues)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(task_priority prio) override
  {
    return std::make_unique<executor_type>(prio, this->worker);
  }
};

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::worker_pool& params)
{
  report_error_if_not(params.queues.size() > 0, "Invalid number of prioritized queues {}", params.queues.size());

  if (params.queues.size() > 1) {
    // Prioritized multiqueue task worker pool.
    return priority_worker_pool_context::create(params);
  }

  switch (params.queues[0].policy) {
    case concurrent_queue_policy::locking_mpmc:
      return worker_pool_context<concurrent_queue_policy::locking_mpmc>::create(params);
    case concurrent_queue_policy::lockfree_mpmc:
      return worker_pool_context<concurrent_queue_policy::lockfree_mpmc>::create(params);
    default:
      break;
  }
  srslog::fetch_basic_logger("ALL").error("Only MPMC queue policies are supported for worker pools");
  return nullptr;
}

/* /////////////////////////////////////////////////////////////////////////////////////////////// */

namespace {

struct priority_multiqueue_worker_context : public common_task_execution_context<priority_task_worker> {
  using base_type = common_task_execution_context<priority_task_worker>;

  static std::unique_ptr<task_execution_context>
  create(const execution_config_helper::priority_multiqueue_worker& params)
  {
    auto ctxt = std::make_unique<priority_multiqueue_worker_context>(
        params.name, convert_to_qparams(params.queues), params.spin_sleep_time, params.prio, params.mask);
    if (ctxt == nullptr or not ctxt->add_executors(params.queues)) {
      return nullptr;
    }
    return ctxt;
  }

  std::string name() const final { return this->worker.worker_name(); }

private:
  using base_type::base_type;

  std::unique_ptr<task_executor> create_executor(task_priority prio) override
  {
    return make_priority_task_executor_ptr(prio, this->worker);
  }
};

} // namespace

std::unique_ptr<task_execution_context>
srsran::create_execution_context(const execution_config_helper::priority_multiqueue_worker& params)
{
  return priority_multiqueue_worker_context::create(params);
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
  auto ctxt_name = ctxt_to_insert->name();
  auto ret       = contexts.insert(std::make_pair(ctxt_name, std::move(ctxt_to_insert)));
  if (not ret.second) {
    logger.error("Unable to create execution context with name \"{}\". Cause: A execution context with the same name "
                 "already exists.",
                 ctxt_name);
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
