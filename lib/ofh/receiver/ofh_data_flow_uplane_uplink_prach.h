/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/span.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane uplink PRACH data flow.
class data_flow_uplane_uplink_prach
{
public:
  /// Default destructor.
  virtual ~data_flow_uplane_uplink_prach() = default;

  /// Decodes the given Open Fronthaul message associated to the given eAxC.
  virtual void decode_type1_message(unsigned eaxc, span<const uint8_t> message) = 0;
};

} // namespace ofh
} // namespace srsran
