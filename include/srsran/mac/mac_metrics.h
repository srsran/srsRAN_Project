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

#include <chrono>
#include <vector>

namespace srsran {

/// Metrics relative to a specific MAC cell.
struct mac_dl_cell_metric_report {
  unsigned                 nof_slots;
  double                   avg_latency_ms;
  std::chrono::nanoseconds max_latency;
  unsigned                 count_voluntary_context_switches;
  unsigned                 count_involuntary_context_switches;
};

struct mac_dl_metric_report {
  std::vector<mac_dl_cell_metric_report> cells;
};

/// \brief Structure holding the metrics of a MAC layer.
struct mac_metric_report {
  /// Metrics of the MAC DL.
  mac_dl_metric_report dl;
};

/// Notifier used by MAC to report new metrics.
class mac_metrics_notifier
{
public:
  virtual ~mac_metrics_notifier() = default;

  virtual void on_new_metrics_report(const mac_metric_report& report) = 0;
};

} // namespace srsran
