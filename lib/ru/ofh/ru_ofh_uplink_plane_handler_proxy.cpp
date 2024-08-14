/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_uplink_plane_handler_proxy.h"
#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;

void ru_uplink_plane_handler_proxy::handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer)
{
  srsran_assert(context.sector < sectors.size(), "Invalid sector id '{}'", context.sector);

  auto& sector = sectors[context.sector];
  sector->handle_prach_occasion(context, buffer);
}

void ru_uplink_plane_handler_proxy::handle_new_uplink_slot(const resource_grid_context& context,
                                                           const shared_resource_grid&  grid)
{
  srsran_assert(context.sector < sectors.size(), "Invalid sector id '{}'", context.sector);
  srsran_assert(grid, "Invalid grid.");

  auto& sector = sectors[context.sector];
  sector->handle_new_uplink_slot(context, grid);
}
