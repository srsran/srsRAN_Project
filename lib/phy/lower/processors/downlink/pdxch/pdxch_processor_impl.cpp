/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdxch_processor_impl.h"
#include "srsran/phy/support/resource_grid_reader_empty.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

const resource_grid_reader_empty pdxch_processor_impl::empty_rg(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_RB);

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

void pdxch_processor_impl::process_symbol(baseband_gateway_buffer_writer&                 samples,
                                          const pdxch_processor_baseband::symbol_context& context)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Check if the slot has changed.
  if (context.slot != current_slot) {
    // Update slot.
    current_slot = context.slot;

    // Exchange an empty request with the current slot with a stored request.
    auto request = requests.exchange({context.slot, nullptr});

    // Handle the returned request.
    if (request.grid == nullptr) {
      // If the request resource grid pointer is nullptr, the request is empty.
      current_grid = &empty_rg;
    } else if (current_slot != request.slot) {
      // If the slot of the request does not match the current slot, then notify a late event.
      resource_grid_context late_context;
      late_context.slot   = request.slot;
      late_context.sector = context.sector;
      notifier->on_pdxch_request_late(late_context);
      current_grid = &empty_rg;
    } else {
      // If the request is valid, then select request grid.
      current_grid = request.grid;
    }
  }

  // Symbol index within the subframe.
  unsigned symbol_index_subframe = context.symbol + context.slot.subframe_slot_index() * nof_symbols_per_slot;

  // Modulate each of the ports.
  for (unsigned i_port = 0; i_port != nof_tx_ports; ++i_port) {
    modulator->modulate(samples.get_channel_buffer(i_port), *current_grid, i_port, symbol_index_subframe);
  }
}

void pdxch_processor_impl::handle_request(const resource_grid_reader& grid, const resource_grid_context& context)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");

  // Swap the new request by the current request in the circular array.
  auto request = requests.exchange({context.slot, &grid});

  // If there was a request with a resource grid, then notify a late event with the context of the discarded request.
  if (request.grid != nullptr) {
    resource_grid_context late_context;
    late_context.slot   = request.slot;
    late_context.sector = context.sector;
    notifier->on_pdxch_request_late(late_context);
  }
}
