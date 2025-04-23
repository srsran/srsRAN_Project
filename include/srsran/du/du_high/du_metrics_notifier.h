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
