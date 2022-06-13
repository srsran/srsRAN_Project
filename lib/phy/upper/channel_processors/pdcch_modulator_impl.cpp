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
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

void pdcch_modulator_impl::scramble(span<uint8_t> b_hat, span<const uint8_t> b, const pdcch_modulator::config_t& config)
{
  // Calculate initial sequence state.
  unsigned c_init = ((config.n_rnti << 16U) + config.n_id) % (1 << 31);

  // Initialize the scrambler with the initial state.
  scrambler->init(c_init);

  // Apply sequence as unpacked XOR.
  scrambler->apply_xor_bit(b, b_hat);
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

  // Generate empty allocation mask for the all symbols.
  std::array<bool, MAX_RB* NRE> mask = {};

  unsigned prb_begin_index;
  {
    int ret = config.rb_mask.find_lowest();
    srsran_assert(ret != -1, "No RB allocated for the transmission rb_mask={}", config.rb_mask);
    prb_begin_index = static_cast<unsigned>(ret);
  }

  unsigned prb_begin_end;
  {
    int ret = config.rb_mask.find_highest();
    srsran_assert(ret != -1, "No RB allocated for the transmission rb_mask={}", config.rb_mask);
    prb_begin_end = static_cast<unsigned>(ret + 1);
  }

  // Generate mapping mask for each .
  for (unsigned prb_index = prb_begin_index; prb_index != prb_begin_end; ++prb_index) {
    // Select RE mask for the given RB index.
    span<bool> re_mask_rb = span<bool>(mask).subspan(prb_index * NRE, NRE);

    // If the resource block is not used, set mask to false and skip.
    if (!config.rb_mask.test(prb_index)) {
      continue;
    }

    // Otherwise, copy RE mask into the RB.
    std::copy(re_mask.begin(), re_mask.end(), re_mask_rb.begin());
  }

  // Repeat the same process for all ports.
  for (uint8_t port_idx : config.ports) {
    // As the resource element buffer advances for every symbol, it needs to be renewed for every port.
    span<const cf_t> d_buffer = d_pdcch;

    // For each symbol in the CORESET map the resource elements.
    for (unsigned symbol_idx       = config.start_symbol_index,
                  end_symbol_index = config.start_symbol_index + config.duration;
         symbol_idx != end_symbol_index;
         ++symbol_idx) {
      d_buffer = grid.put(port_idx, symbol_idx, 0, mask, d_buffer);
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
