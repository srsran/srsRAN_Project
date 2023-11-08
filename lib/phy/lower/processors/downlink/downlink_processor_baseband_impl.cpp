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
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer_view.h"
#include "srsran/phy/lower/lower_phy_timing_context.h"

using namespace srsran;

downlink_processor_baseband_impl::downlink_processor_baseband_impl(
    pdxch_processor_baseband&                        pdxch_proc_baseband_,
    amplitude_controller&                            amplitude_control_,
    const downlink_processor_baseband_configuration& config) :
  amplitude_control_logger(*config.logger),
  pdxch_proc_baseband(pdxch_proc_baseband_),
  amplitude_control(amplitude_control_),
  nof_slot_tti_in_advance(config.nof_slot_tti_in_advance),
  sector_id(config.sector_id),
  scs(config.scs),
  nof_rx_ports(config.nof_tx_ports),
  nof_samples_per_subframe(config.rate.to_kHz()),
  nof_slots_per_subframe(get_nof_slots_per_subframe(config.scs)),
  nof_symbols_per_slot(get_nsymb_per_slot(config.cp)),
  temp_buffer(config.nof_tx_ports, 2 * config.rate.get_dft_size(config.scs))
{
  unsigned symbol_size_no_cp        = config.rate.get_dft_size(config.scs);
  unsigned nof_symbols_per_subframe = nof_symbols_per_slot * nof_slots_per_subframe;

  // Setup symbol sizes.
  symbol_sizes.reserve(nof_symbols_per_subframe);
  for (unsigned i_symbol = 0; i_symbol != nof_symbols_per_subframe; ++i_symbol) {
    unsigned cp_size = config.cp.get_length(i_symbol, config.scs).to_samples(config.rate.to_Hz());
    symbol_sizes.emplace_back(cp_size + symbol_size_no_cp);
  }
}

void downlink_processor_baseband_impl::process(baseband_gateway_buffer_writer& buffer,
                                               baseband_gateway_timestamp      timestamp)
{
  srsran_assert(nof_rx_ports == buffer.get_nof_channels(), "Invalid number of channels.");
  unsigned nof_output_samples = buffer.get_nof_samples();

  // Counter for the number of samples written to the output buffer.
  unsigned nof_written_samples = 0;

  // Process all the input samples.
  while (nof_written_samples < nof_output_samples) {
    // Timestamp of the remaining samples to process.
    baseband_gateway_timestamp proc_timestamp = timestamp + nof_written_samples;

    // If there are no samples available in the temporary buffer, process a new symbol.
    if (temp_buffer.get_nof_available_samples(proc_timestamp) == 0) {
      // Calculate the subframe index.
      auto i_sf =
          static_cast<unsigned>((proc_timestamp / nof_samples_per_subframe) % (NOF_SFNS * NOF_SUBFRAMES_PER_FRAME));
      // Calculate the sample index within the subframe.
      unsigned i_sample_sf = proc_timestamp % nof_samples_per_subframe;

      // Calculate symbol index within the subframe and the sample index within the OFDM symbol.
      unsigned i_sample_symbol = i_sample_sf;
      unsigned i_symbol_sf     = 0;
      while (i_sample_symbol >= symbol_sizes[i_symbol_sf]) {
        i_sample_symbol -= symbol_sizes[i_symbol_sf];
        ++i_symbol_sf;
      }

      // Calculate system slot index and the symbol index within the slot.
      unsigned i_slot   = i_sf * nof_slots_per_subframe + i_symbol_sf / nof_symbols_per_slot;
      unsigned i_symbol = i_symbol_sf % nof_symbols_per_slot;

      // Create slot point.
      slot_point slot(to_numerology_value(scs), i_slot);

      // Detect slot boundary.
      if (i_symbol == 0) {
        // Notify slot boundary.
        lower_phy_timing_context context;
        context.slot = slot + nof_slot_tti_in_advance;
        notifier->on_tti_boundary(context);
      }

      // Number of samples of the symbol to process.
      unsigned symbol_nof_samples = symbol_sizes[i_symbol_sf];

      if ((buffer.get_nof_samples() - nof_written_samples >= symbol_nof_samples) && (i_sample_symbol == 0)) {
        // If The destination buffer is large enough to hold a new symbol and the process timestamp is aligned with the
        // symbol start, skip copying to a temporary buffer.
        baseband_gateway_buffer_writer_view dest_buffer(buffer, nof_written_samples, symbol_nof_samples);
        process_new_symbol(dest_buffer, slot, i_symbol);
        nof_written_samples += symbol_nof_samples;

      } else {
        // Clear the temporary buffer of previously stored samples.
        temp_buffer.clear();

        // Timestamp of the first sample of the current OFDM symbol.
        baseband_gateway_timestamp symbol_timestamp = proc_timestamp - i_sample_symbol;

        // Write the symbol into the temporary buffer.
        baseband_gateway_buffer_writer& dest_buffer = temp_buffer.write_symbol(symbol_timestamp, symbol_nof_samples);
        process_new_symbol(dest_buffer, slot, i_symbol);
      }
    }

    // Read samples from the temporary buffer if available.
    if (temp_buffer.get_nof_available_samples(proc_timestamp) > 0) {
      // Output buffer view starting at the current writing position.
      baseband_gateway_buffer_writer_view dest_buffer(
          buffer, nof_written_samples, nof_output_samples - nof_written_samples);

      // Read from the temporary buffer.
      nof_written_samples += temp_buffer.read(dest_buffer, proc_timestamp);
    }
  }
}

void downlink_processor_baseband_impl::process_new_symbol(baseband_gateway_buffer_writer& buffer,
                                                          slot_point                      slot,
                                                          unsigned                        i_symbol)
{
  // Process symbol by PDxCH processor.
  pdxch_processor_baseband::symbol_context pdxch_context;
  pdxch_context.slot   = slot;
  pdxch_context.sector = sector_id;
  pdxch_context.symbol = i_symbol;

  pdxch_proc_baseband.process_symbol(buffer, pdxch_context);

  // Process amplitude control.
  for (unsigned i_port = 0, i_port_end = buffer.get_nof_channels(); i_port != i_port_end; ++i_port) {
    // Control amplitude.
    amplitude_controller_metrics amplitude_control_metrics = amplitude_control.process(buffer[i_port], buffer[i_port]);

    // Add entries to long term power statistics when the signal carries power.
    if (amplitude_control_metrics.avg_power_fs > 0.0F) {
      avg_symbol_power.update(amplitude_control_metrics.avg_power_fs);
      peak_symbol_power.update(amplitude_control_metrics.peak_power_fs);
      symbol_papr.update(amplitude_control_metrics.papr_lin);
    }

    // Log amplitude controller metrics every 100 subframes (every 100 milliseconds).
    unsigned i_sf      = slot.subframe_index();
    unsigned i_slot_sf = slot.subframe_slot_index();
    if ((avg_symbol_power.get_nof_observations() > 0) && (i_sf % 100 == 0) && (i_slot_sf == 0) && (i_symbol == 0)) {
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
}