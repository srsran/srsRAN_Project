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

#include "srsran/du/du.h"

namespace srsran {

class du_power_controller;

namespace srs_du {

class o_du_high;
class o_du_low;

/// ORAN Distributed Unit interface. Includes the ORAN DU high, ORAN DU low and a DU power controller in an object.
class o_du
{
public:
  virtual ~o_du() = default;

  /// Returns the power controller of this ORAN DU.
  virtual du_power_controller& get_power_controller() = 0;

  /// Returns the ORAN DU high of this ORAN DU.
  virtual o_du_high& get_o_du_high() = 0;

  /// Returns the ORAN DU low of this ORAN DU.
  virtual o_du_low& get_o_du_low() = 0;
};

} // namespace srs_du
} // namespace srsran
