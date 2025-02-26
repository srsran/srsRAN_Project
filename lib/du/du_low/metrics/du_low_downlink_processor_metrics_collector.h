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

#include "aggregators/downlink_processor_metrics_aggregator.h"
#include "srsran/du/du_low/o_du_low_metrics.h"
#include "srsran/phy/metrics/phy_metrics_notifiers.h"

namespace srsran {
namespace srs_du {

/// DU low downlink processing metrics collector.
class du_low_downlink_processor_metrics_collector
{
public:
  /// Collects the metrics from the downlink aggregators and writes them into the given metrics argument.
  void collect_metrics(o_du_low_dl_processor_metrics& metrics)
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

} // namespace srs_du
} // namespace srsran
