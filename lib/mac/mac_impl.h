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

#include "mac_ctrl/mac_config.h"
#include "mac_ctrl/mac_controller.h"
#include "mac_dl/mac_dl_processor.h"
#include "mac_sched/mac_scheduler_adapter.h"
#include "mac_sched/rlf_detector.h"
#include "mac_ul/mac_ul_processor.h"
#include "rnti_manager.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"

namespace srsran {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(const mac_config& mac_cfg);

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) override
  {
    return mac_sched->get_cell_rach_handler(cell_index);
  }

  mac_ue_configurator& get_ue_configurator() override { return ctrl_unit; }

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override
  {
    return mac_sched->get_cell_control_info_handler(cell_index);
  }

  mac_ue_control_information_handler& get_ue_control_info_handler() override { return *mac_sched; }

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override
  {
    return dl_unit.get_slot_handler(cell_index);
  }

  mac_cell_manager& get_cell_manager() override { return ctrl_unit; }

  mac_pdu_handler& get_pdu_handler() override { return ul_unit; }

  mac_paging_information_handler& get_cell_paging_info_handler() override { return *mac_sched; }

private:
  /// Used to allocate new TC-RNTIs and convert from C-RNTI to UE index.
  rnti_manager rnti_table;

  /// MAC scheduler.
  std::unique_ptr<mac_scheduler_adapter> mac_sched;

  /// Used to generate MAC DL PDUs and UL grants to be forwarded to the PHY.
  mac_dl_processor dl_unit;

  /// MAC Rx PDU processor.
  mac_ul_processor ul_unit;

  /// Orchestrates the interaction and configuration of other MAC components.
  mac_controller ctrl_unit;
};

} // namespace srsran
