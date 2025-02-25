/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_metrics_collector_impl.h"

using namespace srsran;

void upper_phy_metrics_collector_impl::collect_metrics(upper_phy_metrics& metrics)
{
  if (last_timestamp == std::chrono::time_point<std::chrono::high_resolution_clock>()) {
    last_timestamp         = std::chrono::high_resolution_clock::now();
    metrics.metrics_period = {};
    return;
  }

  // Collect LDPC mettrics.
  ldpc_collector.collect_metrics(metrics.ldpc_metrics);

  // Collect PUSCH processing metrics.
  pusch_collector.collect_metrics(metrics.pusch_metrics);

  // Collect PDSCH processing metrics.
  pdsch_collector.collect_metrics(metrics.pdsch_metrics);

  // Collect downlink processing metrics.
  dl_processor_collector.collect_metrics(metrics.dl_processor_metrics);

  auto tp_now            = std::chrono::high_resolution_clock::now();
  metrics.metrics_period = std::chrono::duration_cast<std::chrono::microseconds>(tp_now - last_timestamp);
  last_timestamp         = tp_now;
}
