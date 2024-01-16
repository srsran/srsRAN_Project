/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdsch_modulator_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/srsvec/bit.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

const bit_buffer& pdsch_modulator_impl::scramble(const bit_buffer& b, unsigned q, const config_t& config)
{
  temp_b_hat.resize(b.size());

  // Calculate initial scrambling state.
  unsigned c_init = (static_cast<unsigned>(config.rnti) << 15U) + (q << 14U) + config.n_id;

  // Initialize scrambling sequence.
  scrambler->init(c_init);

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_b_hat, b);

  return temp_b_hat;
}

float pdsch_modulator_impl::modulate(span<ci8_t> d_pdsch, const bit_buffer& b_hat, modulation_scheme modulation)
{
  // Actual modulate.
  return modulator->modulate(d_pdsch, b_hat, modulation);
}

void pdsch_modulator_impl::map(resource_grid_mapper& mapper,
                               span<const ci8_t>     data_re,
                               float                 scaling,
                               const config_t&       config)
{
  // Get the PRB allocation mask.
  const bounded_bitset<MAX_RB> prb_allocation_mask =
      config.freq_allocation.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);

  // First symbol used in this transmission.
  unsigned start_symbol_index = config.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = config.start_symbol_index + config.nof_symbols;

  srsran_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission [{}, {}) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(start_symbol_index, end_symbol_index);

  // Allocation pattern for the mapper.
  re_pattern_list allocation;
  re_pattern      pdsch_pattern;

  // Reserved REs, including DM-RS and CSI-RS.
  re_pattern_list reserved(config.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = config.dmrs_config_type.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symb_pos);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  // Set PDSCH allocation pattern.
  pdsch_pattern.prb_mask = prb_allocation_mask;
  pdsch_pattern.re_mask  = ~re_prb_mask();
  pdsch_pattern.symbols  = symbols;
  allocation.merge(pdsch_pattern);

  if (std::isnormal(config.scaling)) {
    scaling *= config.scaling;
  }
  precoding_configuration precoding2 = config.precoding;
  precoding2 *= scaling;

  resource_grid_mapper::symbol_buffer_adapter buffer_adapter(data_re);

  // Map into the resource grid.
  mapper.map(buffer_adapter, allocation, reserved, precoding2);
}

void pdsch_modulator_impl::modulate(resource_grid_mapper&            mapper,
                                    span<const bit_buffer>           codewords,
                                    const pdsch_modulator::config_t& config)
{
  srsran_assert(codewords.size() == 1, "Only one PDSCH codeword is currently supported");

  modulation_scheme mod = config.modulation1;
  unsigned          Qm  = get_bits_per_symbol(mod);

  // Calculate number of REs.
  unsigned nof_bits = codewords[0].size();
  unsigned nof_re   = nof_bits / Qm;

  // Scramble.
  const bit_buffer& b_hat = scramble(codewords[0], 0, config);

  // View over the PDSCH symbols buffer. For a single layer, skip layer mapping and use the final destination RE buffer.
  span<ci8_t> pdsch_symbols = span<ci8_t>(temp_pdsch_symbols).first(nof_re);

  // Modulate codeword.
  float scaling = modulate(pdsch_symbols, b_hat, mod);

  // Map resource elements.
  map(mapper, pdsch_symbols, scaling, config);
}
