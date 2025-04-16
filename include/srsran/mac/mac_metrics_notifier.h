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

namespace srsran {

struct mac_metric_report;

/// Notifier used by MAC to report new metrics.
class mac_metrics_notifier
{
public:
  virtual ~mac_metrics_notifier() = default;

  /// \brief Called on every new MAC metrics report.
  /// \param report Metric report.
  virtual void on_new_metrics_report(const mac_metric_report& report) = 0;
};

} // namespace srsran
