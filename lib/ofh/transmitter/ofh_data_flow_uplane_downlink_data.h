/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
struct resource_grid_context;
class resource_grid_reader;

namespace ofh {

/// Open Fronthaul data flow for User-Plane downlink data.
class data_flow_uplane_downlink_data
{
public:
  /// Default destructor.
  virtual ~data_flow_uplane_downlink_data() = default;

  /// Enqueues the User-Plane downlink data messages with the given context and resource grid.
  virtual void enqueue_section_type_1_message(const resource_grid_context& context,
                                              const resource_grid_reader&  grid,
                                              unsigned                     eaxc) = 0;
};

} // namespace ofh
} // namespace srsran
