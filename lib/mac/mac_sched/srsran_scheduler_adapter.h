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

#include "../mac_ctrl/mac_config.h"
#include "../mac_ctrl/mac_scheduler_configurator.h"
#include "../mac_dl/rlf_detector.h"
#include "../rnti_manager.h"
#include "mac_scheduler_adapter.h"
#include "uci_cell_decoder.h"
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

  void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) override;

  void handle_ul_sched_command(const mac_ul_scheduling_command& cmd) override;

  void handle_dl_mac_ce_indication(const mac_ce_scheduling_command& mac_ce) override;

  /// \brief Forward to scheduler an RLC DL buffer state update.
  void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs_ind) override;

  void handle_ul_phr_indication(const mac_phr_ce_info& phr) override;

  void handle_paging_information(const paging_information& msg) override;

  const sched_result& slot_indication(slot_point slot_tx, du_cell_index_t cell_idx) override;

  mac_cell_rach_handler& get_cell_rach_handler(du_cell_index_t cell_index) override
  {
    return cell_handlers[cell_index];
  }

  mac_cell_control_information_handler& get_cell_control_info_handler(du_cell_index_t cell_index) override
  {
    return cell_handlers[cell_index];
  }

private:
  class cell_handler final : public mac_cell_rach_handler, public mac_cell_control_information_handler
  {
  public:
    cell_handler(du_cell_index_t                                 cell_idx_,
                 srsran_scheduler_adapter&                       parent_,
                 const sched_cell_configuration_request_message& sched_cfg) :
      uci_decoder(sched_cfg, parent_.rnti_mng, parent_.rlf_handler), cell_idx(cell_idx_), parent(&parent_)
    {
    }

    void handle_rach_indication(const mac_rach_indication& rach_ind) override;

    void handle_crc(const mac_crc_indication_message& msg) override;

    void handle_uci(const mac_uci_indication_message& msg) override;

    uci_cell_decoder uci_decoder;

  private:
    du_cell_index_t           cell_idx = INVALID_DU_CELL_INDEX;
    srsran_scheduler_adapter* parent   = nullptr;
  };

  class sched_config_notif_adapter final : public sched_configuration_notifier
  {
  public:
    explicit sched_config_notif_adapter(srsran_scheduler_adapter& parent_) : parent(parent_) {}
    void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) override;
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
  std::unique_ptr<mac_scheduler> sched_impl;

  /// Allocator of TC-RNTI values.
  rnti_manager rnti_alloc;

  /// List of event flags used by scheduler to notify that the configuration is complete.
  struct ue_notification_context {
    manual_event<bool> ue_config_ready;
  };
  std::array<ue_notification_context, MAX_NOF_DU_UES> sched_cfg_notif_map;

  /// Handler for each DU cell.
  slotted_id_table<du_cell_index_t, cell_handler, MAX_NOF_DU_CELLS> cell_handlers;
};

} // namespace srsran
