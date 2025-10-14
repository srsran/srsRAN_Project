/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pss_processor_impl.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

const pss_sequence_generator pss_processor_impl::sequence_generator;

void srsran::pss_processor_impl::mapping(const std::array<cf_t, SEQUENCE_LEN>& sequence,
                                         resource_grid_writer&                 grid,
                                         const config_t&                       args) const
{
  // Calculate symbol and first subcarrier for PSS
  unsigned l = args.ssb_first_symbol + SSB_L;
  unsigned k = args.ssb_first_subcarrier + SSB_K_BEGIN;

  // For each port
  for (unsigned port : args.ports) {
    // Write in grid
    grid.put(port, l, k, sequence);
  }
}

void srsran::pss_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence
  std::array<cf_t, SEQUENCE_LEN> sequence;
  sequence_generator.generate(sequence, config.phys_cell_id, config.amplitude);

  // Mapping to physical resources
  mapping(sequence, grid, config);
}
