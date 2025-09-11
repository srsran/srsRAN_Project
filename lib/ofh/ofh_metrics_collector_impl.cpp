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

#include "ofh_metrics_collector_impl.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/ofh/ofh_metrics.h"

using namespace srsran;
using namespace ofh;

metrics_collector_impl::metrics_collector_impl(receiver_metrics_collector*    rx_metrics_collector_,
                                               transmitter_metrics_collector* tx_metrics_collector_,
                                               unsigned                       sector_id_) :
  sector_id(sector_id_),
  rx_metrics_collector(rx_metrics_collector_),
  tx_metrics_collector(tx_metrics_collector_),
  last_timestamp(std::chrono::high_resolution_clock::now())
{
  is_enabled = (rx_metrics_collector && tx_metrics_collector);
}

void metrics_collector_impl::collect_metrics(sector_metrics& metric)
{
  if (disabled()) {
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
