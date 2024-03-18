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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/ran/srs/srs_resource_configuration.h"
#include <cstdint>

namespace srsran {

/// Parameters required to receive the Sounding Reference Signals described in 3GPP TS38.211 Section 6.4.1.4.
struct srs_estimator_configuration {
  /// Numerology, SFN and slot index.
  slot_point slot;
  /// SRS resource configuration.
  srs_resource_configuration resource;
  /// Receive port list.
  static_vector<uint8_t, srs_constants::max_nof_rx_ports> ports;
};

} // namespace srsran
