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
#include "srsran/support/async/execute_on.h"

using namespace srsran;
using namespace srs_cu_up;

class cu_up_executor_pool_impl final : public cu_up_executor_pool
{
  struct ue_executor_context {
    task_executor& ctrl_exec;
    task_executor& ul_exec;
    task_executor& dl_exec;

    ue_executor_context(task_executor& ctrl_exec_, task_executor& ul_exec_, task_executor& dl_exec_) :
      ctrl_exec(ctrl_exec_), ul_exec(ul_exec_), dl_exec(dl_exec_)
    {
    }
  };

public:
  /// Implementation of the UE executor mapper.
  class ue_executor_mapper_impl final : public ue_executor_mapper
  {
  public:
    ue_executor_mapper_impl(cu_up_executor_pool_impl& parent_, ue_executor_context& ctxt_) :
      parent(parent_), ctxt(&ctxt_)
    {
    }

    ~ue_executor_mapper_impl() override { parent.release_ue_executors(*ctxt); }

    async_task<void> stop() override
    {
      ue_executor_context* ctxt_tmp = nullptr;
      return launch_async([this, ctxt_tmp](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Switch to the UE execution context.
        CORO_AWAIT(defer_to(ctxt->ctrl_exec));

        // Make executors inaccessible via the ue_executor_mapper_impl public interface.
        // Any public access after this point should assert.
        ctxt_tmp = std::exchange(ctxt, nullptr);

        // Synchronize with remaining executors to ensure there are no more pending tasks for this UE.
        CORO_AWAIT(defer_to(ctxt_tmp->ul_exec));
        CORO_AWAIT(defer_to(ctxt_tmp->dl_exec));

        // Return back to main control execution context.
        CORO_AWAIT(execute_on(parent.main_exec));

        // Finally, deregister UE executors.
        parent.release_ue_executors(*ctxt_tmp);

        CORO_RETURN();
      });
    }

    task_executor& ctrl_executor() override
    {
      srsran_assert(ctxt != nullptr, "UE executor mapper already stopped");
      return ctxt->ctrl_exec;
    }
    task_executor& ul_pdu_executor() override
    {
      srsran_assert(ctxt != nullptr, "UE executor mapper already stopped");
      return ctxt->ul_exec;
    }
    task_executor& dl_pdu_executor() override
    {
      srsran_assert(ctxt != nullptr, "UE executor mapper already stopped");
      return ctxt->dl_exec;
    }

  private:
    cu_up_executor_pool_impl& parent;
    ue_executor_context*      ctxt;
  };

  cu_up_executor_pool_impl(task_executor&       cu_up_main_exec,
                           span<task_executor*> dl_executors,
                           span<task_executor*> ul_executors,
                           span<task_executor*> ctrl_executors) :
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
      execs.emplace_back(*ctrl_executors[i], *ul_executors[i], *dl_executors[i]);
    }
  }

  std::unique_ptr<ue_executor_mapper> create_ue_executor_mapper() override
  {
    return std::make_unique<ue_executor_mapper_impl>(
        *this, execs[round_robin_index.fetch_add(1, std::memory_order_relaxed) % execs.size()]);
  }

private:
  void release_ue_executors(ue_executor_context& ue_execs)
  {
    // do nothing.
  }

  // Main executor of the CU-UP.
  task_executor& main_exec;

  // List of UE executor mapper contexts created.
  std::vector<ue_executor_context> execs;

  std::atomic<uint32_t> round_robin_index{0};
};

std::unique_ptr<cu_up_executor_pool> srsran::srs_cu_up::make_cu_up_executor_pool(task_executor& cu_up_main_executor,
                                                                                 span<task_executor*> dl_pdu_executors,
                                                                                 span<task_executor*> ul_pdu_executors,
                                                                                 span<task_executor*> ctrl_executors)
{
  return std::make_unique<cu_up_executor_pool_impl>(
      cu_up_main_executor, dl_pdu_executors, ul_pdu_executors, ctrl_executors);
}
