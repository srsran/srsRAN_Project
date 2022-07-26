/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofdm_modulator_impl.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

ofdm_symbol_modulator_impl::ofdm_symbol_modulator_impl(ofdm_modulator_common_configuration& common_config,
                                                       const ofdm_modulator_configuration&  ofdm_config) :
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
                           true)
{
  SRSGNB_ALWAYS_ASSERT__(std::isnormal(scale), "Invalid scaling factor {}", scale);
  SRSGNB_ALWAYS_ASSERT__(
      dft_size > rg_size, "The DFT size ({}) must be greater than the resource grid size ({})", dft_size, rg_size);

  // Fill DFT input with zeros.
  srsvec::zero(dft->get_input());
}

void ofdm_symbol_modulator_impl::modulate(span<cf_t>                  output,
                                          const resource_grid_reader& grid,
                                          unsigned                    port_index,
                                          unsigned                    symbol_index)
{
  // Calculate number of symbols per slot.
  unsigned nsymb = get_nsymb_per_slot(cp);

  // Calculate cyclic prefix length.
  unsigned cp_len = cp.get_length(symbol_index, numerology, dft_size);

  // Make sure output buffer matches the symbol size.
  SRSGNB_ALWAYS_ASSERT__(
      output.size() == (cp_len + dft_size),
      "The output buffer size ({}) does not match the symbol index {} size ({}+{}={}). Numerology={}.",
      output.size(),
      symbol_index,
      cp_len,
      dft_size,
      cp_len + dft_size,
      numerology);

  // Skip modulator if the grid is empty for the given port.
  if (grid.is_empty(port_index)) {
    srsvec::zero(output);
    return;
  }

  // Prepare lower bound frequency domain data.
  grid.get(dft->get_input().last(rg_size / 2), port_index, symbol_index % nsymb, 0);

  // Prepare upper bound frequency domain data.
  grid.get(dft->get_input().first(rg_size / 2), port_index, symbol_index % nsymb, rg_size / 2);

  // Execute DFT.
  span<const cf_t> dft_output = dft->run();

  // Get phase correction (TS 138.211, Section 5.4)
  cf_t phase_compensation = phase_compensation_table.get_coefficient(symbol_index);

  // Apply scaling and phase compensation.
  srsvec::sc_prod(dft_output, phase_compensation * scale, output.last(dft_size));

  // Copy cyclic prefix.
  srsvec::copy(output.first(cp_len), output.last(cp_len));
}

unsigned ofdm_slot_modulator_impl::get_slot_size(unsigned slot_index) const
{
  unsigned nsymb = get_nsymb_per_slot(cp);
  unsigned count = 0;

  // Iterate all symbols of the slot and accumulate
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    count += symbol_modulator.get_symbol_size(nsymb * slot_index + symbol_idx);
  }

  return count;
}

void ofdm_slot_modulator_impl::modulate(span<cf_t>                  output,
                                        const resource_grid_reader& grid,
                                        unsigned                    port_index,
                                        unsigned                    slot_index)
{
  unsigned nsymb = get_nsymb_per_slot(cp);

  unsigned nslots_per_subframe = get_nof_slots_per_subframe(to_subcarrier_spacing(numerology));
  srsgnb_assert(slot_index < nslots_per_subframe,
                "Slot index within the subframe {} exceeds the number of slots per subframe {}.",
                slot_index,
                nslots_per_subframe);

  // For each symbol in the slot.
  for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
    // Get the current symbol size.
    unsigned symbol_sz = symbol_modulator.get_symbol_size(nsymb * slot_index + symbol_idx);

    // Modulate symbol.
    symbol_modulator.modulate(output.first(symbol_sz), grid, port_index, nsymb * slot_index + symbol_idx);

    // Advance output buffer.
    output = output.last(output.size() - symbol_sz);
  }
}
