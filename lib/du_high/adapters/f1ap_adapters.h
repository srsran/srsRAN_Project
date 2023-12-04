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

#pragma once

#include "srsran/du_manager/du_manager.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/async/execute_on.h"
#include "srsran/support/timers.h"

namespace srsran {

class timer_manager;

namespace srs_du {

class f1ap_ue_task_scheduler_adapter final : public f1ap_ue_task_scheduler
{
public:
  explicit f1ap_ue_task_scheduler_adapter(du_ue_index_t ue_index_, timer_factory timers_) :
    ue_index(ue_index_), timers(timers_)
  {
  }

  unique_timer create_timer() override { return timers.create_timer(); }

  void schedule_async_task(async_task<void>&& task) override { du_mng->schedule_async_task(ue_index, std::move(task)); }

  void connect(du_manager_configurator& du_mng_) { du_mng = &du_mng_; }

private:
  du_ue_index_t            ue_index;
  timer_factory            timers;
  du_manager_configurator* du_mng = nullptr;
};

class f1ap_du_configurator_adapter : public f1ap_du_configurator
{
public:
  explicit f1ap_du_configurator_adapter(timer_factory timers_) : timers(timers_)
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

  timer_factory& get_timer_factory() override { return timers; }

  void schedule_async_task(async_task<void>&& task) override { du_mng->schedule_async_task(std::move(task)); }

  f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) override { return ues[ue_index]; }

  du_ue_index_t find_free_ue_index() override { return du_mng->find_unused_du_ue_index(); }

  async_task<f1ap_ue_context_creation_response>
  request_ue_creation(const f1ap_ue_context_creation_request& request) override
  {
    return du_mng->handle_ue_context_creation(request);
  }

  async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) override
  {
    return du_mng->handle_ue_context_update(request);
  }

  async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) override
  {
    return du_mng->handle_ue_delete_request(request);
  }

  void notify_reestablishment_of_old_ue(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override
  {
    du_mng->handle_ue_reestablishment(new_ue_index, old_ue_index);
  }

private:
  timer_factory                                                 timers;
  du_manager_configurator*                                      du_mng = nullptr;
  slotted_array<f1ap_ue_task_scheduler_adapter, MAX_NOF_DU_UES> ues;
};

} // namespace srs_du
} // namespace srsran
