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

#include "cell/scheduler_cell_manager.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "ue_scheduling/ue_scheduler.h"

namespace srsgnb {

class scheduler_impl final : public mac_scheduler
{
public:
  explicit scheduler_impl(sched_configuration_notifier& notifier);

  bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) override;

  /// Add UE.
  void handle_add_ue_request(const sched_ue_creation_request_message& ue_request) override;

  /// Reconfiguration existing UE.
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override;

  /// Delete UE.
  void handle_ue_delete_request(du_ue_index_t ue_index) override { mac_notifier.on_ue_delete_response(ue_index); }

  /// Called when RACH is detected.
  void handle_rach_indication(const rach_indication_message& msg) override;

  /// Obtain scheduling result for a given slot.
  const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) override;

  /// UE Scheduling Request.
  void handle_sr_indication(const sr_indication_message& sr) override { feedback_handler.handle_sr_indication(sr); }

  /// UE UL Buffer Status Report.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override
  {
    feedback_handler.handle_ul_bsr_indication(bsr);
  }

  /// UE DL Buffer Status Report.
  void handle_dl_bsr_indication(const dl_bsr_indication_message& bsr) override
  { // TODO
  }

  void handle_crc_indication(const ul_crc_indication& crc) override;

private:
  sched_configuration_notifier& mac_notifier;
  srslog::basic_logger&         logger;

  /// Scheduler for UEs.
  std::unique_ptr<ue_scheduler> ue_sched;
  scheduler_ue_configurator&    ue_cfg_handler;
  scheduler_feedback_handler&   feedback_handler;

  /// Cell-specific resources and schedulers.
  scheduler_cell_manager cells;
};

} // namespace srsgnb
