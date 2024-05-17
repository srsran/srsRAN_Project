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

#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Class used to receive metrics reports and write them into a log file.
class metrics_log_helper : public scheduler_ue_metrics_notifier, public rlc_metrics_notifier
{
public:
  explicit metrics_log_helper(srslog::basic_logger& logger_) : logger(logger_) {}

  /// Returns true if the metric logger is enabled, otherwise false.
  bool is_enabled() const { return (logger.info.enabled() or logger.debug.enabled()); };

  /// Notifier for the scheduler metrics.
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override;

  /// Notifier for the RLC metrics.
  void report_metrics(const rlc_metrics& metrics) override;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
