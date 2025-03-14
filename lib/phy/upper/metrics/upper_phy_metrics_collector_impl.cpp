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
