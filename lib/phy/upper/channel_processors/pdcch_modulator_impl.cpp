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
#include "../resource_grid_helpers.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

void pdcch_modulator_impl::scramble(span<uint8_t> b_hat, span<const uint8_t> b, const pdcch_modulator::config_t& config)
{
  // Calculate initial sequence state.
  unsigned c_init = ((config.n_rnti << 16U) + config.n_id) % (1U << 31U);

  // Initialize the scrambler with the initial state.
  scrambler->init(c_init);

  // Apply sequence as unpacked XOR.
  scrambler->apply_xor_bit(b_hat, b);
}

void pdcch_modulator_impl::modulate(span<cf_t> d_pdcch, span<const uint8_t> b_hat, float scaling)
{
  // Modulate as QPSK.
  modulator->modulate(b_hat, d_pdcch, modulation_scheme::QPSK);

  // Apply scaling to conform power.
  if (std::isnormal(scaling)) {
    srsvec::sc_prod(d_pdcch, scaling, d_pdcch);
  }
}

void pdcch_modulator_impl::map(resource_grid_writer& grid, span<const cf_t> d_pdcch, const config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const std::array<bool, NRE> re_mask = {
      true, false, true, true, true, false, true, true, true, false, true, true};

  // Initial subcarrier index.
  unsigned rg_subc_mask_ref = get_rg_subc_mask_reference(config.rb_mask);

  // Create RG OFDM symbol mask. Identical for all OFDM symbols.
  static_vector<bool, MAX_RB* NRE> rg_subc_mask = get_rg_subc_mask(config.rb_mask, re_mask);

  // Repeat the same process for all ports.
  for (uint8_t port_idx : config.ports) {
    // As the resource element buffer advances for every symbol, it needs to be renewed for every port.
    span<const cf_t> d_buffer = d_pdcch;

    // For each symbol in the CORESET map the resource elements.
    for (unsigned symbol_idx       = config.start_symbol_index,
                  end_symbol_index = config.start_symbol_index + config.duration;
         symbol_idx != end_symbol_index;
         ++symbol_idx) {
      d_buffer = grid.put(port_idx, symbol_idx, rg_subc_mask_ref, rg_subc_mask, d_buffer);
    }
  }
}

void pdcch_modulator_impl::modulate(srsgnb::resource_grid_writer&            grid,
                                    srsgnb::span<const uint8_t>              data,
                                    const srsgnb::pdcch_modulator::config_t& config)
{
  // Apply scrambling.
  static_vector<uint8_t, MAX_BITS> b_hat(data.size());
  scramble(b_hat, data, config);

  // Apply modulation mapping.
  static_vector<cf_t, MAX_RE> d_pdcch(data.size() / 2);
  modulate(d_pdcch, b_hat, config.scaling);

  // Map to resource elements.
  map(grid, d_pdcch, config);
}
