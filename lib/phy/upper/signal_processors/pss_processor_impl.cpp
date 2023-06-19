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

#include "pss_processor_impl.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

pss_processor_impl::pregen_signal_s::pregen_signal_s()
{
  // Initialize M sequence x.
  std::array<uint32_t, SEQUENCE_LEN + 7> x;
  x[6] = 1;
  x[5] = 1;
  x[4] = 1;
  x[3] = 0;
  x[2] = 1;
  x[1] = 1;
  x[0] = 0;

  // Generate M sequence x
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    x[i + 7] = (x[i + 4] + x[i]) % 2;
  }

  // Modulate M sequence d
  for (uint32_t i = 0; i != SEQUENCE_LEN; ++i) {
    (*this)[i] = 1.0F - 2.0F * (float)x[i];
  }
}

const pss_processor_impl::pregen_signal_s pss_processor_impl::signal = pss_processor_impl::pregen_signal_s();

void srsran::pss_processor_impl::generation(std::array<cf_t, SEQUENCE_LEN>& sequence, const config_t& config) const
{
  // Calculate generation parameters
  unsigned m = M(phys_cell_id::NID_2(config.phys_cell_id));

  // Temporal sequence
  span<cf_t>       tmp    = sequence;
  span<const cf_t> pregen = signal;

  // Copy sequence from offset to the end
  srsvec::sc_prod(pregen.last(SEQUENCE_LEN - m), config.amplitude, tmp.first(SEQUENCE_LEN - m));

  // Copy sequence from 0 to offset
  srsvec::sc_prod(pregen.first(m), config.amplitude, tmp.last(m));
}

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
  generation(sequence, config);

  // Mapping to physical resources
  mapping(sequence, grid, config);
}
