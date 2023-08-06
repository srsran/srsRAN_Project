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

#include "ofdm_demodulator_impl.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

ofdm_symbol_demodulator_impl::ofdm_symbol_demodulator_impl(ofdm_demodulator_common_configuration& common_config,
                                                           const ofdm_demodulator_configuration&  ofdm_config) :
  dft_size(ofdm_config.dft_size),
  rg_size(ofdm_config.bw_rb * NRE),
  cp(ofdm_config.cp),
  nof_samples_window_offset(ofdm_config.nof_samples_window_offset),
  scs(to_subcarrier_spacing(ofdm_config.numerology)),
  sampling_rate_Hz(to_sampling_rate_Hz(scs, dft_size)),
  scale(ofdm_config.scale),
  dft(std::move(common_config.dft)),
  phase_compensation_table(to_subcarrier_spacing(ofdm_config.numerology),
                           ofdm_config.cp,
                           ofdm_config.dft_size,
                           ofdm_config.center_freq_hz,
                           false)
{
  report_fatal_error_if_not(std::isnormal(scale), "Invalid scaling factor {}.", scale);
  report_fatal_error_if_not(
      dft_size > rg_size, "The DFT size ({}) must be greater than the resource grid size ({}).", dft_size, rg_size);

  // Fill DFT input with zeros.
  srsvec::zero(dft->get_input());

  // Set the right size to the internal phase compensation buffer.
  compensated_output.resize(dft_size);

  if (ofdm_config.nof_samples_window_offset != 0) {
    // Verify the window is valid.
    srsran_assert(ofdm_config.nof_samples_window_offset < (144 * ofdm_config.dft_size) / 2048,
                  "The DFT window offset (i.e., {}) must be lower than {}.",
                  ofdm_config.nof_samples_window_offset,
                  (144 * ofdm_config.dft_size) / 2048);

    // Prepare phase compensation vector.
    window_phase_compensation.resize(dft_size);

    // Discrete frequency of the complex exponential.
    float omega = static_cast<float>(ofdm_config.nof_samples_window_offset) * static_cast<float>(2.0 * M_PI) /
                  static_cast<float>(dft_size);
    for (unsigned i = 0; i != dft_size; ++i) {
      window_phase_compensation[i] = std::polar(1.0F, omega * static_cast<float>(i));
    }
  }
}

unsigned ofdm_symbol_demodulator_impl::get_cp_offset(unsigned symbol_index, unsigned slot_index) const
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate the offset in samples to the start of the symbol CP within the current slot
  unsigned cp_offset = 0;
  for (unsigned symb_idx = 0; symb_idx != symbol_index; ++symb_idx) {
    cp_offset += cp.get_length(nsymb * slot_index + symb_idx, scs).to_samples(sampling_rate_Hz) + dft_size;
  }

  return cp_offset;
}

void ofdm_symbol_demodulator_impl::demodulate(resource_grid_writer& grid,
                                              span<const cf_t>      input,
                                              unsigned              port_index,
                                              unsigned              symbol_index)
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate cyclic prefix length.
  unsigned cp_len = cp.get_length(symbol_index, scs).to_samples(sampling_rate_Hz);

  // Make sure output buffer matches the symbol size.
  srsran_assert(input.size() == (cp_len + dft_size),
                "The input buffer size ({}) does not match the symbol index {} size ({}+{}={}). SCS={}kHz.",
                input.size(),
                symbol_index,
                cp_len,
                dft_size,
                cp_len + dft_size,
                scs_to_khz(scs));

  // Prepare the DFT inputs, while skipping the cyclic prefix.
  srsvec::copy(dft->get_input().first(dft_size), input.subspan(cp_len - nof_samples_window_offset, dft_size));

  // Execute DFT.
  span<const cf_t> dft_output = dft->run();

  // Get phase correction (TS 138.211, Section 5.4)
  cf_t phase_compensation = phase_compensation_table.get_coefficient(symbol_index);

  // Apply scaling and phase compensation.
  srsvec::sc_prod(dft_output, phase_compensation * scale, compensated_output);

  // Compensate DFT window offset phase shift.
  if (!window_phase_compensation.empty()) {
    srsvec::prod(compensated_output, window_phase_compensation, compensated_output);
  }

  // Map the upper bound frequency domain data.
  span<cf_t> upper_bound(&compensated_output[dft_size - rg_size / 2], rg_size / 2);
  grid.put(port_index, symbol_index % nsymb, 0, upper_bound);

  // Map the lower bound frequency domain data.
  span<cf_t> lower_bound(&compensated_output[0], rg_size / 2);
  grid.put(port_index, symbol_index % nsymb, rg_size / 2, lower_bound);
}

unsigned ofdm_slot_demodulator_impl::get_slot_size(unsigned slot_index) const
{
  unsigned nsymb = get_nsymb_per_slot(cp);
  unsigned count = 0;

  // Iterate all symbols of the slot and accumulate
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    count += symbol_demodulator.get_symbol_size(nsymb * slot_index + symbol_idx);
  }

  return count;
}

void ofdm_slot_demodulator_impl::demodulate(resource_grid_writer& grid,
                                            span<const cf_t>      input,
                                            unsigned              port_index,
                                            unsigned              slot_index)
{
  unsigned nsymb = get_nsymb_per_slot(cp);

  // For each symbol in the slot.
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    // Get the current symbol size.
    unsigned symbol_sz = symbol_demodulator.get_symbol_size(nsymb * slot_index + symbol_idx);

    // Demodulate symbol.
    symbol_demodulator.demodulate(grid, input.first(symbol_sz), port_index, nsymb * slot_index + symbol_idx);

    input = input.last(input.size() - symbol_sz);
  }
}
