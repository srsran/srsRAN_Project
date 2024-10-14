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

#include "srsran/cu_up/cu_up_executor_mapper.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/execute_on.h"
#include "srsran/support/executors/inline_task_executor.h"
#include "srsran/support/executors/strand_executor.h"
#include <variant>

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

struct base_cu_up_executor_pool_config {
  task_executor&       main_exec;
  span<task_executor*> dl_executors;
  span<task_executor*> ul_executors;
  span<task_executor*> ctrl_executors;
  task_executor&       crypto_exec;
};

class round_robin_cu_up_exec_pool
{
public:
  round_robin_cu_up_exec_pool(base_cu_up_executor_pool_config config) : main_exec(config.main_exec)
  {
    srsran_assert(config.ctrl_executors.size() > 0, "At least one DL executor must be specified");
    if (config.dl_executors.empty()) {
      config.dl_executors = config.ctrl_executors;
    } else {
      srsran_assert(config.dl_executors.size() == config.ctrl_executors.size(),
                    "If specified, the number of DL executors must be equal to the number of control executors");
    }
    if (config.ul_executors.empty()) {
      config.ul_executors = config.ctrl_executors;
    } else {
      srsran_assert(config.ul_executors.size() == config.ctrl_executors.size(),
                    "If specified, the number of UL executors must be equal to the number of control executors");
    }

    for (unsigned i = 0; i != config.ctrl_executors.size(); ++i) {
      execs.emplace_back(
          *config.ctrl_executors[i], *config.ul_executors[i], *config.dl_executors[i], config.crypto_exec);
    }
  }

  std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper()
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

/// \brief CU-UP Executor Pool based on strands pointing to a worker pool.
///
/// This is the executor architecture:
/// - "main_executor" is a strand called "cu_up_strand" that wraps "worker_pool_executor". Thus, it is sequential.
/// - "crypto_executor" is a pointer to "worker_pool_executor". Thus, it is *not* sequential.
/// - "ue_ctrl_executor", "ue_ul_executor", "ue_dl_executor" are the three-level priorities of a strand that adapts
/// the "main_executor" strand. "ue_ctrl_executor" is for timers and control events, "ue_ul_executor" for UL PDUs,
/// and "ue_dl_executor" for DL PDUs.
/// Thus, all executors, with the exception of "crypto_executor", go through the same "cu_up_strand", and there is
/// no parallelization, except for the crypto tasks.
/// TODO: Revisit executor architecture once CU-UP supports parallelization.
class strand_based_cu_up_executor_mapper final : public cu_up_executor_mapper
{
public:
  strand_based_cu_up_executor_mapper(const strand_based_executor_config& config) :
    cu_up_strand(&config.worker_pool_executor, config.default_task_queue_size), cu_up_exec_pool(create_strands(config))
  {
  }

  task_executor& ctrl_executor() override { return cu_up_strand; }

  task_executor& io_ul_executor() override { return *io_ul_exec; }

  task_executor& e2_executor() override { return cu_up_strand; }

  std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() override
  {
    return cu_up_exec_pool.create_ue_executor_mapper();
  }

private:
  using cu_up_strand_type        = task_strand<task_executor*, concurrent_queue_policy::lockfree_mpmc>;
  using io_dedicated_strand_type = task_strand<cu_up_strand_type*, concurrent_queue_policy::lockfree_mpmc>;
  using ue_strand_type           = priority_task_strand<cu_up_strand_type*>;

  base_cu_up_executor_pool_config create_strands(const strand_based_executor_config& config)
  {
    concurrent_queue_params qparams{srsran::concurrent_queue_policy::lockfree_mpmc, config.default_task_queue_size};
    concurrent_queue_params data_qparams{srsran::concurrent_queue_policy::lockfree_mpmc, config.gtpu_task_queue_size};

    // Create IO executor that can be either inlined with CU-UP strand or its own strand.
    if (config.dedicated_io_strand) {
      io_ul_strand.emplace<io_dedicated_strand_type>(&cu_up_strand, config.default_task_queue_size);
      io_ul_exec = &std::get<io_dedicated_strand_type>(io_ul_strand);
    } else {
      io_ul_strand.emplace<inline_task_executor>();
      io_ul_exec = &std::get<inline_task_executor>(io_ul_strand);
    }

    // Create UE-dedicated strands.
    ue_strands.resize(config.max_nof_ue_strands);
    ue_ctrl_execs.resize(config.max_nof_ue_strands);
    ue_ul_execs.resize(config.max_nof_ue_strands);
    ue_dl_execs.resize(config.max_nof_ue_strands);
    std::array<concurrent_queue_params, 3> ue_queue_params = {qparams, data_qparams, data_qparams};
    for (unsigned i = 0; i != config.max_nof_ue_strands; ++i) {
      ue_strands[i]                             = std::make_unique<ue_strand_type>(&cu_up_strand, ue_queue_params);
      span<ue_strand_type::executor_type> execs = ue_strands[i]->get_executors();
      srsran_assert(execs.size() == 3, "Three executors should have been created for the three priorities");
      ue_ctrl_execs[i] = &execs[0];
      ue_ul_execs[i]   = &execs[1];
      ue_dl_execs[i]   = &execs[2];
    }

    return base_cu_up_executor_pool_config{
        cu_up_strand, ue_dl_execs, ue_ul_execs, ue_ctrl_execs, config.worker_pool_executor};
  }

  // Base strand that sequentializes accesses to the worker pool executor.
  cu_up_strand_type cu_up_strand;

  // IO executor with two modes
  std::variant<inline_task_executor, io_dedicated_strand_type> io_ul_strand;
  task_executor*                                               io_ul_exec;

  // UE strands and respective executors.
  std::vector<std::unique_ptr<ue_strand_type>> ue_strands;
  std::vector<task_executor*>                  ue_ctrl_execs;
  std::vector<task_executor*>                  ue_ul_execs;
  std::vector<task_executor*>                  ue_dl_execs;

  // pool of UE executors with round-robin dispatch policy.
  round_robin_cu_up_exec_pool cu_up_exec_pool;
};

} // namespace

std::unique_ptr<cu_up_executor_mapper>
srsran::srs_cu_up::make_cu_up_executor_mapper(const strand_based_executor_config& config)
{
  return std::make_unique<strand_based_cu_up_executor_mapper>(config);
}
