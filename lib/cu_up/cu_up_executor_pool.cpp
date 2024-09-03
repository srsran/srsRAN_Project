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

#include "srsran/cu_up/cu_up_executor_pool.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_cu_up;

namespace {

/// Task executor adaptor that allows cancelling pending tasks, from within the executor's context.
class cancellable_task_executor final : public task_executor
{
public:
  cancellable_task_executor(task_executor& exec_) : exec(&exec_) {}

  /// Note: This needs to be called from within the executor's context.
  ~cancellable_task_executor() override
  {
    // cancel pending tasks.
    *cancelled = true;

    // extend life-time of cancelled flag by moving it into an executor.
    while (not exec->defer([flag_moved = std::move(cancelled)]() mutable {
      // flag is finally destroyed.
      flag_moved.reset();
    })) {
      srslog::fetch_basic_logger("ALL").warning("Unable to dispatch UE task executor deletion. Retrying...");
      std::this_thread::sleep_for(std::chrono::microseconds{100});
    }
  }

  [[nodiscard]] bool execute(unique_task task) override
  {
    return exec->execute([&cancelled_ref = *cancelled, task = std::move(task)]() {
      if (cancelled_ref) {
        return;
      }
      task();
    });
  }

  [[nodiscard]] bool defer(unique_task task) override
  {
    return exec->defer([&cancelled_ref = *cancelled, task = std::move(task)]() {
      if (cancelled_ref) {
        return;
      }
      task();
    });
  }

private:
  task_executor*        exec;
  std::unique_ptr<bool> cancelled = std::make_unique<bool>(false);
};

/// Implementation of the UE executor mapper.
class ue_executor_mapper_impl final : public ue_executor_mapper
{
public:
  ue_executor_mapper_impl(task_executor& ctrl_exec_,
                          task_executor& ul_exec_,
                          task_executor& dl_exec_,
                          task_executor& crypto_exec_,
                          task_executor& main_exec_) :
    ctrl_exec(ctrl_exec_), ul_exec(ul_exec_), dl_exec(dl_exec_), crypto_exec(crypto_exec_), main_exec(main_exec_)
  {
  }

  ~ue_executor_mapper_impl() override = default;

  async_task<void> stop() override
  {
    return launch_async([this](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);

      // Switch to the UE execution context.
      CORO_AWAIT(defer_to_blocking(*ctrl_exec));

      // Cancel all pending tasks.
      // Note: this operation is only thread-safe because we are calling it from the UE executor's context.
      ctrl_exec.reset();
      ul_exec.reset();
      dl_exec.reset();

      // Return back to main control execution context.
      CORO_AWAIT(defer_to_blocking(main_exec));

      CORO_RETURN();
    });
  }

  task_executor& ctrl_executor() override { return *ctrl_exec; }
  task_executor& ul_pdu_executor() override { return *ul_exec; }
  task_executor& dl_pdu_executor() override { return *dl_exec; }
  task_executor& crypto_executor() override { return crypto_exec; }

private:
  std::optional<cancellable_task_executor> ctrl_exec;
  std::optional<cancellable_task_executor> ul_exec;
  std::optional<cancellable_task_executor> dl_exec;
  task_executor&                           crypto_exec;
  task_executor&                           main_exec;
};

} // namespace

class cu_up_executor_pool_impl final : public cu_up_executor_pool
{
public:
  cu_up_executor_pool_impl(task_executor&       cu_up_main_exec,
                           span<task_executor*> dl_executors,
                           span<task_executor*> ul_executors,
                           span<task_executor*> ctrl_executors,
                           task_executor&       crypto_exec) :
    main_exec(cu_up_main_exec)
  {
    srsran_assert(ctrl_executors.size() > 0, "At least one DL executor must be specified");
    if (dl_executors.empty()) {
      dl_executors = ctrl_executors;
    } else {
      srsran_assert(dl_executors.size() == ctrl_executors.size(),
                    "If specified, the number of DL executors must be equal to the number of control executors");
    }
    if (ul_executors.empty()) {
      ul_executors = ctrl_executors;
    } else {
      srsran_assert(ul_executors.size() == ctrl_executors.size(),
                    "If specified, the number of UL executors must be equal to the number of control executors");
    }

    for (unsigned i = 0; i != ctrl_executors.size(); ++i) {
      execs.emplace_back(*ctrl_executors[i], *ul_executors[i], *dl_executors[i], crypto_exec);
    }
  }

  std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() override
  {
    auto& ctxt = execs[round_robin_index.fetch_add(1, std::memory_order_relaxed) % execs.size()];
    return std::make_unique<ue_executor_mapper_impl>(
        ctxt.ctrl_exec, ctxt.ul_exec, ctxt.dl_exec, ctxt.crypto_exec, main_exec);
  }

private:
  struct ue_executor_context {
    task_executor& ctrl_exec;
    task_executor& ul_exec;
    task_executor& dl_exec;
    task_executor& crypto_exec;

    ue_executor_context(task_executor& ctrl_exec_,
                        task_executor& ul_exec_,
                        task_executor& dl_exec_,
                        task_executor& crypto_exec_) :
      ctrl_exec(ctrl_exec_), ul_exec(ul_exec_), dl_exec(dl_exec_), crypto_exec(crypto_exec_)
    {
    }
  };

  // Main executor of the CU-UP.
  task_executor& main_exec;

  // List of UE executor mapper contexts created.
  std::vector<ue_executor_context> execs;

  // A round-robin algorithm is used to distribute executors to UEs.
  std::atomic<uint32_t> round_robin_index{0};
};

std::unique_ptr<cu_up_executor_pool> srsran::srs_cu_up::make_cu_up_executor_pool(task_executor& cu_up_main_executor,
                                                                                 span<task_executor*> dl_pdu_executors,
                                                                                 span<task_executor*> ul_pdu_executors,
                                                                                 span<task_executor*> ctrl_executors,
                                                                                 task_executor&       crypto_executor)
{
  return std::make_unique<cu_up_executor_pool_impl>(
      cu_up_main_executor, dl_pdu_executors, ul_pdu_executors, ctrl_executors, crypto_executor);
}
