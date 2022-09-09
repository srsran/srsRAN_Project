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

#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {
namespace srs_du {

class f1c_ue_task_scheduler_adapter final : public f1c_ue_config_update_handler
{
public:
  explicit f1c_ue_task_scheduler_adapter(du_ue_index_t ue_index_, timer_manager& timers_) :
    ue_index(ue_index_), timers(timers_)
  {
  }

  unique_timer create_timer() override { return timers.create_unique_timer(); }

  void schedule_async_task(async_task<void>&& task) override { du_mng->schedule_async_task(ue_index, std::move(task)); }

  void request_ue_config_update(const f1ap_ue_config_update_request& request) override
  {
    du_ue_config_update_request du_req;
    du_req.ue_index       = ue_index;
    du_req.srbs_to_addmod = request.srbs_to_addmod;
    du_req.drbs_to_addmod = request.drbs_to_addmod;
    du_mng->handle_ue_config_update(du_req);
  }

  void connect(du_manager_configurator& du_mng_) { du_mng = &du_mng_; }

private:
  du_ue_index_t            ue_index;
  timer_manager&           timers;
  du_manager_configurator* du_mng;
};

class f1c_task_scheduler_adapter : public f1c_du_config_notifier
{
public:
  explicit f1c_task_scheduler_adapter(timer_manager& timers_) : timers(timers_)
  {
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      ues.emplace(i, to_du_ue_index(i), timers);
    }
  }

  void connect(du_manager_configurator& du_mng_)
  {
    du_mng = &du_mng_;
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      ues[i].connect(*du_mng);
    }
  }

  timer_manager& get_timer_manager() override { return timers; }

  f1c_ue_config_update_handler& get_ue_handler(du_ue_index_t ue_index) override { return ues[ue_index]; }

private:
  timer_manager&                                            timers;
  du_manager_configurator*                                  du_mng;
  slot_array<f1c_ue_task_scheduler_adapter, MAX_NOF_DU_UES> ues;
};

} // namespace srs_du
} // namespace srsgnb