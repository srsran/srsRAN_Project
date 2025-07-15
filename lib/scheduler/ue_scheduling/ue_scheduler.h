/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../pucch_scheduling/pucch_allocator.h"
#include "../uci_scheduling/uci_allocator.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

class pdcch_resource_allocator;
struct cell_resource_allocator;
class sched_ue_configuration_handler;
class scheduler_event_logger;
class cell_metrics_handler;

struct ue_scheduler_cell_params {
  du_cell_index_t           cell_index;
  pdcch_resource_allocator* pdcch_sched;
  pucch_allocator*          pucch_alloc;
  uci_allocator*            uci_alloc;
  cell_resource_allocator*  cell_res_alloc;
  cell_metrics_handler*     cell_metrics;
  scheduler_event_logger*   ev_logger;
};

/// Handler of scheduling of UEs in a given cell.
class ue_cell_scheduler
{
public:
  virtual ~ue_cell_scheduler() = default;

  /// Schedule UE DL and UL grants for a given {slot, cell}.
  virtual void run_slot(slot_point sl_tx) = 0;

  /// Retrieves handler of UE feedback for a given cell.
  virtual scheduler_feedback_handler& get_feedback_handler() = 0;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler
{
protected:
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
  unique_cell_ptr add_cell(const ue_scheduler_cell_params& params)
  {
    return unique_cell_ptr{do_add_cell(params), cell_deleter{*this, params.cell_index}};
  }

  /// Handle error in the lower layers.
  virtual void handle_error_indication(slot_point                            sl_tx,
                                       du_cell_index_t                       cell_index,
                                       scheduler_slot_handler::error_outcome event) = 0;

  /// Return UE configurator.
  virtual sched_ue_configuration_handler& get_ue_configurator() = 0;

  virtual scheduler_feedback_handler& get_feedback_handler() = 0;

  virtual scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler() = 0;

  virtual scheduler_positioning_handler& get_positioning_handler() = 0;

private:
  virtual ue_cell_scheduler* do_add_cell(const ue_scheduler_cell_params& params) = 0;

  virtual void do_rem_cell(du_cell_index_t cell_index) = 0;
};

} // namespace srsran
