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

#include "mac_config.h"
#include "mac_ctrl/mac_scheduler_configurator.h"
#include "mac_dl/rlf_detector.h"
#include "mac_scheduler_adapter.h"
#include "mac_ul/mac_scheduler_ul_buffer_state_updater.h"
#include "rach_handler/rnti_manager.h"
#include "srsran/mac/mac_configuration_helpers.h"
#include "srsran/scheduler/mac_scheduler.h"
#include "srsran/support/async/manual_event.h"

namespace srsran {

/// \brief This class adapts srsRAN scheduler interface to operate with srsRAN MAC.
/// The configuration completion notification handling (e.g. ue creation complete) is deferred for later processing
/// rather than being processed inline. We defer the processing because we do not want it to take place while
/// the scheduler is still processing the slot_indication, given that latter has higher priority.
class srsran_scheduler_adapter final : public mac_scheduler_adapter
{
public:
  explicit srsran_scheduler_adapter(const mac_config& params, rnti_manager& rnti_mng_, rlf_detector& rlf_handler_);

  void add_cell(const mac_cell_creation_request& msg) override;

  void remove_cell(du_cell_index_t cell_index) override
  {
    // TODO: Call scheduler cell remove.
  }

  async_task<bool> handle_ue_creation_request(const mac_ue_create_request& msg) override;

  async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg) override;

  async_task<bool> handle_ue_removal_request(const mac_ue_delete_request& msg) override;

  sched_configuration_notifier& get_sched_notifier() override { return notifier; }

  void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) override;

  void handle_ul_sched_command(const mac_ul_scheduling_command& cmd) override;

  void handle_crc_info(du_cell_index_t cell_idx, const mac_crc_indication_message& msg) override;

  void handle_uci(du_cell_index_t cell_idx, const mac_uci_indication_message& msg) override;

  /// \brief Forward to scheduler an RLC DL buffer state update.
  void handle_dl_buffer_state_update_required(const mac_dl_buffer_state_indication_message& dl_bs_ind) override;

  void handle_rach_indication(du_cell_index_t cell_index, const mac_rach_indication& rach_ind) override;

  void handle_paging_information(const paging_information& msg) override;

  const sched_result& slot_indication(slot_point slot_tx, du_cell_index_t cell_idx) override;

private:
  class sched_config_notif_adapter : public sched_configuration_notifier
  {
  public:
    explicit sched_config_notif_adapter(srsran_scheduler_adapter& parent_) : parent(parent_) {}
    void on_ue_config_complete(du_ue_index_t ue_index) override;
    void on_ue_delete_response(du_ue_index_t ue_index) override;

  private:
    srsran_scheduler_adapter& parent;
  };

  rnti_manager&         rnti_mng;
  rlf_detector&         rlf_handler;
  task_executor&        ctrl_exec;
  srslog::basic_logger& logger;

  /// Notifier that is used by MAC to start and await configurations of the scheduler.
  sched_config_notif_adapter notifier;

  /// srsGNB scheduler.
  std::unique_ptr<mac_scheduler> srs_sched;

  /// Allocator of TC-RNTI values.
  rnti_manager rnti_alloc;

  /// List of event flags used by scheduler to notify that the configuration is complete.
  struct ue_notification_context {
    manual_event<bool> ue_config_ready;
  };
  std::array<ue_notification_context, MAX_NOF_DU_UES> sched_cfg_notif_map;
};

} // namespace srsran
