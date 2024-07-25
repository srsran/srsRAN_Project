/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_rx_symbol_data_flow_notifier.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

using namespace srsran;
using namespace ofh;

void uplane_rx_symbol_data_flow_notifier::notify_received_symbol(slot_point slot, unsigned symbol)
{
  expected<uplink_context::uplink_context_resource_grid_info> context =
      ul_context_repo->try_poping_complete_resource_grid_symbol(slot, symbol);

  if (!context.has_value()) {
    return;
  }

  const auto               ctx_value            = context.value();
  uplane_rx_symbol_context notification_context = {ctx_value.context.slot, symbol, ctx_value.context.sector};
  notifier->on_new_uplink_symbol(notification_context, ctx_value.grid->get_reader());

  logger.debug("Notifying UL symbol in slot '{}', symbol '{}' for sector#{}",
               notification_context.slot,
               notification_context.symbol,
               notification_context.sector);
}
