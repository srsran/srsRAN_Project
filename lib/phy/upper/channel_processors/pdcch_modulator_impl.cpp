/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_modulator_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

void pdcch_modulator_impl::scramble(span<uint8_t> b_hat, span<const uint8_t> b, const pdcch_modulator::config_t& config)
{
  // Calculate initial sequence state.
  unsigned c_init = ((config.n_rnti << 16U) + config.n_id) % (1U << 31U);

  // Initialize the scrambler with the initial state.
  scrambler->init(c_init);

  // Apply sequence as unpacked XOR.
  scrambler->apply_xor(b_hat, b);
}

void pdcch_modulator_impl::modulate(span<cf_t> d_pdcch, span<const uint8_t> b_hat, float scaling)
{
  // Adapt the bits for the modulator.
  static_bit_buffer<MAX_BITS> b_hat_packed(b_hat.size());
  srsvec::bit_pack(b_hat_packed, b_hat);

  // Modulate as QPSK.
  modulator->modulate(d_pdcch, b_hat_packed, modulation_scheme::QPSK);

  // Apply scaling to conform power.
  if (std::isnormal(scaling)) {
    srsvec::sc_prod(d_pdcch, scaling, d_pdcch);
  }
}

void pdcch_modulator_impl::map(resource_grid_writer& grid, span<const cf_t> d_pdcch, const config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const bounded_bitset<NRE> re_mask = {
      true, false, true, true, true, false, true, true, true, false, true, true};

  // Create RG OFDM symbol mask. Identical for all OFDM symbols.
  bounded_bitset<MAX_RB* NRE> rg_subc_mask = config.rb_mask.kronecker_product<NRE>(re_mask);

  // Repeat the same process for all ports.
  for (uint8_t port_idx : config.ports) {
    // As the resource element buffer advances for every symbol, it needs to be renewed for every port.
    span<const cf_t> d_buffer = d_pdcch;

    // For each symbol in the CORESET map the resource elements.
    for (unsigned symbol_idx       = config.start_symbol_index,
                  end_symbol_index = config.start_symbol_index + config.duration;
         symbol_idx != end_symbol_index;
         ++symbol_idx) {
      d_buffer = grid.put(port_idx, symbol_idx, 0, rg_subc_mask, d_buffer);
    }
  }
}

void pdcch_modulator_impl::modulate(srsgnb::resource_grid_writer&            grid,
                                    srsgnb::span<const uint8_t>              data,
                                    const srsgnb::pdcch_modulator::config_t& config)
{
  // Apply scrambling.
  span<uint8_t> b_hat = span<uint8_t>(temp_b_hat).first(data.size());
  scramble(b_hat, data, config);

  // Apply modulation mapping.
  span<cf_t> d_pdcch = span<cf_t>(temp_d_pdcch).first(data.size() / 2);
  modulate(d_pdcch, b_hat, config.scaling);

  // Map to resource elements.
  map(grid, d_pdcch, config);
}
