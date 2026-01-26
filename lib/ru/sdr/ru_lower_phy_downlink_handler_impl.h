/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/ru/ru_downlink_plane.h"

namespace srsran {

class lower_phy_downlink_handler;

/// Radio Unit to lower physical layer downlink handler implementation.
class ru_lower_phy_downlink_handler_impl : public ru_downlink_plane_handler
{
public:
  explicit ru_lower_phy_downlink_handler_impl(std::vector<lower_phy_downlink_handler*> handlers_) :
    handlers(std::move(handlers_))
  {
  }

  // See interface for documentation.
  void handle_dl_data(const resource_grid_context& context, const shared_resource_grid& grid) override;

private:
  std::vector<lower_phy_downlink_handler*> handlers;
};

} // namespace srsran
