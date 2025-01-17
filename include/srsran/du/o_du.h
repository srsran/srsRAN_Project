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

#include "srsran/du/du.h"

namespace srsran {

class du_operation_controller;

namespace srs_du {

class o_du_high;
class o_du_low;

/// O-RAN Distributed Unit interface. Includes the O-RAN DU high, O-RAN DU low and a DU operation controller in an
/// object.
class o_du
{
public:
  virtual ~o_du() = default;

  /// Returns the operation controller of this O-RAN DU.
  virtual du_operation_controller& get_operation_controller() = 0;

  /// Returns the O-RAN DU high of this O-RAN DU.
  virtual o_du_high& get_o_du_high() = 0;

  /// Returns the O-RAN DU low of this O-RAN DU.
  virtual o_du_low& get_o_du_low() = 0;
};

} // namespace srs_du
} // namespace srsran
