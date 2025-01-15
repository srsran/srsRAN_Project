/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_rx_symbol_handler_impl.h"
#include "srsran/phy/support/shared_resource_grid.h"

using namespace srsran;

void ru_ofh_rx_symbol_handler_impl::on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context,
                                                         shared_resource_grid                 grid)
{
  ru_uplink_rx_symbol_context ru_context;
  ru_context.sector    = context.sector;
  ru_context.slot      = context.slot;
  ru_context.symbol_id = context.symbol;

  notifier.on_new_uplink_symbol(ru_context, grid);
}

void ru_ofh_rx_symbol_handler_impl::on_new_prach_window_data(const prach_buffer_context& context,
                                                             const prach_buffer&         buffer)
{
  notifier.on_new_prach_window_data(context, buffer);
}
