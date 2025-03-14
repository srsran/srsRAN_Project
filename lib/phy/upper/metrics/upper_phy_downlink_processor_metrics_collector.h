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

#include "aggregators/downlink_processor_metrics_aggregator.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/upper/upper_phy_metrics.h"

namespace srsran {

/// Upper PHY downlink processing metrics collector.
class upper_phy_downlink_processor_metrics_collector
{
public:
  /// Collects the metrics from the downlink aggregators and writes them into the given metrics argument.
  void collect_metrics(upper_phy_dl_processor_metrics& metrics)
  {
    metrics.min_latency_us = downlink_processor_aggregator.get_min_latency_us();
    metrics.avg_latency_us = downlink_processor_aggregator.get_avg_latency_us();
    metrics.max_latency_us = downlink_processor_aggregator.get_max_latency_us();
    downlink_processor_aggregator.reset();
  }

  /// Gets the downlink processor metric notifier interface.
  downlink_processor_metric_notifier& get_notifier() { return downlink_processor_aggregator; }

private:
  downlink_processor_metrics_aggregator downlink_processor_aggregator;
};

} // namespace srsran
