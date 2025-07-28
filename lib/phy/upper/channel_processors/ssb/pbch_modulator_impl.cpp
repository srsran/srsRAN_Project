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

#include "pbch_modulator_impl.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;

void pbch_modulator_impl::scramble(span<const uint8_t> b, span<uint8_t> b_hat, const config_t& config)
{
  // Initialize sequence.
  scrambler->init(config.phys_cell_id);

  // Advance sequence.
  scrambler->advance((config.ssb_idx & 0x7) * M_bit);

  // Apply XOR.
  scrambler->apply_xor(b_hat, b);
}

void pbch_modulator_impl::modulate(span<const uint8_t> b_hat, span<cf_t> d_pbch)
{
  // Adapt the bits for the modulator.
  static_bit_buffer<M_bit> b_hat_packed(b_hat.size());
  srsvec::bit_pack(b_hat_packed, b_hat);

  // Modulate as QPSK.
  modulator->modulate(d_pbch, b_hat_packed, modulation_scheme::QPSK);
}

void pbch_modulator_impl::map(span<const cf_t> d_pbch, resource_grid_writer& grid, const config_t& config)
{
  unsigned l0 = config.ssb_first_symbol;
  unsigned k0 = config.ssb_first_subcarrier;

  // Calculate DM-RS index shift.
  uint32_t v = config.phys_cell_id % 4;

  // Resource element mask within each resource block. Remove DM-RS from the mask.
  bounded_bitset<NRE> re_mask = ~bounded_bitset<NRE>(NRE);
  re_mask.reset(v);
  re_mask.reset(v + 4);
  re_mask.reset(v + 8);

  // Full SS/PBCH block bandwidth resource blocks.
  bounded_bitset<MAX_RB>       rb_full_mask = ~bounded_bitset<MAX_RB>(SSB_BW_RB);
  bounded_bitset<NRE * MAX_RB> full_mask    = rb_full_mask.kronecker_product<NRE>(re_mask);

  // Edges SS/PBCH block bandwidth resource blocks.
  bounded_bitset<MAX_RB> rb_edge_mask(SSB_BW_RB);
  rb_edge_mask.fill(0, 4);
  rb_edge_mask.fill(16, 20);
  bounded_bitset<NRE * MAX_RB> edge_mask = rb_edge_mask.kronecker_product<NRE>(re_mask);

  // Map symbols for each of the ports.
  for (unsigned port : config.ports) {
    span<const cf_t> symbols = d_pbch;

    // Put sequence in symbol 1 (0, 1, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 1, k0, full_mask, symbols);

    // Put sequence in symbol 2 (0, 1, ..., 41) and (192, 193, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 2, k0, edge_mask, symbols);

    // Put sequence in symbol 3 (0, 1, ..., 239), skip the subcarriers reserved for PBCH DM-RS.
    symbols = grid.put(port, l0 + 3, k0, full_mask, symbols);

    srsran_assert(symbols.empty(), "Symbols must be empty.");
  }
}

void pbch_modulator_impl::put(span<const uint8_t>             bits,
                              resource_grid_writer&           grid,
                              const pbch_modulator::config_t& args)
{
  srsran_assert(bits.size() == M_bit, "Input span size must equal M_bit");

  // Scramble.
  std::array<uint8_t, M_bit> b_hat;
  scramble(bits, b_hat, args);

  // Modulate.
  std::array<cf_t, M_symb> d_pbch;
  modulate(b_hat, d_pbch);

  // Map.
  map(d_pbch, grid, args);
}
