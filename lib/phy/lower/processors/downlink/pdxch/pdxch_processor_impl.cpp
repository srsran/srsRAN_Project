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

#include "pdxch_processor_impl.h"
#include "srsran/phy/support/resource_grid_reader_empty.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

const resource_grid_reader_empty pdxch_processor_impl::empty_rg;

void pdxch_processor_impl::connect(pdxch_processor_notifier& notifier_)
{
  notifier = &notifier_;
}

pdxch_processor_request_handler& pdxch_processor_impl::get_request_handler()
{
  return *this;
}

pdxch_processor_baseband& pdxch_processor_impl::get_baseband()
{
  return *this;
}

void pdxch_processor_impl::process_symbol(baseband_gateway_buffer&                        samples,
                                          const pdxch_processor_baseband::symbol_context& context)
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
      current_grid = &empty_rg;
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
      notifier->on_late_resource_grid(late_context);
    }
  }

  // Use empty grid if the context slot does not match with the current slot or no resource grid is available.
  const resource_grid_reader* rg_reader = current_grid;
  if ((context.slot != current_slot) || (rg_reader == nullptr)) {
    rg_reader = &empty_rg;
  }

  // Symbol index within the subframe.
  unsigned symbol_index_subframe = context.symbol + context.slot.subframe_slot_index() * nof_symbols_per_slot;

  // Modulate each of the ports.
  for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
    modulator->modulate(samples.get_channel_buffer(i_port), *rg_reader, i_port, symbol_index_subframe);
  }
}

void pdxch_processor_impl::handle_request(const resource_grid_reader& grid, const resource_grid_context& context)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  rg_grid_request request = {context.slot, &grid};
  if (!request_queue.try_push(request)) {
    notifier->on_overflow_resource_grid(context);
  }
}
