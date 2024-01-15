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

#include "../pucch_scheduling/pucch_allocator.h"
#include "../uci_scheduling/uci_allocator.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

class pdcch_resource_allocator;
struct cell_resource_allocator;
class sched_ue_configuration_handler;

struct ue_scheduler_cell_params {
  du_cell_index_t           cell_index;
  pdcch_resource_allocator* pdcch_sched;
  pucch_allocator*          pucch_alloc;
  uci_allocator*            uci_alloc;
  cell_resource_allocator*  cell_res_alloc;
};

/// Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler
{
public:
  virtual ~ue_scheduler() = default;

  virtual void add_cell(const ue_scheduler_cell_params& params) = 0;

  /// Schedule UE DL and UL grants for a given {slot, cell}.
  virtual void run_slot(slot_point slot_tx, du_cell_index_t cell_index) = 0;

  /// Handle error in the lower layers.
  virtual void handle_error_indication(slot_point                            sl_tx,
                                       du_cell_index_t                       cell_index,
                                       scheduler_slot_handler::error_outcome event) = 0;

  /// Return UE configurator.
  virtual sched_ue_configuration_handler& get_ue_configurator() = 0;

  virtual scheduler_feedback_handler& get_feedback_handler() = 0;

  virtual scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler() = 0;
};

} // namespace srsran
