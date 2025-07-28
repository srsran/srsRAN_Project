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

/// Lower physical layer carrier center frequency controller interface.
class lower_phy_center_freq_controller
{
public:
  /// Default destructor.
  virtual ~lower_phy_center_freq_controller() = default;

  /// Sets the carrier center frequency to the given value. Returns true on success, otherwise false.
  virtual bool set_carrier_center_frequency(double carrier_center_frequency_Hz) = 0;
};

} // namespace srsran
