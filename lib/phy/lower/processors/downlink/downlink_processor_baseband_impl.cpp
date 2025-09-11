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

#include "downlink_processor_baseband_impl.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "srsran/instrumentation/traces/ru_traces.h"
#include "srsran/phy/lower/lower_phy_baseband_metrics.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

downlink_processor_baseband_impl::downlink_processor_baseband_impl(
    pdxch_processor_baseband&                        pdxch_proc_baseband_,
    const downlink_processor_baseband_configuration& config) :
  pdxch_proc_baseband(pdxch_proc_baseband_),
  nof_slot_tti_in_advance(config.nof_slot_tti_in_advance),
  nof_slot_tti_in_advance_ns(config.nof_slot_tti_in_advance * 1000000 /
                             slot_point(config.scs, 0).nof_slots_per_subframe()),
  sector_id(config.sector_id),
  rate(config.rate),
  scs(config.scs),
  nof_samples_per_subframe(config.rate.to_kHz()),
  nof_slots_per_subframe(get_nof_slots_per_subframe(config.scs)),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  temp_buffer(config.nof_tx_ports, 2 * config.rate.get_dft_size(config.scs)),
  cfo_processor(config.rate),
  buffer_pool(nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME, config.nof_tx_ports, nof_samples_per_subframe)
{
  unsigned symbol_size_no_cp        = config.rate.get_dft_size(config.scs);
  unsigned nof_symbols_per_subframe = nof_symbols_per_slot * nof_slots_per_subframe;

  // Setup symbol sizes.
  symbol_sizes_sf.reserve(nof_symbols_per_subframe);
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    symbol_sizes_sf.emplace_back(cp_size + symbol_size_no_cp);
  }
}

// Fills the unprocessed reagions of a baseband buffer with zeros, according to the downink processor baseband metadata.
static void fill_zeros(baseband_gateway_buffer_writer& buffer, const baseband_gateway_transmitter_metadata& md)
{
  // If discontinous mode is disabled, fill the non-processed regions with zeros and report full buffer metadata.
  if (md.is_empty) {
    for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
      srsvec::zero(buffer.get_channel_buffer(i_channel));
    }
  } else {
    if (md.tx_start.has_value()) {
      for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
        srsvec::zero(buffer.get_channel_buffer(i_channel).first(*md.tx_start));
      }
    }
    if (md.tx_end.has_value()) {
      for (unsigned i_channel = 0, i_channel_end = buffer.get_nof_channels(); i_channel != i_channel_end; ++i_channel) {
        srsvec::zero(buffer.get_channel_buffer(i_channel).last(buffer.get_nof_samples() - *md.tx_end));
      }
    }
  }
}

downlink_processor_baseband::processing_result
downlink_processor_baseband_impl::process(baseband_gateway_timestamp timestamp)
{
  // Calculate an adjusted timestamp for the samples to be generated. The transmit time offset is subtracted from
  // the requested buffer timestamp to shift the signal in time. The generated signal will then be stored in the
  // destination buffer according to the timestamp at which it should be transmitted.
  baseband_gateway_timestamp proc_timestamp_offset  = timestamp;
  int                        current_tx_time_offset = tx_time_offset.load(std::memory_order::memory_order_relaxed);
  if ((current_tx_time_offset < 0) ||
      (static_cast<baseband_gateway_timestamp>(current_tx_time_offset) < proc_timestamp_offset)) {
    // Make sure the subtraction does not overflow.
    proc_timestamp_offset -= current_tx_time_offset;
  }

  // Calculate the subframe index.
  auto i_sf =
      static_cast<unsigned>((proc_timestamp_offset / nof_samples_per_subframe) % (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));
  // Calculate the sample index within the subframe.
  unsigned i_sample_sf = proc_timestamp_offset % nof_samples_per_subframe;

  // Calculate symbol index within the subframe and the sample index within the OFDM symbol.
  unsigned i_sample_symbol = i_sample_sf;
  unsigned i_symbol_sf     = 0;
  while (i_sample_symbol >= symbol_sizes_sf[i_symbol_sf]) {
    i_sample_symbol -= symbol_sizes_sf[i_symbol_sf];
    ++i_symbol_sf;
  }

  // Calculate system slot index and the symbol index within the slot.
  unsigned i_slot   = i_sf * nof_slots_per_subframe + i_symbol_sf / nof_symbols_per_slot;
  unsigned i_symbol = i_symbol_sf % nof_symbols_per_slot;

  // If the timestamp is not aligned with the beginning of a slot, Align with the next subframe.
  if ((i_symbol != 0) || (i_sample_symbol != 0)) {
    // Prepare result metadata and obtain baseband buffer from the pool.
    processing_result result = {};
    result.metadata.ts       = timestamp;
    result.metadata.is_empty = true;
    result.buffer            = buffer_pool.get();
    report_fatal_error_if_not(result.buffer, "Failed to retrieve a baseband buffer.");

    // Calculate the number of samples to the next subframe.
    unsigned nof_samples_to_next_sf = nof_samples_per_subframe - i_sample_sf;

    // Resize buffer and zero.
    result.buffer->resize(nof_samples_to_next_sf);
    fill_zeros(result.buffer->get_writer(), result.metadata);

    return result;
  }

  // Create slot point.
  slot_point slot(to_numerology_value(scs), i_slot);

  // Notify slot boundary.
  trace_point              tp = ru_tracer.now();
  lower_phy_timing_context context;
  context.slot       = slot + nof_slot_tti_in_advance;
  context.time_point = std::chrono::system_clock::now() + nof_slot_tti_in_advance_ns;
  notifier->on_tti_boundary(context);
  ru_tracer << trace_event("on_tti_boundary", tp);

  // Obtain the downlink baseband processing for the slot.
  auto slot_result = pdxch_proc_baseband.process_slot({.slot = slot, .sector = sector_id});

  // If the results do not contain any buffer, then return a buffer containing zeros.
  if (!slot_result.buffer) {
    // Prepare result metadata and obtain baseband buffer from the pool.
    processing_result result = {};
    result.metadata.ts       = timestamp;
    result.metadata.is_empty = true;
    result.buffer            = buffer_pool.get();
    report_fatal_error_if_not(result.buffer, "Failed to retrieve a baseband buffer.");

    // Calculate the slot number of samples.
    span<unsigned> symbol_sizes_slot = span<unsigned>(symbol_sizes_sf).subspan(i_symbol_sf, nof_symbols_per_slot);
    unsigned       nof_samples_slot  = std::accumulate(symbol_sizes_slot.begin(), symbol_sizes_slot.end(), 0U);

    // Resize buffer and zero.
    result.buffer->resize(nof_samples_slot);
    fill_zeros(result.buffer->get_writer(), result.metadata);

    return result;
  }

  // Notify metrics.
  notifier->on_new_metrics(slot_result.metrics);

  // Prepare result metadata.
  processing_result result = {};
  result.metadata.ts       = timestamp;
  result.metadata.is_empty = false;
  result.buffer            = std::move(slot_result.buffer);
  return result;
}

void downlink_processor_baseband_impl::set_tx_time_offset(phy_time_unit tx_time_offset_)
{
  tx_time_offset = tx_time_offset_.to_nearest_samples(rate.to_Hz());
}
