/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "ofh_uplink_request_handler_task_dispatcher.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/support/rtsan.h"

using namespace srsran;
using namespace ofh;

void uplink_request_handler_task_dispatcher::start()
{
  stop_manager.reset();
}

void uplink_request_handler_task_dispatcher::stop()
{
  stop_manager.stop();
}

void uplink_request_handler_task_dispatcher::handle_prach_occasion(const prach_buffer_context& context,
                                                                   prach_buffer&               buffer)
{
  // Do not process if stop was requested.
  if (stop_manager.stop_was_requested()) {
    return;
  }

  if (!executor.defer([context, &buffer, this]() noexcept SRSRAN_RTSAN_NONBLOCKING {
        uplink_handler.handle_prach_occasion(context, buffer);
      })) {
    logger.warning(
        "Sector#{}: failed to handle PRACH in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}

void uplink_request_handler_task_dispatcher::handle_new_uplink_slot(const resource_grid_context& context,
                                                                    const shared_resource_grid&  grid)
{
  // Do not process if stop was requested.
  if (stop_manager.stop_was_requested()) {
    return;
  }
  if (!executor.defer(
          [context, rg = grid.copy(), this, token = stop_manager.get_token()]() noexcept SRSRAN_RTSAN_NONBLOCKING {
            uplink_handler.handle_new_uplink_slot(context, rg);
          })) {
    logger.warning(
        "Sector#{}: failed to handle uplink slot in the uplink request handler for slot '{}'", sector_id, context.slot);
  }
}
