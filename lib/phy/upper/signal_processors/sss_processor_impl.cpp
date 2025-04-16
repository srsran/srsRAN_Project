/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sss_processor_impl.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

const sss_sequence_generator sss_processor_impl::sequence_generator;

void sss_processor_impl::mapping(span<const cf_t> sequence, resource_grid_writer& grid, const config_t& args) const
{
  // Calculate symbol and first subcarrier for SSS.
  unsigned l = args.ssb_first_symbol + ssb_l;
  unsigned k = args.ssb_first_subcarrier + ssb_k_begin;

  // Write in grid for each port.
  for (unsigned port : args.ports) {
    grid.put(port, l, k, sequence);
  }
}

void sss_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence.
  std::array<cf_t, sequence_len> sequence;
  sequence_generator.generate(sequence, config.phys_cell_id, config.amplitude);

  // Mapping to physical resources.
  mapping(sequence, grid, config);
}
