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

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver message processing metrics.
struct rx_data_flow_perf_metrics {
  /// Received message unpacking statistics.
  float message_unpacking_min_latency_us;
  float message_unpacking_max_latency_us;
  float message_unpacking_avg_latency_us;

  /// Number of dropped messages by the data flow.
  unsigned nof_dropped_messages;

  /// CPU usage in microseconds of the message processing in the receiver.
  float cpu_usage_us;
};

} // namespace ofh
} // namespace srsran
