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

#include "mac_ctrl/mac_config.h"
#include "mac_ctrl/mac_controller.h"
#include "mac_dl/mac_dl_processor.h"
#include "mac_dl/rlf_detector.h"
#include "mac_scheduler_adapter.h"
#include "mac_ul/mac_ul_processor.h"
#include "rach_handler/rach_handler.h"
#include "rach_handler/rnti_manager.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"

namespace srsran {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(const mac_config& mac_cfg);

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) override { return rach_hdl.get_cell(cell_index); }

  mac_ue_configurator& get_ue_configurator() override { return ctrl_unit; }

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override
  {
    return dl_unit.get_cell_control_information_handler(cell_index);
  }

  mac_ue_control_information_handler& get_ue_control_info_handler() override { return *mac_sched; }

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override
  {
    return dl_unit.get_slot_handler(cell_index);
  }

  mac_cell_manager& get_cell_manager() override { return ctrl_unit; }

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index) override { return ul_unit; }

  mac_paging_information_handler& get_cell_paging_info_handler() override { return *mac_sched; }

private:
  /// Used to allocate new TC-RNTIs and convert from C-RNTI to UE index.
  rnti_manager rnti_table;

  /// Detector of UE RLFs.
  rlf_detector rlf_handler;

  /// MAC scheduler.
  std::unique_ptr<mac_scheduler_adapter> mac_sched;

  /// Used to generate MAC DL PDUs and UL grants to be forwarded to the PHY.
  mac_dl_processor dl_unit;

  /// MAC Rx PDU processor.
  mac_ul_processor ul_unit;

  /// Used to handle RACH indications.
  rach_handler rach_hdl;

  /// Orchestrates the interaction and configuration of other MAC components.
  mac_controller ctrl_unit;
};

} // namespace srsran
