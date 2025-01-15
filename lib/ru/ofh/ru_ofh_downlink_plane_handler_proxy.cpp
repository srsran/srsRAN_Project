/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_downlink_plane_handler_proxy.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include <memory>

using namespace srsran;

void ru_downlink_plane_handler_proxy::handle_dl_data(const resource_grid_context& context,
                                                     const shared_resource_grid&  grid)
{
  srsran_assert(context.sector < sectors.size(), "Invalid sector id '{}'", context.sector);

  auto& sector = sectors[context.sector];
  sector->handle_dl_data(context, grid);
}
