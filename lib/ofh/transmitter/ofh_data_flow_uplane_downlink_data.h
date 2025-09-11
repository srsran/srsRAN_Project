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

#include "ofh_data_flow_cuplane_encoding_metrics_collector.h"
#include "srsran/ofh/transmitter/ofh_transmitter_data_flow_metrics.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
struct resource_grid_context;
class shared_resource_grid;

namespace ofh {

class operation_controller;

/// Open Fronthaul User-Plane downlink data flow resource grid context.
struct data_flow_uplane_resource_grid_context {
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  uint8_t sector;
  /// Provides the port identifier.
  uint8_t port;
  /// eAxC.
  uint8_t eaxc;
  /// Symbol range.
  ofdm_symbol_range symbol_range;
};

/// Open Fronthaul User-Plane downlink data flow.
class data_flow_uplane_downlink_data
{
public:
  /// Default destructor.
  virtual ~data_flow_uplane_downlink_data() = default;

  /// Returns the controller of this Open Fronthaul User-Plane data flow.
  virtual operation_controller& get_operation_controller() = 0;

  /// Enqueues the User-Plane downlink data messages with the given context and resource grid.
  virtual void enqueue_section_type_1_message(const data_flow_uplane_resource_grid_context& context,
                                              const shared_resource_grid&                   grid) = 0;

  /// Returns the performance metrics collector of this data flow.
  virtual data_flow_message_encoding_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
