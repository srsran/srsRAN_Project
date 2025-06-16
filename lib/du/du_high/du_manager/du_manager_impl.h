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

#pragma once

#include "du_cell_manager.h"
#include "du_metrics_aggregator_impl.h"
#include "du_ue/du_ue_manager.h"
#include "ran_resource_management/du_ran_resource_manager_impl.h"
#include "srsran/du/du_high/du_manager/du_manager.h"
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include <condition_variable>

namespace srsran {
namespace srs_du {

class du_manager_impl final : public du_manager_interface
{
public:
  explicit du_manager_impl(const du_manager_params& params_);
  ~du_manager_impl() override;

  // Controller interface.
  void start() override;
  void stop() override;

  // MAC interface
  void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) override;

  // Task scheduling interface.
  void schedule_async_task(async_task<void>&& task) override { main_ctrl_loop.schedule(std::move(task)); }
  void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task) override
  {
    ue_mng.schedule_async_task(ue_index, std::move(task));
  }

  void handle_du_stop_request() override;

  du_ue_index_t find_unused_du_ue_index() override;

  async_task<void> handle_f1_reset_request(const std::vector<du_ue_index_t>& ues_to_reset) override;
  async_task<gnbcu_config_update_response>
  handle_cu_context_update_request(const gnbcu_config_update_request& request) override;

  async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) override;

  async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) override;

  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) override;

  async_task<void> handle_ue_deactivation_request(du_ue_index_t ue_index) override;

  void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override;

  void handle_ue_config_applied(du_ue_index_t ue_index) override;

  size_t                nof_ues() override;
  mac_cell_time_mapper& get_time_mapper() override;

  async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(du_mac_sched_control_config reconf) override;

  du_param_config_response handle_operator_config_request(const du_param_config_request& req) override;

  void handle_si_pdu_update(const du_si_pdu_update_request& req) override;

  f1ap_du_positioning_handler& get_positioning_handler() override { return *positioning_handler; }

  du_manager_mac_metric_aggregator& get_metrics_aggregator() override { return metrics; }

private:
  // DU manager configuration that will be visible to all running procedures
  du_manager_params     params;
  srslog::basic_logger& logger;

  // Components
  du_cell_manager                              cell_mng;
  du_ran_resource_manager_impl                 cell_res_alloc;
  du_ue_manager                                ue_mng;
  std::unique_ptr<f1ap_du_positioning_handler> positioning_handler;
  du_manager_metrics_aggregator_impl           metrics;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    running{false};

  // Handler for DU tasks.
  fifo_async_task_scheduler main_ctrl_loop;
};

} // namespace srs_du
} // namespace srsran
