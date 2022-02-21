/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pbch_modulator_impl.h"
#include "srsgnb/phy/constants.h"

using namespace srsgnb;

void pbch_modulator_impl::scramble(span<const uint8_t>& b, std::array<uint8_t, M_bit>& b_hat, const args_t& args)
{
  // Initialise sequence
  scrambler->init(args.phys_cell_id);

  // Advance sequence
  scrambler->advance((args.ssb_idx & 0x7) * M_bit);

  // Apply XOR
  scrambler->apply_xor_bit(b, b_hat);
}

void pbch_modulator_impl::modulate(const std::array<uint8_t, M_bit>& b_hat, span<cf_t> d_pbch)
{
  modulator->modulate(b_hat, d_pbch, modulation_scheme::QPSK);
}

void pbch_modulator_impl::map(const std::array<cf_t, M_symb>& d_pbch, resource_grid_writer& grid, const args_t& args)
{
  unsigned count = 0;
  unsigned l0    = args.ssb_first_symbol;
  unsigned k0    = args.ssb_first_subcarrier;

  // Calculate DMRS index shift
  uint32_t v = args.phys_cell_id % 4;

  // Put sequence in symbol 1 (0, 1, ..., 239) not reserved for PBCH DM-RS
  for (uint32_t k = 0; k < SSB_BW_RE; ++k) {
    if (k % 4 != v) {
      grid.put(l0 + 1, k0 + k, d_pbch[count++]);
    }
  }

  // Put sequence in symbol 2, lower section (0 + v , 4 + v , 8 + v ,..., 44 + v) not reserved for PBCH DM-RS
  for (uint32_t k = 0; k < 48; ++k) {
    if (k % 4 != v) {
      grid.put(l0 + 2, k0 + k, d_pbch[count++]);
    }
  }

  // Put sequence in symbol 2, upper section (192 + v , 196 + v ,..., 236 + v) not reserved for PBCH DM-RS
  for (uint32_t k = 192; k < SSB_BW_RE; ++k) {
    if (k % 4 != v) {
      grid.put(l0 + 2, k0 + k, d_pbch[count++]);
    }
  }

  // Put sequence in symbol 3 (0, 1, ..., 239) not reserved for PBCH DM-RS
  for (uint32_t k = 0; k < SSB_BW_RE; ++k) {
    if (k % 4 != v) {
      grid.put(l0 + 3, k0 + k, d_pbch[count++]);
    }
  }
}

void pbch_modulator_impl::put(span<const uint8_t> bits, resource_grid_writer& grid, const pbch_modulator::args_t& args)
{
  assert(bits.size() == M_bit);

  // Scramble
  std::array<uint8_t, M_bit> b_hat;
  scramble(bits, b_hat, args);

  // Modulate
  std::array<cf_t, M_symb> d_pbch;
  modulate(b_hat, d_pbch);

  // Map
  map(d_pbch, grid, args);
}

std::unique_ptr<pbch_modulator> srsgnb::create_pbch_modulator()
{
  return std::make_unique<pbch_modulator_impl>();
}