/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplink_request_handler_task_dispatcher.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;
using namespace ofh;

void uplink_request_handler_task_dispatcher::handle_prach_occasion(const prach_buffer_context& context,
                                                                   prach_buffer&               buffer)
{
  if (!executor.execute([context, &buffer, this]() { uplink_handler.handle_prach_occasion(context, buffer); })) {
    logger.warning(
        "Sector#{}: failed to handle PRACH in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}

void uplink_request_handler_task_dispatcher::handle_new_uplink_slot(const resource_grid_context& context,
                                                                    const shared_resource_grid&  grid)
{
  if (!executor.execute([context, rg = grid.copy(), this]() { uplink_handler.handle_new_uplink_slot(context, rg); })) {
    logger.warning(
        "Sector#{}: failed to handle uplink slot in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}
