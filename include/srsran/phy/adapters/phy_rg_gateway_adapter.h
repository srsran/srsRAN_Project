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

#include "srsran/phy/lower/lower_phy_rg_handler.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"

namespace srsran {

/// Implements a generic physical layer resource grid gateway adapter.
class phy_rg_gateway_adapter : public upper_phy_rg_gateway
{
private:
  lower_phy_rg_handler* rg_handler = nullptr;

public:
  /// Connects the adaptor to the lower physical layer gateway.
  void connect(lower_phy_rg_handler* handler) { rg_handler = handler; }

  // See interface for documentation.
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    report_fatal_error_if_not(rg_handler, "Adapter is not connected.");
    rg_handler->handle_resource_grid(context, grid);
  }
};

} // namespace srsran
