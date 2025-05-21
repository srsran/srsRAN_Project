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

#include "srsran/du/du_high/du_metrics_notifier.h"
#include "srsran/du/du_high/o_du_high_metrics_notifier.h"

namespace srsran {
namespace srs_du {

/// Proxy class that listen to scheduler metrics and forwards them to the configured scheduler metrics notifier and O-DU
/// high metrics notifier.
class o_du_high_metrics_notifier_proxy : public du_metrics_notifier
{
  du_metrics_notifier*        du_notifier     = nullptr;
  o_du_high_metrics_notifier* odu_hi_notifier = nullptr;

public:
  explicit o_du_high_metrics_notifier_proxy(du_metrics_notifier* notifier_);

  // See interface for documentation.
  void on_new_metric_report(const du_metrics_report& report) override;

  /// Sets the O-DU high metrics notifier to the given one.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier);
};

} // namespace srs_du
} // namespace srsran
