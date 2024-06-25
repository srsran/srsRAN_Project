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

#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Class used to receive metrics reports and write them into a log file.
class metrics_log_helper : public scheduler_metrics_notifier, public rlc_metrics_notifier
{
public:
  explicit metrics_log_helper(srslog::basic_logger& logger_) : logger(logger_) {}

  /// Returns true if the metric logger is enabled, otherwise false.
  bool is_enabled() const { return (logger.info.enabled() or logger.debug.enabled()); };

  /// Notifier for the scheduler metrics.
  void report_metrics(const scheduler_cell_metrics& metrics) override;

  /// Notifier for the RLC metrics.
  void report_metrics(const rlc_metrics& metrics) override;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
