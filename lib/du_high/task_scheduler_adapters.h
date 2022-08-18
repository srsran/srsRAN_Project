/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {
namespace srs_du {

class f1c_ue_task_scheduler_adapter : public f1c_ue_task_scheduler
{
public:
  f1c_ue_task_scheduler_adapter(du_ue_index_t                 ue_index_,
                                du_manager_ue_task_scheduler& task_sched_,
                                timer_manager&                timers_,
                                task_executor&                ctrl_exec_,
                                du_l2_ul_executor_mapper&     ue_exec_mapper_) :
    ue_index(ue_index_),
    task_sched(task_sched_),
    timers(timers_),
    ctrl_exec(ctrl_exec_),
    ue_exec_mapper(ue_exec_mapper_)
  {
  }

  unique_timer create_timer() override { return timers.create_unique_timer(); }

  /// \brief Schedules UE task to run in the CTRL executor, and returns back to UE executor once the task is complete.
  void schedule_async_task(async_task<void>&& task) override
  {
    task_sched.schedule_async_task(
        launch_async([this, t = std::move(task)](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT(execute_on(ctrl_exec));
          CORO_AWAIT(t);
          CORO_AWAIT(execute_on(ue_exec_mapper.executor(ue_index)));
          CORO_RETURN();
        }));
  }

private:
  du_ue_index_t                 ue_index;
  du_manager_ue_task_scheduler& task_sched;
  timer_manager&                timers;
  task_executor&                ctrl_exec;
  du_l2_ul_executor_mapper&     ue_exec_mapper;
};

class f1c_task_scheduler_adapter : public f1c_task_scheduler
{
public:
  f1c_task_scheduler_adapter(timer_manager& timers_) : timers(timers_) {}

  void
  connect(du_manager_task_scheduler& du_task_sched_, task_executor& ctrl_exec, du_l2_ul_executor_mapper& ue_exec_mapper)
  {
    du_task_sched = &du_task_sched_;
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      du_ue_index_t ue_index = to_du_ue_index(i);
      ues.emplace(i, ue_index, du_task_sched->get_ue_task_scheduler(ue_index), timers, ctrl_exec, ue_exec_mapper);
    }
  }

  timer_manager& get_timer_manager() override { return timers; }

  f1c_ue_task_scheduler& get_ue_task_scheduler(du_ue_index_t ue_index) override { return ues[ue_index]; }

private:
  timer_manager&                                            timers;
  du_manager_task_scheduler*                                du_task_sched;
  slot_array<f1c_ue_task_scheduler_adapter, MAX_NOF_DU_UES> ues;
};

} // namespace srs_du
} // namespace srsgnb