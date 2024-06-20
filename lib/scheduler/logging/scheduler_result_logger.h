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
