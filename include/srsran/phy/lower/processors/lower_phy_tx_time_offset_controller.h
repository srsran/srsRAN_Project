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

#include "srsran/ran/phy_time_unit.h"
#include <chrono>

namespace srsran {

/// Lower physical layer transmit time offset controller interface.
class lower_phy_tx_time_offset_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_tx_time_offset_controller() = default;

  /// \brief Sets a transmit time offset.
  virtual void set_tx_time_offset(phy_time_unit tx_time_offset) = 0;
};

} // namespace srsran
