/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_du_low_metrics_collector_impl.h"
#include "srsran/du/du_low/o_du_low_metrics.h"

using namespace srsran;
using namespace srs_du;

void o_du_low_metrics_collector_impl::collect_o_du_low_metrics(o_du_low_metrics& metrics)
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

  auto tp_now            = std::chrono::high_resolution_clock::now();
  metrics.metrics_period = std::chrono::duration_cast<std::chrono::microseconds>(tp_now - last_timestamp);
  last_timestamp         = tp_now;
}
