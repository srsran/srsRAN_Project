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

#include "srsran/ran/du_types.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/slot_point_extended.h"
#include "srsran/srslog/logger.h"
#include <array>

namespace srsran {

class timer_manager;
class task_executor;
class du_cell_timer_source;

/// \brief This entity is responsible for gathering the SFNs and subframes of all the cells, keep track of HSFN
/// increments, tick DU timers.
class du_time_controller
{
public:
  du_time_controller(timer_manager& timers_, task_executor& tick_exec_, srslog::basic_logger& logger_);

  std::unique_ptr<du_cell_timer_source> add_cell(du_cell_index_t cell_index);

private:
  class time_ticker_impl;

  struct cell_context {
    bool                active{false};
    slot_point_extended last_counter;
    std::atomic<int>    next{-1};
  };

  /// Called on the first slot indication received for a cell.
  void handle_cell_activation(du_cell_index_t cell_index, slot_point sl_tx);

  /// Called when the cell gets deactivated.
  void handle_cell_deactivation(du_cell_index_t cell_index);

  slot_point_extended handle_slot_ind(du_cell_index_t cell_index, slot_point sl_tx);

  timer_manager&        timers;
  task_executor&        tick_exec;
  srslog::basic_logger& logger;

  std::array<cell_context, MAX_NOF_DU_CELLS> cells;

  std::atomic<int32_t>  master_count{-1};
  std::atomic<int32_t>  missed_slots{0};
  std::atomic<uint32_t> nof_active_cells{0};
};

} // namespace srsran
