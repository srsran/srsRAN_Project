/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given DU.
class du_task_scheduler
{
public:
  explicit du_task_scheduler(timer_manager&        timers_,
                             task_executor&        exec_,
                             uint16_t              max_nof_dus,
                             srslog::basic_logger& logger_);
  ~du_task_scheduler() = default;

  // CU-UP task scheduler
  void handle_du_async_task(du_index_t du_index, async_task<void>&& task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager&        timers;
  task_executor&        exec;
  srslog::basic_logger& logger;

  // task event loops indexed by du_index
  std::map<du_index_t, fifo_async_task_scheduler> du_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
