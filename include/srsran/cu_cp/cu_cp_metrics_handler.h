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

#include "srsran/cu_cp/cu_cp_metrics_notifier.h"
#include <chrono>
#include <memory>

namespace srsran::srs_cu_cp {

/// Request for CU-CP metrics reporting.
struct periodic_metric_report_request {
  /// \brief The period at which the CU-CP metrics are reported. In case, the period is 0, the CU-CP metrics are only
  /// sent once.
  std::chrono::milliseconds period{0};
  /// \brief The notifier used by the CU-CP to report metrics.
  cu_cp_metrics_report_notifier* report_notifier = nullptr;
};

/// Metric Report session instance.
class metrics_report_session
{
public:
  /// Closes the session and destroys it.
  virtual ~metrics_report_session() = default;

  /// Configure the metrics report session.
  virtual void reconfigure_request(const periodic_metric_report_request& request) = 0;

  /// Close the session, explicitly stopping the reporting of new metrics.
  virtual void stop() = 0;
};

/// Interface for requesting CU-CP metrics
class metrics_handler
{
public:
  virtual ~metrics_handler() = default;

  /// \brief Create a new metrics periodic report session.
  /// \return Handle to the session. When the session object goes out of scope, the session is automatically closed.
  virtual std::unique_ptr<metrics_report_session>
  create_periodic_report_session(const periodic_metric_report_request& request) = 0;

  /// \brief Synchronously request a new single metrics report. The call blocks until the report is ready.
  ///
  /// Note: Given its blocking nature, avoid calling this method in contexts other than unit tests.
  /// \return The metrics report.
  virtual cu_cp_metrics_report request_metrics_report() const = 0;
};

} // namespace srsran::srs_cu_cp
