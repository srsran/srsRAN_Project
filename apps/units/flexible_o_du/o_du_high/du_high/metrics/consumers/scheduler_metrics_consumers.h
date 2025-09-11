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

#include "srsran/scheduler/scheduler_metrics.h"
#include <optional>

namespace srsran {

/// STDOUT consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_stdout
{
  /// Maximum number of metric lines in STDOUT without a header, ie, print header every
  /// MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER lines.
  static constexpr unsigned MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER = 10;

public:
  /// Handle scheduler metrics.
  void handle_metric(const std::optional<scheduler_metrics_report>& report);

  /// Prints the header in the next metric handle.
  void print_header() { nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER; }

private:
  unsigned nof_lines = MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER;
};

/// Logger consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_log
{
public:
  explicit scheduler_cell_metrics_consumer_log(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "Logger log channel is not enabled");
  }

  /// Handle scheduler metrics.
  void handle_metric(const std::optional<scheduler_metrics_report>& report);

private:
  srslog::log_channel& log_chan;
};

/// E2 consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_e2
{
public:
  explicit scheduler_cell_metrics_consumer_e2(scheduler_metrics_notifier& notifier_) : notifier(notifier_) {}

  /// Handle scheduler metrics.
  void handle_metric(const std::optional<scheduler_metrics_report>& report);

private:
  scheduler_metrics_notifier& notifier;
};

} // namespace srsran
