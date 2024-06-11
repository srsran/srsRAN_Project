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
  const pci_t           pci;

  std::chrono::time_point<std::chrono::high_resolution_clock> slot_start_tp;
  fmt::memory_buffer                                          fmtbuf;
};

} // namespace srsran
