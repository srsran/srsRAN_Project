/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srs_estimator_generic_impl.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include "srsran/ran/srs/srs_information.h"
#include "srsran/srsvec/dot_prod.h"

using namespace srsran;

void srs_estimator_generic_impl::extract_sequence(span<cf_t>                  sequence,
                                                  const resource_grid_reader& grid,
                                                  unsigned                    i_rx_port,
                                                  unsigned                    i_symbol,
                                                  unsigned                    initial_subcarrier,
                                                  unsigned                    comb_size)
{
  bounded_bitset<NRE * MAX_RB> mask(sequence.size() * comb_size);

  for (unsigned i = 0, i_end = sequence.size(); i != i_end; ++i) {
    mask.set(comb_size * i);
  }

  grid.get(sequence, i_rx_port, i_symbol, initial_subcarrier, mask);
}

srs_estimator_result srs_estimator_generic_impl::estimate(const resource_grid_reader&        grid,
                                                          const srs_estimator_configuration& config)
{
  unsigned nof_symbols          = static_cast<unsigned>(config.resource.nof_symbols);
  unsigned nof_symbols_per_slot = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  srsran_assert(config.resource.start_symbol.value() + nof_symbols <= nof_symbols_per_slot,
                "The start symbol index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the number of symbols "
                "per slot (i.e., {})",
                config.resource.start_symbol,
                nof_symbols,
                nof_symbols_per_slot);

  srs_estimator_result result;
  result.channel_matrix =
      srs_channel_matrix(config.ports.size(), static_cast<unsigned>(config.resource.nof_antenna_ports));

  for (uint8_t i_rx_port : config.ports) {
    for (unsigned i_antenna_port = 0, i_antenna_port_end = static_cast<unsigned>(config.resource.nof_antenna_ports);
         i_antenna_port != i_antenna_port_end;
         ++i_antenna_port) {
      cf_t coefficient = 0;
      for (unsigned i_symbol     = config.resource.start_symbol.value(),
                    i_symbol_end = config.resource.start_symbol.value() + nof_symbols;
           i_symbol != i_symbol_end;
           ++i_symbol) {
        // Generate SRS information for a given SRS antenna port and symbol.
        srs_information info = get_srs_information(config.resource, i_antenna_port, i_symbol);

        // Generate sequence.
        static_vector<cf_t, max_seq_length> sequence(info.sequence_length);
        deps.sequence_generator->generate(sequence, info.sequence_group, info.sequence_number, info.alpha);

        static_vector<cf_t, max_seq_length> rx_sequence(info.sequence_length);
        extract_sequence(rx_sequence, grid, i_rx_port, i_symbol, info.mapping_initial_subcarrier, info.comb_size);

        coefficient += srsvec::dot_prod(rx_sequence, sequence) / static_cast<float>(sequence.size());
      }

      coefficient /= static_cast<float>(nof_symbols);

      result.channel_matrix.set_coefficient(coefficient, i_rx_port, i_antenna_port);
    }
  }

  return result;
}
