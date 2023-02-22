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

#include "cell/scheduler_cell_manager.h"
#include "logging/scheduler_event_logger.h"
#include "logging/scheduler_metrics_handler.h"
#include "logging/scheduler_result_logger.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/mac_scheduler.h"
#include "srsran/scheduler/scheduler_factory.h"
#include "ue_scheduling/ue_scheduler.h"

namespace srsran {

class scheduler_impl final : public mac_scheduler
{
public:
  explicit scheduler_impl(const scheduler_config& sched_cfg);

  bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) override;

  /// Add new UE to scheduler.
  void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request) override;

  /// Reconfiguration existing UE.
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;

  /// Remove UE from scheduler.
  void handle_ue_removal_request(du_ue_index_t ue_index) override;

  /// Called when RACH is detected.
  void handle_rach_indication(const rach_indication_message& msg) override;

  /// Obtain scheduling result for a given slot.
  const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) override;

  /// UE UL Buffer Status Report.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override
  {
    feedback_handler.handle_ul_bsr_indication(bsr);
  }

  /// UE DL buffer state update.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override
  {
    dl_bs_handler.handle_dl_buffer_state_indication(bs);
  }

  void handle_crc_indication(const ul_crc_indication& crc) override;

  void handle_uci_indication(const uci_indication& uci) override;

  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override
  {
    feedback_handler.handle_dl_mac_ce_indication(mac_ce);
  }

  /// Handle scheduling of paging message.
  void handle_paging_indication(const paging_indication_message& pi) override;

private:
  const scheduler_expert_config sched_cfg;
  srslog::basic_logger&         logger;
  scheduler_result_logger       sched_result_logger;
  scheduler_event_logger        sched_ev_logger;

  /// Slot metrics sink.
  scheduler_metrics_handler metrics;

  /// Scheduler for UEs.
  std::unique_ptr<ue_scheduler>                 ue_sched;
  scheduler_ue_configurator&                    ue_cfg_handler;
  scheduler_feedback_handler&                   feedback_handler;
  scheduler_dl_buffer_state_indication_handler& dl_bs_handler;

  /// Cell-specific resources and schedulers.
  scheduler_cell_manager cells;
};

} // namespace srsran
