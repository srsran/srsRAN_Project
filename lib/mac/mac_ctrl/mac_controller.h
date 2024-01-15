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

#pragma once

#include "../mac_config_interfaces.h"
#include "mac_config.h"
#include "mac_scheduler_configurator.h"
#include "srsran/mac/mac_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/du_ue_list.h"

namespace srsran {

struct mac_ue_context {
  du_ue_index_t   du_ue_index = MAX_NOF_DU_UES;
  rnti_t          rnti        = rnti_t::INVALID_RNTI;
  du_cell_index_t pcell_idx   = MAX_NOF_DU_CELLS;
};

class rnti_manager;

class mac_controller : public mac_ctrl_configurator, public mac_ue_configurator, public mac_cell_manager
{
public:
  mac_controller(const mac_control_config&   cfg_,
                 mac_ul_configurator&        ul_unit_,
                 mac_dl_configurator&        dl_unit_,
                 rnti_manager&               rnti_table_,
                 mac_scheduler_configurator& sched_cfg_);

  /// Adds new cell configuration to MAC. The configuration is forwarded to the scheduler.
  void add_cell(const mac_cell_creation_request& cell_cfg) override;

  /// Removes cell configuration from MAC. The cell is also removed from the scheduler.
  void remove_cell(du_cell_index_t cell_index) override;

  mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) override;

  /// Creates UE in MAC and scheduler.
  async_task<mac_ue_create_response> handle_ue_create_request(const mac_ue_create_request& msg) override;

  /// Deletes UE from MAC and scheduler.
  async_task<mac_ue_delete_response> handle_ue_delete_request(const mac_ue_delete_request& msg) override;

  /// Reconfigures UE in MAC and scheduler.
  async_task<mac_ue_reconfiguration_response>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg) override;

  bool handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) override
  {
    return ul_unit.flush_ul_ccch_msg(ue_index, std::move(pdu));
  }

  /// Fetch UE context
  mac_ue_context* find_ue(du_ue_index_t ue_index);
  mac_ue_context* find_by_rnti(rnti_t rnti);

private:
  /// Adds UE solely in MAC controller.
  rnti_t add_ue(du_ue_index_t ue_index, du_cell_index_t pcell_index, rnti_t tc_rnti) override;

  /// Interface to MAC controller main class used by MAC controller procedures.
  void remove_ue(du_ue_index_t ue_index) override;

  // args
  mac_control_config          cfg;
  srslog::basic_logger&       logger;
  mac_ul_configurator&        ul_unit;
  mac_dl_configurator&        dl_unit;
  rnti_manager&               rnti_table;
  mac_scheduler_configurator& sched_cfg;

  // UE database
  du_ue_list<mac_ue_context> ue_db;
};

} // namespace srsran
