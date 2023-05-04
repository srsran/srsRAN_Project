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

#include "downlink_processor_baseband_impl.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"

using namespace srsran;

downlink_processor_baseband_impl::downlink_processor_baseband_impl(
    pdxch_processor_baseband&                        pdxch_proc_baseband_,
    amplitude_controller&                            amplitude_control_,
    const downlink_processor_baseband_configuration& config) :
  amplitude_control_logger(srslog::fetch_basic_logger("Low-PHY")),
  pdxch_proc_baseband(pdxch_proc_baseband_),
  amplitude_control(amplitude_control_),
  nof_slot_tti_in_advance(config.nof_slot_tti_in_advance),
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
    context.slot = current_slot + nof_slot_tti_in_advance;
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

  for (unsigned i_port = 0, i_port_end = temp_buffer.get_nof_channels(); i_port != i_port_end; ++i_port) {
    // Control amplitude.
    amplitude_controller_metrics amplitude_control_metrics =
        amplitude_control.process(temp_buffer.get_channel_buffer(i_port), temp_buffer.get_channel_buffer(i_port));

    // Add entries to long term power statistics when the signal carries power.
    if (amplitude_control_metrics.avg_power_fs > 0.0F) {
      avg_symbol_power.update(amplitude_control_metrics.avg_power_fs);
      peak_symbol_power.update(amplitude_control_metrics.peak_power_fs);
      symbol_papr.update(amplitude_control_metrics.papr_lin);
    }

    // Log amplitude controller metrics every 10 frames.
    if ((current_slot.sfn() % 10 == 0) && (current_slot.slot_index() == 0) && (current_symbol_index == 0)) {
      // Long term average signal power can be computed as the mean of the average power of each OFDM symbol.
      float avg_power = avg_symbol_power.get_mean();

      // Long term peak power is the maximum registered symbol peak power.
      float peak_pwr = peak_symbol_power.get_max();

      float papr = 1.0F;
      if (std::isnormal(avg_power)) {
        // Long term PAPR is the ratio between the maximum observed instantaneous power and the average power. This
        // indicates the dynamic range of the signal in the entire analysis window.
        papr = peak_pwr / avg_power;
      }

      // Mean symbol PAPR. This indicates the mean dynamic range of the signal for an analysis window of 1 OFDM
      // symbol.
      float mean_symb_papr = symbol_papr.get_mean();

      if (amplitude_control_metrics.clipping_enabled) {
        amplitude_control_logger.debug(
            "Tx: gain={:+.2f}dB avg_pwr={:+.2f}dBFS peak_pwr={:+.2f}dBFS papr={:+.2f}dB mean_symb_papr={:+.2f}dB "
            "clip_sps={} clip_prob={:.2e}",
            amplitude_control_metrics.gain_dB,
            convert_power_to_dB(avg_power),
            convert_power_to_dB(peak_pwr),
            convert_power_to_dB(papr),
            convert_power_to_dB(mean_symb_papr),
            amplitude_control_metrics.nof_clipped_samples,
            amplitude_control_metrics.clipping_probability);
      } else {
        amplitude_control_logger.debug(
            "Tx: gain={:+.2f}dB avg_pwr={:+.2f}dBFS peak_pwr={:+.2f}dBFS papr={:+.2f}dB mean_symb_papr={:+.2f}dB",
            amplitude_control_metrics.gain_dB,
            convert_power_to_dB(avg_power),
            convert_power_to_dB(peak_pwr),
            convert_power_to_dB(papr),
            convert_power_to_dB(mean_symb_papr));
      }

      // Reset long term analysis window.
      avg_symbol_power.reset();
      peak_symbol_power.reset();
      symbol_papr.reset();
    }
  }

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