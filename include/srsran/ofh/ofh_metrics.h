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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics.h"
#include "srsran/ofh/timing/ofh_timing_metrics.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics.h"
#include "srsran/ran/gnb_constants.h"
#include <chrono>

namespace srsran {
namespace ofh {

/// Open Fronthaul sector metrics.
struct sector_metrics {
  /// Sector identifier.
  unsigned sector_id;
  /// Receiver metrics.
  receiver_metrics rx_metrics;
  /// Transmitter metrics.
  transmitter_metrics tx_metrics;
  /// Metrics period.
  std::chrono::milliseconds metrics_period_ms;
};

/// Open Fronthaul metrics.
struct metrics {
  /// Timestamp of the OFH RU metrics.
  std::chrono::time_point<std::chrono::system_clock> timestamp;
  /// Timing metrics.
  timing_metrics timing;
  /// Metrics for every sector.
  static_vector<sector_metrics, MAX_CELLS_PER_DU> sectors;
};

} // namespace ofh
} // namespace srsran
