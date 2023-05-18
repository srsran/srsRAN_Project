/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_downlink_handler_generic_impl.h"
#include "srsran/phy/lower/lower_phy_rg_handler.h"

using namespace srsran;

void ru_downlink_handler_generic_impl::handle_dl_data(const resource_grid_context& context,
                                                      const resource_grid_reader&  grid)
{
  handler.handle_resource_grid(context, grid);
}
