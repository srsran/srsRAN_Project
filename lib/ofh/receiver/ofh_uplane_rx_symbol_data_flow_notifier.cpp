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

#include "ofh_uplane_rx_symbol_data_flow_notifier.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

using namespace srsran;
using namespace ofh;

void uplane_rx_symbol_data_flow_notifier::notify_received_symbol(slot_point slot, unsigned symbol)
{
  expected<uplink_context::uplink_context_resource_grid_info> context =
      ul_context_repo->try_popping_complete_resource_grid_symbol(slot, symbol);

  if (!context) {
    return;
  }

  uplink_context::uplink_context_resource_grid_info& ctx_value = *context;
  uplane_rx_symbol_context notification_context = {ctx_value.context.slot, symbol, ctx_value.context.sector};
  notifier->on_new_uplink_symbol(notification_context, std::move(ctx_value.grid), true);

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Notifying UL symbol in slot '{}', symbol '{}' for sector#{}",
                 notification_context.slot,
                 notification_context.symbol,
                 notification_context.sector);
  }
}
