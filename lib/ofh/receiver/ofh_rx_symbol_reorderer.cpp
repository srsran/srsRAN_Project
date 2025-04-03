/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_rx_symbol_reorderer.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;
using namespace ofh;

void rx_symbol_reorderer::on_new_uplink_symbol(const uplane_rx_symbol_context& context, shared_resource_grid grid)
{
  std::optional<interval<unsigned>> symbols =
      uplink_symbol_repo->update_rx_symbol_and_compute_symbols_to_notify(context.slot, context.symbol);

  // No symbols to notify.
  if (!symbols) {
    return;
  }

  for (unsigned i = symbols->start(), e = symbols->stop(); i != e; ++i) {
    uplane_rx_symbol_context new_context = context;
    new_context.symbol                   = i;
    notifier.on_new_uplink_symbol(new_context, grid.copy());
  }
}
