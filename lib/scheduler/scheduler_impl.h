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

#include "cell_scheduler.h"
#include "config/sched_config_manager.h"
#include "logging/scheduler_event_logger.h"
#include "logging/scheduler_metrics_handler.h"
#include "logging/scheduler_result_logger.h"
#include "ue_scheduling/ue_scheduler.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

class scheduler_impl final : public mac_scheduler
{
public:
  explicit scheduler_impl(const scheduler_config& sched_cfg);

  bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) override;

  // scheduler_slot_handler interface methods.
  const sched_result& slot_indication(slot_point sl_tx, du_cell_index_t cell_index) override;
  void handle_error_indication(slot_point sl_tx, du_cell_index_t cell_index, error_outcome event) override;

  // DU manager events.
  void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request) override;
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;
  void handle_ue_removal_request(du_ue_index_t ue_index) override;

  // F1 events.
  void handle_paging_information(const sched_paging_information& pi) override;

  // RLC events.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override;

  // MAC events.
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override;
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;

  // PHY events.
  void handle_rach_indication(const rach_indication_message& msg) override;
  void handle_crc_indication(const ul_crc_indication& crc) override;
  void handle_uci_indication(const uci_indication& uci) override;

private:
  const scheduler_expert_config expert_params;
  sched_configuration_notifier& config_notifier;

  srslog::basic_logger&  logger;
  scheduler_event_logger sched_ev_logger;

  // Slot metrics sink.
  scheduler_metrics_handler metrics;

  // Manager of configurations forwarded to the scheduler.
  sched_config_manager cfg_mng;

  /// Container of DU Cell-specific resources.
  slotted_id_table<du_cell_index_t, std::unique_ptr<cell_scheduler>, MAX_NOF_DU_CELLS> cells;

  /// Container of DU Cell Group-specific resources.
  slotted_id_table<du_cell_group_index_t, std::unique_ptr<ue_scheduler>, MAX_DU_CELL_GROUPS> groups;
};

} // namespace srsran
