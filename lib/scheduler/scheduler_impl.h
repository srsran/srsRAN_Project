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

#include "cell_scheduler.h"
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

  /// Obtain scheduling result for a given slot and cell.
  const sched_result& slot_indication(slot_point sl_tx, du_cell_index_t cell_index) override;

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
  void handle_ul_ta_offset_indication(const ul_ta_offset_indication& ta_offset_ind) override;

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

  /// Container of DU Cell-specific resources.
  slotted_id_table<du_cell_index_t, std::unique_ptr<cell_scheduler>, MAX_NOF_DU_CELLS> cells;

  /// Container of DU Cell Group-specific resources.
  slotted_id_table<du_cell_group_index_t, std::unique_ptr<ue_scheduler>, MAX_DU_CELL_GROUPS> groups;

  /// Mapping of UEs to DU Cell Groups.
  slotted_id_table<du_ue_index_t, du_cell_group_index_t, MAX_NOF_DU_UES> ue_to_cell_group_index;
};

} // namespace srsran
