/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "puxch_processor_impl.h"
#include "srsran/gateways/baseband/baseband_gateway_buffer.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/support/resource_grid_context.h"

using namespace srsran;

void puxch_processor_impl::process_symbol(const baseband_gateway_buffer&     samples,
                                          const lower_phy_rx_symbol_context& context)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Renew the current slot and grid if the current slot is invalid or in the past.
  while ((current_slot.numerology() == to_numerology_value(subcarrier_spacing::invalid)) ||
         (context.slot > current_slot)) {
    // Try to get the next request.
    rg_grid_request request;
    if (!request_queue.try_pop(request)) {
      // No request available, then set the context slot and no grid.
      current_slot = context.slot;
      current_grid = nullptr;
    } else {
      // Request available, set the next slot and grid to process.
      current_slot = request.slot;
      current_grid = request.grid;
    }

    // Detect if the slot is in the past.
    if (context.slot > current_slot) {
      // Notify a late request.
      resource_grid_context late_context;
      late_context.slot   = current_slot;
      late_context.sector = context.sector;
      notifier->on_puxch_request_late(late_context);
    }
  }

  // Skip symbol processing if the context slot does not match with the current slot or no resource grid is available.
  if ((context.slot != current_slot) || (current_grid == nullptr)) {
    return;
  }

  // Symbol index within the subframe.
  unsigned symbol_index_subframe = context.nof_symbols + context.slot.subframe_slot_index() * nof_symbols_per_slot;

  // Demodulate each of the ports.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    demodulator->demodulate(*current_grid, samples.get_channel_buffer(i_port), i_port, symbol_index_subframe);
  }

  // Notify.
  lower_phy_rx_symbol_context rx_symbol_context;
  rx_symbol_context.slot        = current_slot;
  rx_symbol_context.nof_symbols = context.nof_symbols;
  notifier->on_rx_symbol(*current_grid, context);
}

void puxch_processor_impl::handle_request(resource_grid& grid, const resource_grid_context& context)
{
  rg_grid_request request = {context.slot, &grid};
  if (!request_queue.try_push(request)) {
    notifier->on_puxch_request_overflow(context);
  }
}
