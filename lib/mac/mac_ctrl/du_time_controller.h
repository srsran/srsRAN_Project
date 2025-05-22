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
  std::atomic<uint32_t> nof_active_cells{0};
};

} // namespace srsran
