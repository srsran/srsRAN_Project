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

#include "srsran/scheduler/scheduler_slot_handler.h"
#include "srsran/support/format_utils.h"

namespace srsran {

class scheduler_result_logger
{
public:
  explicit scheduler_result_logger(bool log_broadcast_ = true, du_cell_index_t cell_idx = to_du_cell_index(0)) :
    logger(srslog::fetch_basic_logger("SCHED")),
    log_broadcast(log_broadcast_),
    enabled(logger.info.enabled()),
    cell_index(cell_idx)
  {
  }

  void on_slot_start()
  {
    if (enabled) {
      slot_start_tp = std::chrono::high_resolution_clock::now();
    }
  }

  void on_scheduler_result(const sched_result& result);

private:
  void log_debug(const sched_result& result);

  void log_info(const sched_result& result);

  srslog::basic_logger& logger;
  bool                  log_broadcast;
  bool                  enabled;

  du_cell_index_t cell_index;

  std::chrono::time_point<std::chrono::high_resolution_clock> slot_start_tp;
  fmt::memory_buffer                                          fmtbuf;
};

} // namespace srsran
