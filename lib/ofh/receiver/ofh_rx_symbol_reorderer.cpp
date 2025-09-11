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

#include "ofh_rx_symbol_reorderer.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;
using namespace ofh;

void rx_symbol_reorderer::on_new_uplink_symbol(const uplane_rx_symbol_context& context,
                                               shared_resource_grid            grid,
                                               bool                            is_valid)
{
  std::optional<uplink_notified_grid_symbol_repository::results> symbols =
      uplink_symbol_repo->update_rx_symbol_and_compute_symbols_to_notify(context.slot, context.symbol, is_valid);

  // No symbols to notify.
  if (!symbols) {
    return;
  }

  for (unsigned i = symbols->symbols.start(), e = symbols->symbols.stop(); i != e; ++i) {
    uplane_rx_symbol_context new_context = context;
    new_context.symbol                   = i;
    notifier.on_new_uplink_symbol(new_context, grid.copy(), symbols->valid_symbols.test(i));
  }
}
