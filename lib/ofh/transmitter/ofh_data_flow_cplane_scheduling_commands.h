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

#include "srsran/ofh/ofh_message_properties.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul data flow for Control-Plane scheduling and beamforming commands.
class data_flow_cplane_scheduling_commands
{
public:
  /// Default destructor.
  virtual ~data_flow_cplane_scheduling_commands() = default;

  /// Enqueues a section type 1 Open Fronthaul message with the given slot and eAxC.
  virtual void enqueue_section_type_1_message(slot_point        slot,
                                              unsigned          eaxc,
                                              data_direction    direction,
                                              filter_index_type filter_type) = 0;
};

} // namespace ofh
} // namespace srsran
