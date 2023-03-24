/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "downlink_processor_baseband_impl.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"

using namespace srsran;

downlink_processor_baseband_impl::downlink_processor_baseband_impl(
    pdxch_processor_baseband&                        pdxch_proc_baseband_,
    const downlink_processor_baseband_configuration& config) :
  pdxch_proc_baseband(pdxch_proc_baseband_),
  sector_id(config.sector_id),
  nof_rx_ports(config.nof_tx_ports),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  current_nof_samples(0),
  current_symbol_size(0),
  current_symbol_index(0),
  current_slot(to_numerology_value(config.scs), config.initial_slot_index),
  temp_buffer(config.nof_tx_ports, 2 * config.rate.get_dft_size(config.scs))
{
  unsigned symbol_size_no_cp        = config.rate.get_dft_size(config.scs);
  unsigned nof_symbols_per_subframe = nof_symbols_per_slot * get_nof_slots_per_subframe(config.scs);

  // Setup symbol sizes.
  symbol_sizes.reserve(nof_symbols_per_subframe);
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    symbol_sizes.emplace_back(cp_size + symbol_size_no_cp);
  }
}

void downlink_processor_baseband_impl::process(baseband_gateway_buffer& buffer)
{
  srsran_assert(nof_rx_ports == buffer.get_nof_channels(), "Invalid number of channels.");
  unsigned nof_samples           = buffer.get_nof_samples();
  unsigned nof_processed_samples = 0;

  // Process all the input samples.
  while (nof_processed_samples < nof_samples) {
    // If there are no more samples available in the temporary buffer, process a new symbol.
    if (current_symbol_size == current_nof_samples) {
      process_new_symbol();
    }

    // Select the minimum among the remainder of samples to process and the number of samples to complete the buffer.
    unsigned count = std::min(nof_samples - nof_processed_samples, current_symbol_size - current_nof_samples);

    // For each port, concatenate samples.
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      // Select view of the temporary buffer.
      span<const cf_t> temp_buffer_src = temp_buffer.get_channel_buffer(i_port).subspan(current_nof_samples, count);

      // Select view of the output samples.
      span<cf_t> temp_buffer_dst = buffer.get_channel_buffer(i_port).subspan(nof_processed_samples, count);

      // Append input samples into the temporary buffer.
      srsvec::copy(temp_buffer_dst, temp_buffer_src);
    }

    // Increment current number of samples in the buffer and the number of processed samples.
    current_nof_samples += count;
    nof_processed_samples += count;
  }
}

void downlink_processor_baseband_impl::process_new_symbol()
{
  // Detect slot boundary.
  if (current_symbol_index == 0) {
    // Notify slot boundary.
    lower_phy_timing_context context;
    context.slot = current_slot;
    notifier->on_tti_boundary(context);
  }

  // Reset current number of samples in the buffer.
  current_nof_samples = 0;

  // Select current size symbol size.
  current_symbol_size = symbol_sizes[current_symbol_index + current_slot.subframe_slot_index() * nof_symbols_per_slot];

  // Resize temporary buffer.
  temp_buffer.resize(current_symbol_size);

  // Process symbol by PDxCH processor.
  pdxch_processor_baseband::symbol_context pdxch_context;
  pdxch_context.slot   = current_slot;
  pdxch_context.sector = sector_id;
  pdxch_context.symbol = current_symbol_index;
  pdxch_proc_baseband.process_symbol(temp_buffer, pdxch_context);

  // Increment current symbol index.
  ++current_symbol_index;

  // Wrap slot boundary.
  if (current_symbol_index == nof_symbols_per_slot) {
    // Reset symbol counter.
    current_symbol_index = 0;

    // Increment slot.
    ++current_slot;
  }
}