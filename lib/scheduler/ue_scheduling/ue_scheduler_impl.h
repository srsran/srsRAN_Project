/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../cell/cell_harq_manager.h"
#include "../logging/scheduler_event_logger.h"
#include "../slicing/inter_slice_scheduler.h"
#include "../srs/srs_scheduler_impl.h"
#include "../uci_scheduling/uci_scheduler_impl.h"
#include "intra_slice_scheduler.h"
#include "ue_cell_grid_allocator.h"
#include "ue_event_manager.h"
#include "ue_fallback_scheduler.h"
#include "ue_repository.h"
#include "ue_scheduler.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include <mutex>

namespace srsran {

/// \brief Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot.
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler_impl final : public ue_scheduler
{
public:
  explicit ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_);

private:
  ue_cell_scheduler* do_add_cell(const ue_cell_scheduler_creation_request& params) override;

  void do_start_cell(du_cell_index_t cell_index);
  void do_stop_cell(du_cell_index_t cell_index);

  void do_rem_cell(du_cell_index_t cell_index) override;

  void run_slot_impl(slot_point sl_tx);

  void run_sched_strategy(du_cell_index_t cell_index);

  /// Counts the number of PUCCH grants that are allocated for a given user at a specific slot.
  void update_harq_pucch_counter(cell_resource_allocator& cell_alloc);

  struct cell_context : public ue_cell_scheduler {
    ue_scheduler_impl& parent;

    cell_resource_allocator* cell_res_alloc;

    /// HARQ pool for this cell.
    cell_harq_manager cell_harqs;

    /// PUCCH scheduler.
    uci_scheduler_impl uci_sched;

    /// Fallback scheduler.
    ue_fallback_scheduler fallback_sched;

    /// Slice scheduler.
    inter_slice_scheduler slice_sched;

    /// Intra-slice scheduler.
    intra_slice_scheduler intra_slice_sched;

    /// SRS scheduler
    srs_scheduler_impl srs_sched;

    /// Cell-specific event manager.
    std::unique_ptr<ue_cell_event_manager> ev_mng;

    cell_context(ue_scheduler_impl& parent, const ue_cell_scheduler_creation_request& params);

    void run_slot(slot_point sl_tx) override { parent.run_slot_impl(sl_tx); }

    void handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event) override
    {
      ev_mng->handle_error_indication(sl_tx, event);
    }

    void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req) override
    {
      ev_mng->handle_slice_reconfiguration_request(slice_reconf_req);
    }

    scheduler_feedback_handler&                   get_feedback_handler() override { return *ev_mng; }
    scheduler_positioning_handler&                get_positioning_handler() override { return *ev_mng; }
    scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler() override { return *ev_mng; }
    sched_ue_configuration_handler&               get_ue_configurator() override { return *ev_mng; }

    void start() override { parent.do_start_cell(cell_res_alloc->cfg.cell_index); }

    void stop() override { parent.do_stop_cell(cell_res_alloc->cfg.cell_index); }
  };

  const scheduler_ue_expert_config& expert_cfg;
  srslog::basic_logger&             logger;

  // List of cells of the UE scheduler.
  slotted_array<cell_context, MAX_NOF_DU_CELLS> cells;

  /// Repository of created UEs.
  ue_repository ue_db;

  /// Processor of UE input events.
  ue_event_manager event_mng;

  // Mutex to lock cells of the same cell group (when CA enabled) for joint carrier scheduling
  std::mutex cell_group_mutex;

  // Last slot run.
  slot_point last_sl_ind;
};

} // namespace srsran
