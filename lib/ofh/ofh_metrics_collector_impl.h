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

#include "srsran/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "srsran/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "srsran/ofh/ofh_sector_metrics_collector.h"
#include "srsran/ofh/receiver/ofh_receiver_metrics_collector.h"
#include "srsran/ofh/transmitter/ofh_transmitter_metrics_collector.h"
#include <chrono>

namespace srsran {
namespace ofh {

/// Open Fronthaul sector metrics collector implementation.
class metrics_collector_impl : public metrics_collector
{
  bool                           is_enabled = false;
  const unsigned                 sector_id;
  receiver_metrics_collector*    rx_metrics_collector;
  transmitter_metrics_collector* tx_metrics_collector;

  /// The time point when the metrics where collected last time.
  std::chrono::high_resolution_clock::time_point last_timestamp = {};

public:
  metrics_collector_impl(receiver_metrics_collector*    rx_metrics_collector_,
                         transmitter_metrics_collector* tx_metrics_collector_,
                         unsigned                       sector_id_);

  // See interface for documentation.
  void collect_metrics(sector_metrics& metric) override;

  /// Returns true if the metrics are disabled, otherwise true.
  bool disabled() const { return !is_enabled; }
};

} // namespace ofh
} // namespace srsran
