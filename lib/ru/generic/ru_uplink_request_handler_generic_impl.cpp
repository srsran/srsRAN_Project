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

#include "ru_uplink_request_handler_generic_impl.h"
#include "srsran/phy/lower/lower_phy_request_handler.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;

void ru_uplink_request_handler_generic_impl::handle_prach_occasion(const prach_buffer_context& context,
                                                                   prach_buffer&               buffer)
{
  srsran_assert(context.sector < low_phy_handler.size(), "Invalid sector {}", context.sector);

  low_phy_handler[context.sector]->request_prach_window(context, buffer);
}

void ru_uplink_request_handler_generic_impl::handle_new_uplink_slot(const resource_grid_context& context,
                                                                    resource_grid&               grid)
{
  srsran_assert(context.sector < low_phy_handler.size(), "Invalid sector {}", context.sector);

  low_phy_handler[context.sector]->request_uplink_slot(context, grid);
}
