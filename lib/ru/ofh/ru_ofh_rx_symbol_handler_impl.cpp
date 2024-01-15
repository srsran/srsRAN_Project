/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ru_ofh_rx_symbol_handler_impl.h"

using namespace srsran;

void ru_ofh_rx_symbol_handler_impl::on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context,
                                                         const resource_grid_reader&          grid)
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
