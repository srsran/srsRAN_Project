/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "../mac_ctrl/srs_sched_config_adapter.h"
#include "mac_cell_processor.h"
#include "mac_dl_ue_manager.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {

class mac_dl_processor final : public mac_dl_configurator
{
public:
  explicit mac_dl_processor(mac_common_config_t& cfg_, mac_scheduler& sched_, du_rnti_table& rnti_table_);

  bool has_cell(du_cell_index_t cell_index) const;

  /// Adds new cell configuration to MAC DL.
  void add_cell(const mac_cell_creation_request& cell_cfg) override;

  /// Removes cell configuration from MAC DL.
  void remove_cell(du_cell_index_t cell_index) override;

  mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) override { return *cells[cell_index]; }

  /// Creates new UE DL context, updates logical channel MUX, adds UE in scheduler.
  async_task<bool> add_ue(const mac_ue_create_request_message& request) override;

  /// Deletes UE context in MAC DL and scheduler.
  async_task<void> remove_ue(const mac_ue_delete_request_message& request) override;

  /// Reconfigures UE in MAC DL and in the scheduler.
  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override;

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) { return *cells[cell_index]; }

  mac_cell_control_information_handler& get_cell_control_information_handler(du_cell_index_t cell_index)
  {
    srsgnb_assert(cells[cell_index], "Cell index does not exist");

    return *cells[cell_index];
  }

private:
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  std::array<std::unique_ptr<mac_cell_processor>, MAX_NOF_DU_CELLS> cells;

  mac_dl_ue_manager ue_mng;

  mac_scheduler& sched_obj;
};

} // namespace srsgnb
