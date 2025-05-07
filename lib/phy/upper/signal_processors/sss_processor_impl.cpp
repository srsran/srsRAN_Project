/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
