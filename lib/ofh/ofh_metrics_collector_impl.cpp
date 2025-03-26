/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_metrics_collector_impl.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/ofh/ofh_metrics.h"

using namespace srsran;
using namespace ofh;

metrics_collector_impl::metrics_collector_impl(receiver_metrics_collector*    rx_metrics_collector_,
                                               transmitter_metrics_collector* tx_metrics_collector_,
                                               unsigned                       sector_id_) :
  sector_id(sector_id_), rx_metrics_collector(rx_metrics_collector_), tx_metrics_collector(tx_metrics_collector_)
{
  is_enabled = (rx_metrics_collector && tx_metrics_collector);
}

void metrics_collector_impl::collect_metrics(sector_metrics& metric)
{
  if (disabled()) {
    return;
  }

  if (last_timestamp == std::chrono::time_point<std::chrono::high_resolution_clock>()) {
    last_timestamp = std::chrono::high_resolution_clock::now();
    metric         = {};
    return;
  }

  auto tp_now      = std::chrono::high_resolution_clock::now();
  metric.sector_id = sector_id;

  // Collect receiver metrics.
  rx_metrics_collector->collect_metrics(metric.rx_metrics);
  // Collect transmitter metrics.
  tx_metrics_collector->collect_metrics(metric.tx_metrics);

  metric.metrics_period_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp_now - last_timestamp);
  last_timestamp           = tp_now;
}
