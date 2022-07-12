/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pdcch_processor_impl.h"
#include "../resource_grid_helpers.h"
#include "dmrs_helper.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

unsigned dmrs_pdcch_processor_impl::c_init(unsigned symbol, const dmrs_pdcch_processor::config_t& config)
{
  unsigned n_slot = config.slot.slot_index();
  unsigned n_id   = config.n_id;

  return ((get_nsymb_per_slot(config.cp) * n_slot + symbol + 1) * (2 * n_id + 1) * pow2(17) + 2 * n_id) % pow2(31);
}

void dmrs_pdcch_processor_impl::sequence_generation(span<cf_t>                            sequence,
                                                    unsigned int                          symbol,
                                                    const dmrs_pdcch_processor::config_t& config) const
{
  // Initialize pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Generate sequence.
  dmrs_sequence_generate(
      sequence, *prg, M_SQRT1_2 * config.amplitude, config.reference_point_k_rb, NOF_DMRS_PER_RB, config.rb_mask);
}

void dmrs_pdcch_processor_impl::mapping(resource_grid_writer&                    grid,
                                        span<const cf_t>                         sequence,
                                        unsigned                                 rg_subc_mask_ref,
                                        span<const bool>                         rg_subc_mask,
                                        unsigned                                 symbol,
                                        const static_vector<uint8_t, MAX_PORTS>& ports)
{
  // Put signal for each port.
  for (unsigned port_idx : ports) {
    grid.put(port_idx, symbol, rg_subc_mask_ref, rg_subc_mask, sequence);
  }
}

void dmrs_pdcch_processor_impl::map(srsgnb::resource_grid_writer&                 grid,
                                    const srsgnb::dmrs_pdcch_processor::config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const std::array<bool, NRE> re_mask = {
      false, true, false, false, false, true, false, false, false, true, false, false};

  // Count number of DMRS.
  unsigned count_dmrs = config.rb_mask.count() * NOF_DMRS_PER_RB;

  // Initial subcarrier index.
  unsigned rg_subc_mask_ref = get_rg_subc_mask_reference(config.rb_mask);

  // Create RG OFDM symbol mask. Identical for all OFDM symbols.
  static_vector<bool, MAX_RB* NRE> rg_subc_mask = get_rg_subc_mask(config.rb_mask, re_mask);

  // Generate and map for each symbol of the PDCCH transmission.
  for (unsigned symbol = 0; symbol != config.start_symbol_index + config.duration; ++symbol) {
    // Temporal allocation of the sequence.
    static_vector<cf_t, MAX_NOF_DMRS_PER_SYMBOL> sequence(count_dmrs);

    // Generate sequence.
    sequence_generation(sequence, symbol, config);

    // Map sequence in symbols.
    mapping(grid, sequence, rg_subc_mask_ref, rg_subc_mask, symbol, config.ports);
  }
}
