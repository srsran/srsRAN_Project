/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "dmrs_pdcch_processor_impl.h"
#include "dmrs_helper.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/support/math_utils.h"

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

void dmrs_pdcch_processor_impl::mapping(resource_grid_mapper&   mapper,
                                        const re_buffer_reader& d_pdcch,
                                        const config_t&         config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const re_prb_mask re_mask = {false, true, false, false, false, true, false, false, false, true, false, false};

  // Create PDCCH mapping pattern.
  re_pattern pattern;
  pattern.prb_mask = config.rb_mask;
  pattern.symbols.fill(config.start_symbol_index, config.start_symbol_index + config.duration);
  pattern.re_mask = re_mask;

  // Actual mapping.
  mapper.map(d_pdcch, pattern, config.precoding);
}

void dmrs_pdcch_processor_impl::map(resource_grid_mapper& mapper, const dmrs_pdcch_processor::config_t& config)
{
  srsran_assert(config.precoding.get_nof_layers() == 1,
                "Number of layers (i.e., {}) must be one.",
                config.precoding.get_nof_layers());
  srsran_assert(config.precoding.get_nof_ports() >= 1,
                "Number of ports (i.e., {}) must be equal to or greater than one.",
                config.precoding.get_nof_ports());
  srsran_assert(config.precoding.get_nof_prg() >= 1,
                "Number of PRG (i.e., {}) must be equal to or greater than one.",
                config.precoding.get_nof_prg());

  // Number of DM-RS per symbol.
  unsigned nof_dmrs_symbol = config.rb_mask.count() * NOF_DMRS_PER_RB;

  // Storage of modulated PDCCH data.
  static_re_buffer<1, MAX_NOF_DMRS> d_pdcch(1, config.duration * nof_dmrs_symbol);

  // Generate and map for each symbol of the PDCCH transmission.
  for (unsigned i_symbol     = config.start_symbol_index,
                i_symbol_end = config.start_symbol_index + config.duration,
                i_re         = 0;
       i_symbol != i_symbol_end;
       ++i_symbol, i_re += nof_dmrs_symbol) {
    // Get view for the current symbol.
    span<cf_t> sequence = d_pdcch.get_slice(0).subspan(i_re, nof_dmrs_symbol);

    // Generate sequence.
    sequence_generation(sequence, i_symbol, config);
  }

  // Map sequence.
  mapping(mapper, d_pdcch, config);
}
