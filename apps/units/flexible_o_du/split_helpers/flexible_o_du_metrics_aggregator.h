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

#include "metrics/flexible_o_du_metrics_notifier.h"
#include "srsran/du/o_du_metrics_notifier.h"

namespace srsran {

class ru_metrics_collector;

/// Flexible O-DU metrics aggregator. It aggregates O-DU metrics and RU metrics.
class flexible_o_du_metrics_aggregator : public srs_du::o_du_metrics_notifier
{
  flexible_o_du_metrics_notifier& notifier;
  ru_metrics_collector*           ru_collector = nullptr;

public:
  explicit flexible_o_du_metrics_aggregator(flexible_o_du_metrics_notifier* notifier_);

  // See interface for documentation.
  void on_new_metrics(const srs_du::o_du_metrics& odu_metrics) override;

  /// Sets the RU metrics collector to the given one.
  void set_ru_metrics_collector(ru_metrics_collector& collector) { ru_collector = &collector; }
};

} // namespace srsran
