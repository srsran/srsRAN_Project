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
#include "srsran/srslog/logger.h"
#include <array>

namespace srsran {

class timer_manager;
class task_executor;
class du_cell_timer_source;

using subframe_counter = uint32_t;

class du_time_controller
{
public:
  du_time_controller(timer_manager& timers_, task_executor& tick_exec_, srslog::basic_logger& logger_);

  std::unique_ptr<du_cell_timer_source> add_cell(du_cell_index_t cell_index);

private:
  class time_ticker_impl;

  struct cell_context {
    bool             active{false};
    subframe_counter last_counter{0};
    std::atomic<int> next{-1};
  };

  bool push_back_new_cell(du_cell_index_t cell_index);

  bool rem_cell(du_cell_index_t cell_index);

  void handle_cell_activation(du_cell_index_t cell_index, unsigned sfn, unsigned subframe);

  void handle_cell_deactivation(du_cell_index_t cell_index);

  void handle_slot_ind(du_cell_index_t cell_index, unsigned sfn, unsigned subframe);

  subframe_counter now(du_cell_index_t cell_index) const;

  timer_manager&        timers;
  task_executor&        tick_exec;
  srslog::basic_logger& logger;

  std::array<cell_context, MAX_NOF_DU_CELLS> cells;

  std::atomic<int>              head{-1};
  std::atomic<subframe_counter> master_count{0};
};

} // namespace srsran
