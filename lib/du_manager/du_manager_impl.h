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

#include "du_cell_manager.h"
#include "du_ue_manager.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

class du_manager_impl final : public du_manager_interface
{
public:
  du_manager_impl(const du_manager_config_t& cfg);

  // Controller interface.
  void start() override;
  void stop() override;

  // MAC interface
  void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) override;

  // Task scheduling interface.
  void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task) override
  {
    ue_mng.schedule_async_task(ue_index, std::move(task));
  }

  async_task<f1ap_ue_config_update_response>
  handle_ue_config_update(const f1ap_ue_config_update_request& request) override;

  async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) override;

  size_t nof_ues() override;

private:
  // DU manager configuration that will be visible to all running procedures
  du_manager_config_t cfg;

  // Components
  du_cell_manager cell_mng;
  du_ue_manager   ue_mng;

  // Handler for DU tasks.
  async_task_sequencer main_ctrl_loop;
};

} // namespace srs_du
} // namespace srsgnb
