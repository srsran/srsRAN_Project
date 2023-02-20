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
#include "dmrs_helper.h"
#include "srsgnb/phy/support/mask_types.h"
#include "srsgnb/support/math_utils.h"

using namespace srsran;

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
                                        const bounded_bitset<MAX_RB * NRE>&      rg_subc_mask,
                                        unsigned                                 symbol,
                                        const static_vector<uint8_t, MAX_PORTS>& ports)
{
  // Put signal for each port.
  for (unsigned port_idx : ports) {
    grid.put(port_idx, symbol, 0, rg_subc_mask, sequence);
  }
}

void dmrs_pdcch_processor_impl::map(srsran::resource_grid_writer&                 grid,
                                    const srsran::dmrs_pdcch_processor::config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const re_prb_mask re_mask = {false, true, false, false, false, true, false, false, false, true, false, false};

  // Count number of DMRS.
  unsigned count_dmrs = config.rb_mask.count() * NOF_DMRS_PER_RB;

  // Create RG OFDM symbol mask. Identical for all OFDM symbols.
  bounded_bitset<MAX_RB* NRE> rg_subc_mask = config.rb_mask.kronecker_product<NRE>(re_mask);

  // Generate and map for each symbol of the PDCCH transmission.
  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.duration;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Temporal allocation of the sequence.
    span<cf_t> sequence = span<cf_t>(temp_sequence).first(count_dmrs);

    // Generate sequence.
    sequence_generation(sequence, i_symbol, config);

    // Map sequence in symbols.
    mapping(grid, sequence, rg_subc_mask, i_symbol, config.ports);
  }
}
