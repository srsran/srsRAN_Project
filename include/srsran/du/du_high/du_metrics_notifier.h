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

#include "srsran/du/du_high/du_metrics_report.h"

namespace srsran {
namespace srs_du {

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
