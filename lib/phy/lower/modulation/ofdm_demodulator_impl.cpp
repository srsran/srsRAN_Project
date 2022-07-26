/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofdm_demodulator_impl.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

ofdm_symbol_demodulator_impl::ofdm_symbol_demodulator_impl(ofdm_demodulator_common_configuration& common_config,
                                                           const ofdm_demodulator_configuration&  ofdm_config) :
  dft_size(ofdm_config.dft_size),
  rg_size(ofdm_config.bw_rb * NRE),
  cp(ofdm_config.cp),
  numerology(ofdm_config.numerology),
  scale(ofdm_config.scale),
  dft(std::move(common_config.dft)),
  phase_compensation_table(to_subcarrier_spacing(ofdm_config.numerology),
                           ofdm_config.cp,
                           ofdm_config.dft_size,
                           ofdm_config.center_freq_hz,
                           false)
{
  report_fatal_error_if_not(std::isnormal(scale), "Invalid scaling factor %f", scale);
  report_fatal_error_if_not(
      dft_size > rg_size, "The DFT size (%d) must be greater than the resource grid size (%d)", dft_size, rg_size);

  // Fill DFT input with zeros.
  srsvec::zero(dft->get_input());

  // Set the right size to the internal phase compensation buffer.
  compensated_output.resize(dft_size);
}

unsigned ofdm_symbol_demodulator_impl::get_cp_offset(unsigned symbol_index, unsigned slot_index) const
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate the offset in samples to the start of the symbol CP within the current slot
  unsigned cp_offset = 0;
  for (unsigned symb_idx = 0; symb_idx != symbol_index; ++symb_idx) {
    cp_offset += cp.get_length(nsymb * slot_index + symb_idx, numerology, dft_size) + dft_size;
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
  unsigned cp_len = cp.get_length(symbol_index, numerology, dft_size);

  // Make sure output buffer matches the symbol size.
  report_fatal_error_if_not(
      input.size() == (cp_len + dft_size),
      "The input buffer size ({}) does not match the symbol index {} size ({}+{}={}). Numerology={}.",
      input.size(),
      symbol_index,
      cp_len,
      dft_size,
      cp_len + dft_size,
      numerology);

  // Prepare the DFT inputs, while skipping the cyclic prefix.
  srsvec::copy(dft->get_input().first(dft_size), input.last(dft_size));

  // Execute DFT.
  span<const cf_t> dft_output = dft->run();

  // Get phase correction (TS 138.211, Section 5.4)
  cf_t phase_compensation = phase_compensation_table.get_coefficient(symbol_index);

  // Apply scaling and phase compensation.
  srsvec::sc_prod(dft_output, phase_compensation * scale, compensated_output);

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

    // Get the offset to the start of the (CP of the) current symbol within the slot.
    unsigned offset = symbol_demodulator.get_cp_offset(symbol_idx, slot_index);

    // Demodulate symbol.
    symbol_demodulator.demodulate(grid, input.subspan(offset, symbol_sz), port_index, nsymb * slot_index + symbol_idx);
  }
}