/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_downlink_handler_task_dispatcher.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;
using namespace ofh;

void downlink_handler_task_dispatcher::handle_dl_data(const resource_grid_context& context,
                                                      const shared_resource_grid&  grid)
{
  if (!executor.execute([context, rg = grid.copy(), this]() { dl_handler.handle_dl_data(context, rg); })) {
    srslog::fetch_basic_logger("OFH").warning(
        "Sector#{}: failed to handle downlink data in the downlink handler for slot '{}'", sector_id, context.slot);
  }
}
