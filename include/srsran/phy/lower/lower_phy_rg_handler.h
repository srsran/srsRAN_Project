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

#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

/// \brief Lower physical layer resource grid handler interface.
///
/// The resource grid handler modulates and transmits the given downlink resource grid through the underlying radio
/// device.
class lower_phy_rg_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_rg_handler() = default;

  /// \brief Handles the given resource grid to be transmitted.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid Resource grid to transmit.
  virtual void handle_resource_grid(const resource_grid_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsran
