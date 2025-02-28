/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_downlink_handler_generic_impl.h"
#include "lower_phy/lower_phy_sector.h"
#include "srsran/phy/lower/lower_phy_rg_handler.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;

void ru_downlink_handler_generic_impl::handle_dl_data(const resource_grid_context& context,
                                                      const shared_resource_grid&  grid)
{
  srsran_assert(context.sector < handler.size(), "Invalid sector {}", context.sector);

  handler[context.sector]->get_rg_handler().handle_resource_grid(context, grid);
}
