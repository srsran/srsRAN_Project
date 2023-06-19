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

#include "dmrs_pbch_processor_impl.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

unsigned dmrs_pbch_processor_impl::c_init(const config_t& config)
{
  // Default values for L_max == 4
  uint64_t i_ssb = (config.ssb_idx & 0b11U) + 4UL * config.hrf; // Least 2 significant bits

  if (config.L_max == 8 || config.L_max == 64) {
    i_ssb = config.ssb_idx & 0b111U; // Least 3 significant bits
  }

  return ((i_ssb + 1UL) * ((config.phys_cell_id / 4UL) + 1UL) << 11UL) + ((i_ssb + 1UL) << 6UL) +
         (config.phys_cell_id % 4);
}

void dmrs_pbch_processor_impl::generation(std::array<cf_t, NOF_RE>& sequence, const config_t& config) const
{
  // Calculate initial state
  prg->init(c_init(config));

  // Generate sequence
  prg->generate(sequence, M_SQRT1_2);
}

void dmrs_pbch_processor_impl::mapping(const std::array<cf_t, NOF_RE>& r,
                                       resource_grid_writer&           grid,
                                       const config_t&                 args) const
{
  // Calculate index shift
  uint32_t v = args.phys_cell_id % 4;

  // r sequence read index
  uint32_t r_idx = 0;

  // Create resource grid coordinates
  std::array<resource_grid_coordinate, NOF_RE> coordinates;

  const uint8_t  l0 = args.ssb_first_symbol;
  const uint16_t k0 = args.ssb_first_subcarrier;

  // Put sequence in symbol 1 (0 + v , 4 + v , 8 + v ,..., 236 + v)
  for (uint16_t k = v; k < SSB_BW_RE; k += 4) {
    coordinates[r_idx++] = resource_grid_coordinate(l0 + 1, k0 + k);
  }

  // Put sequence in symbol 2, lower section (0 + v , 4 + v , 8 + v ,..., 44 + v)
  for (uint16_t k = v; k < 48; k += 4) {
    coordinates[r_idx++] = resource_grid_coordinate(l0 + 2, k0 + k);
  }

  // Put sequence in symbol 2, upper section
  for (uint32_t k = 192 + v; k < SSB_BW_RE; k += 4) {
    coordinates[r_idx++] = resource_grid_coordinate(l0 + 2, k0 + k);
  }

  // Put sequence in symbol 3
  for (uint32_t k = v; k < SSB_BW_RE; k += 4) {
    coordinates[r_idx++] = resource_grid_coordinate(l0 + 3, k0 + k);
  }

  // For each port...
  for (unsigned port : args.ports) {
    // ... put data in grid using the generated coordinates
    grid.put(port, coordinates, r);
  }
}

void dmrs_pbch_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Generate sequence
  std::array<cf_t, NOF_RE> sequence;
  generation(sequence, config);

  // Mapping to physical resources
  mapping(sequence, grid, config);
}
