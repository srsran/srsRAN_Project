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

#include "uplink_processor_impl.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/uplink_processor_notifier.h"

using namespace srsran;

lower_phy_uplink_processor_impl::lower_phy_uplink_processor_impl(std::unique_ptr<prach_processor> prach_proc_,
                                                                 std::unique_ptr<puxch_processor> puxch_proc_,
                                                                 const configuration&             config) :
  sector_id(config.sector_id),
  nof_rx_ports(config.nof_rx_ports),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  nof_symbols_per_subframe(nof_symbols_per_slot * get_nof_slots_per_subframe(config.scs)),
  current_nof_samples(0),
  current_symbol_index(0),
  current_slot(to_numerology_value(config.scs), config.initial_slot_index),
  temp_buffer(config.nof_rx_ports, 2 * config.rate.get_dft_size(config.scs)),
  prach_proc(std::move(prach_proc_)),
  puxch_proc(std::move(puxch_proc_))
{
  srsran_assert(prach_proc, "Invalid PRACH processor.");
  srsran_assert(puxch_proc, "Invalid PUxCH processor.");

  unsigned symbol_size_no_cp = config.rate.get_dft_size(config.scs);

  // Setup symbol sizes.
  symbol_sizes.reserve(nof_symbols_per_subframe);
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    symbol_sizes.emplace_back(cp_size + symbol_size_no_cp);
  }
}

void lower_phy_uplink_processor_impl::connect(uplink_processor_notifier& notifier_,
                                              prach_processor_notifier&  prach_notifier,
                                              puxch_processor_notifier&  puxch_notifier)
{
  notifier = &notifier_;
  prach_proc->connect(prach_notifier);
  puxch_proc->connect(puxch_notifier);
}

prach_processor_request_handler& lower_phy_uplink_processor_impl::get_prach_request_handler()
{
  return prach_proc->get_request_handler();
}

puxch_processor_request_handler& lower_phy_uplink_processor_impl::get_puxch_request_handler()
{
  return puxch_proc->get_request_handler();
}

uplink_processor_baseband& lower_phy_uplink_processor_impl::get_baseband()
{
  return *this;
}

void lower_phy_uplink_processor_impl::process(const baseband_gateway_buffer& samples)
{
  srsran_assert(notifier != nullptr, "Notifier has not been connected.");
  srsran_assert(nof_rx_ports == samples.get_nof_channels(), "Invalid number of channels.");

  unsigned nof_samples           = samples.get_nof_samples();
  unsigned nof_processed_samples = 0;

  // Process all the input samples.
  while (nof_processed_samples < nof_samples) {
    // Select current OFDM symbol size.
    unsigned current_symbol_size =
        symbol_sizes[current_symbol_index + current_slot.subframe_slot_index() * nof_symbols_per_slot];
    temp_buffer.resize(current_symbol_size);

    // Select the minimum among the remainder of samples to process and the number of samples to complete the buffer.
    unsigned count = std::min(nof_samples - nof_processed_samples, current_symbol_size - current_nof_samples);

    // For each port, concatenate samples.
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      // Select view of the temporary buffer.
      span<cf_t> temp_buffer_dst = temp_buffer.get_channel_buffer(i_port).subspan(current_nof_samples, count);

      // Select view of the input samples.
      span<const cf_t> temp_buffer_src = samples.get_channel_buffer(i_port).subspan(nof_processed_samples, count);

      // Append input samples into the temporary buffer.
      srsvec::copy(temp_buffer_dst, temp_buffer_src);
    }

    // Increment current number of samples in the buffer and the number of processed samples.
    current_nof_samples += count;
    nof_processed_samples += count;

    // Process PUxCH and PRACH symbols if the symbol is complete.
    if (current_nof_samples == current_symbol_size) {
      process_new_symbol();
    }
  }
}

void lower_phy_uplink_processor_impl::process_new_symbol()
{
  // Reset current number of samples in the buffer.
  current_nof_samples = 0;

  // Process symbol by PRACH processor.
  prach_processor_baseband::symbol_context prach_context;
  prach_context.slot   = current_slot;
  prach_context.symbol = current_symbol_index;
  prach_context.sector = sector_id;
  prach_context.port   = 0;
  prach_proc->get_baseband().process_symbol(temp_buffer.get_channel_buffer(0), prach_context);

  // Process symbol by PUxCH processor.
  lower_phy_rx_symbol_context puxch_context;
  puxch_context.slot        = current_slot;
  puxch_context.sector      = sector_id;
  puxch_context.nof_symbols = current_symbol_index;
  puxch_proc->get_baseband().process_symbol(temp_buffer, puxch_context);

  // Increment current symbol index and wrap slot boundary.
  current_symbol_index = (current_symbol_index + 1) % nof_symbols_per_slot;

  // Detect half-slot boundary.
  if (current_symbol_index == nof_symbols_per_slot / 2) {
    // Notify half slot boundary.
    lower_phy_timing_context context;
    context.slot = current_slot;
    notifier->on_half_slot(context);
  }

  // Detect full slot boundary.
  if (current_symbol_index == 0) {
    // Notify full slot boundary.
    lower_phy_timing_context context;
    context.slot = current_slot;
    notifier->on_full_slot(context);

    // Increment slot.
    ++current_slot;
  }
}
