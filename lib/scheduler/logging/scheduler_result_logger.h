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
#include "srsran/srslog/logger.h"
#include "srsran/support/format_utils.h"

namespace srsran {

class scheduler_result_logger
{
public:
  explicit scheduler_result_logger(bool log_broadcast_, pci_t pci_);

  /// Log scheduler result for a specific cell and slot.
  /// \param[in] result Scheduling result for this slot.
  /// \param[in] slot_latency Latency that it took for the scheduler to make the decision for the slot.
  void on_scheduler_result(const sched_result&       result,
                           std::chrono::microseconds slot_latency = std::chrono::microseconds{0});

private:
  void log_debug(const sched_result& result, std::chrono::microseconds slot_latency);

  void log_info(const sched_result& result, std::chrono::microseconds slot_latency);

  srslog::basic_logger& logger;
  bool                  log_broadcast;
  bool                  enabled;
  const pci_t           pci;

  fmt::memory_buffer fmtbuf;
};

} // namespace srsran
