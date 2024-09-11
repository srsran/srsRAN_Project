/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_cell_manager.h"
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
  ~du_manager_impl();

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

  async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) override;

  async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) override;

  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) override;

  async_task<void> handle_ue_deactivation_request(du_ue_index_t ue_index) override;

  void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override;

  void handle_ue_config_applied(du_ue_index_t ue_index) override;

  size_t nof_ues() override;

  async_task<du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(du_mac_sched_control_config reconf) override;

private:
  // DU manager configuration that will be visible to all running procedures
  du_manager_params     params;
  srslog::basic_logger& logger;

  // Components
  du_cell_manager              cell_mng;
  du_ran_resource_manager_impl cell_res_alloc;
  du_ue_manager                ue_mng;

  std::mutex              mutex;
  std::condition_variable cvar;
  bool                    running{false};

  // Handler for DU tasks.
  fifo_async_task_scheduler main_ctrl_loop;
};

} // namespace srs_du
} // namespace srsran