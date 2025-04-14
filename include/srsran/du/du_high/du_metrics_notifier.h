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

#include "srsran/f1ap/du/f1ap_du_metrics_collector.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <chrono>

namespace srsran {
namespace srs_du {

/// \brief Report of the DU metrics.
struct du_metrics_report {
  unsigned                                           version = 0;
  std::chrono::time_point<std::chrono::steady_clock> start_time;
  std::chrono::milliseconds                          period;
  std::optional<f1ap_metrics_report>                 f1ap;
  std::optional<mac_metric_report>                   mac;
  std::optional<scheduler_metrics_report>            scheduler;
};

/// \brief Interface used to push new DU metrics reports.
class du_metrics_notifier
{
public:
  virtual ~du_metrics_notifier() = default;

  /// \brief Called when a new DU metrics report is generated.
  /// \param report Generated report.
  virtual void on_new_metric_report(const du_metrics_report& report) = 0;
};

} // namespace srs_du
} // namespace srsran
