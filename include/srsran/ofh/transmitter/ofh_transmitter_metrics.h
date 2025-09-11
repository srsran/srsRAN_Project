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

#include "ofh_transmitter_data_flow_metrics.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul transmitter downlink processing metrics.
struct transmitter_dl_metrics {
  /// OFH transmitter downlink C-Plane processing metrics.
  tx_data_flow_perf_metrics dl_cp_metrics;
  /// OFH transmitter U-Plane processing metrics.
  tx_data_flow_perf_metrics dl_up_metrics;
  /// Number of late downlink resource grids received from the PHY.
  unsigned nof_late_dl_grids;
  /// Number of late Control-Plane downlink messages, ie, messages that were not transmitted.
  unsigned nof_late_cp_dl;
  /// Number of late User-Plane downlink messages, ie, messages that were not transmitted.
  unsigned nof_late_up_dl;
};

/// Open Fronthaul transmitter uplink requests processing metrics.
struct transmitter_ul_metrics {
  /// Open Fronthaul transmitter uplink C-Plane processing metrics.
  tx_data_flow_perf_metrics ul_cp_metrics;
  /// Number of late uplink request.
  unsigned nof_late_ul_requests;
  /// Number of late Control-Plane uplink messages, ie, messages that were not transmitted.
  unsigned nof_late_cp_ul;
};

/// Open Fronthaul message transmitter metrics.
struct message_transmitter_metrics {
  /// Message enqueueing and transmission statistics.
  float message_tx_min_latency_us;
  float message_tx_max_latency_us;
  float message_tx_avg_latency_us;

  /// CPU usage in microseconds of the message transmitter processing.
  float cpu_usage_us;
};

/// Open Fronthaul transmitter metrics.
struct transmitter_metrics {
  transmitter_dl_metrics      dl_metrics;
  transmitter_ul_metrics      ul_metrics;
  message_transmitter_metrics message_tx_metrics;
  ether::transmitter_metrics  eth_transmitter_metrics;
};

} // namespace ofh
} // namespace srsran
