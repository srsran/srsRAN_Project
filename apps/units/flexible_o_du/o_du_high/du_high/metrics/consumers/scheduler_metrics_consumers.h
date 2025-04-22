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

/// JSON consumer for the scheduler cell metrics.
class scheduler_cell_metrics_consumer_json
{
public:
  explicit scheduler_cell_metrics_consumer_json(srslog::log_channel& log_chan_) : log_chan(log_chan_)
  {
    srsran_assert(log_chan.enabled(), "JSON log channel is not enabled");
  }

  /// Handle scheduler metrics.
  void handle_metric(const std::optional<scheduler_metrics_report>& report);

private:
  srslog::log_channel& log_chan;
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
