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

#include "../pucch_scheduling/pucch_allocator.h"
#include "../uci_scheduling/uci_allocator.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/scheduler/scheduler_positioning_handler.h"

namespace srsran {

class pdcch_resource_allocator;
struct cell_resource_allocator;
class sched_ue_configuration_handler;
class scheduler_event_logger;
class cell_metrics_handler;

/// Request to create a new cell handler in the UE scheduler.
struct ue_cell_scheduler_creation_request {
  du_cell_index_t cell_index;
  /// PDCCH allocator for the cell.
  pdcch_resource_allocator* pdcch_sched;
  /// PUCCH allocator for the cell.
  pucch_allocator* pucch_alloc;
  /// UCI allocator for the cell.
  uci_allocator* uci_alloc;
  /// Resource grid for the cell.
  cell_resource_allocator* cell_res_alloc;
  /// Cell metrics handler for the cell.
  cell_metrics_handler* cell_metrics;
  /// Logger of events for the cell.
  scheduler_event_logger* ev_logger;
};

/// Handler of UE grant scheduling for a given cell.
class ue_cell_scheduler
{
public:
  virtual ~ue_cell_scheduler() = default;

  /// Schedule UE DL and UL grants for a given {slot, cell}.
  virtual void run_slot(slot_point sl_tx) = 0;

  /// Handle error indication coming from the lower layers for a given {slot, cell}.
  virtual void handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event) = 0;

  /// Handle slice reconfiguration request for a given cell.
  virtual void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req) = 0;

  /// Retrieves handler of UE feedback for a given cell.
  virtual scheduler_feedback_handler& get_feedback_handler() = 0;

  /// Retrieves handler of UE positioning for a given cell.
  virtual scheduler_cell_positioning_handler& get_positioning_handler() = 0;

  /// Retrieves handler of DL buffer state updates for a given cell.
  virtual scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler() = 0;

  /// Return UE configurator.
  virtual sched_ue_configuration_handler& get_ue_configurator() = 0;

  /// Called when cell is activated.
  virtual void start() = 0;

  /// Called when cell is deactivated.
  virtual void stop() = 0;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler
{
protected:
  // Custom deleter that notifies the ue_scheduler that a cell has been removed.
  struct cell_deleter {
    cell_deleter() = default;
    cell_deleter(ue_scheduler& parent_, du_cell_index_t cell_idx_) : parent(&parent_), cell_index(cell_idx_) {}
    void operator()(ue_cell_scheduler* cell) const
    {
      if (cell != nullptr) {
        parent->do_rem_cell(cell_index);
      }
    }

    ue_scheduler*   parent;
    du_cell_index_t cell_index = INVALID_DU_CELL_INDEX;
  };

public:
  using unique_cell_ptr = std::unique_ptr<ue_cell_scheduler, cell_deleter>;

  virtual ~ue_scheduler() = default;

  /// Creates a new UE cell scheduler instance in the UE scheduler and returns a RAII handler for it.
  unique_cell_ptr add_cell(const ue_cell_scheduler_creation_request& params)
  {
    return unique_cell_ptr{do_add_cell(params), cell_deleter{*this, params.cell_index}};
  }

private:
  virtual ue_cell_scheduler* do_add_cell(const ue_cell_scheduler_creation_request& params) = 0;

  virtual void do_rem_cell(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
