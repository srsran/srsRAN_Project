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

#include "puxch_processor_impl.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_writer.h"

using namespace srsran;

bool puxch_processor_impl::process_symbol(const baseband_gateway_buffer_reader& samples,
                                          const lower_phy_rx_symbol_context&    context)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Check if the slot has changed.
  if (context.slot != current_slot) {
    // Update slot.
    current_slot = context.slot;

    // Exchange an empty request with the current slot with a stored request.
    auto request = requests.exchange({context.slot, shared_resource_grid()});

    // Handle the returned request.
    if (!request.resource) {
      // If the request resource grid pointer is invalid, the request is empty.
      current_grid.release();
    } else if (current_slot != request.slot) {
      // If the slot of the request does not match the current slot, then notify a late event.
      resource_grid_context late_context;
      late_context.slot   = request.slot;
      late_context.sector = context.sector;
      notifier->on_puxch_request_late(late_context);
      current_grid.release();
    } else {
      // If the request is valid, then select request grid.
      current_grid = std::move(request.resource);
    }
  }

  // Skip symbol processing if the context slot does not match with the current slot or no resource grid is available.
  if (!current_grid) {
    return false;
  }

  // Symbol index within the subframe.
  unsigned symbol_index_subframe = context.nof_symbols + context.slot.subframe_slot_index() * nof_symbols_per_slot;

  // Demodulate each of the ports.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    demodulator->demodulate(
        current_grid.get().get_writer(), samples.get_channel_buffer(i_port), i_port, symbol_index_subframe);
  }

  // Notify.
  lower_phy_rx_symbol_context rx_symbol_context;
  rx_symbol_context.slot        = current_slot;
  rx_symbol_context.nof_symbols = context.nof_symbols;
  notifier->on_rx_symbol(current_grid, context, true);

  // Release current grid if the slot is completed.
  if (context.nof_symbols == nof_symbols_per_slot - 1) {
    current_grid.release();
  }

  return true;
}

void puxch_processor_impl::handle_request(const shared_resource_grid& grid, const resource_grid_context& context)
{
  // Ignore request if the processor has stopped.
  if (stopped) {
    return;
  }

  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Swap the new request by the current request in the circular array.
  auto request = requests.exchange({context.slot, grid.copy()});

  // If there was a request at the same request index, notify a late event with the context of the discarded request.
  if (request.resource) {
    resource_grid_context late_context;
    late_context.slot   = request.slot;
    late_context.sector = context.sector;
    notifier->on_puxch_request_late(late_context);
  }
}

lower_phy_center_freq_controller& puxch_processor_impl::get_center_freq_control()
{
  return *this;
}

bool puxch_processor_impl::set_carrier_center_frequency(double carrier_center_frequency_Hz)
{
  demodulator->set_center_frequency(carrier_center_frequency_Hz);
  return true;
}
