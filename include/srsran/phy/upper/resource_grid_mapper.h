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

#pragma once

#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/upper/re_buffer.h"
#include "srsran/phy/upper/re_pattern.h"
#include "srsran/ran/precoding/precoding_configuration.h"

namespace srsran {

/// Resource grid mapper - Precodes and writes resource elements into the resource grid.
class resource_grid_mapper
{
public:
  /// Constructs a resource grid mapper for a given grid.
  resource_grid_mapper(resource_grid_writer& grid_) : grid(grid_) {}

  /// Default destructor.
  virtual ~resource_grid_mapper() = default;

  /// \brief Applies the resource grid precoding to the input.
  /// \param[out] grid      Resource grid destination.
  /// \param[in] input      Input data.
  /// \param[in] pattern    Data allocation pattern in the resource grid.
  /// \param[in] precoding  Precoding configuration.
  void map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding)
  {
    srsran_assert(input.get_nof_slices() == precoding.get_nof_layers(),
                  "The input number of layers (i.e., {}) and the precoding number of layers (i.e., {}) are different.",
                  input.get_nof_slices(),
                  precoding.get_nof_layers());

    unsigned nof_layers = precoding.get_nof_layers();
    unsigned nof_ports  = precoding.get_nof_ports();
    srsran_assert(nof_layers == 1, "Only one layer is currently supported.");
    srsran_assert(nof_ports == 1, "Only one port is currently supported.");

    for (unsigned i_symbol = 0, i_re = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
      // Get the symbol RE mask.
      bounded_bitset<MAX_RB * NRE> symbol_re_mask(MAX_RB * NRE);
      pattern.get_inclusion_mask(symbol_re_mask, i_symbol);

      // Find the highest used subcarrier. Skip symbol if no active subcarrier.
      int i_highest_subc = symbol_re_mask.find_highest();
      if (i_highest_subc < 0) {
        continue;
      }

      // Resize the mask to the highest subcarrier, ceiling to PRB.
      symbol_re_mask.resize(divide_ceil(i_highest_subc, NOF_SUBCARRIERS_PER_RB) * NOF_SUBCARRIERS_PER_RB);

      // Count the number of mapped RE.
      unsigned nof_re = symbol_re_mask.count();

      // Map each layer to a port.
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          span<const cf_t> layer_data = input.get_slice(i_layer);
          grid.put(i_layer, i_symbol, 0, symbol_re_mask, layer_data.subspan(i_re, nof_re));
        }
      }

      // Advance RE counter.
      i_re += nof_re;
    }
  }

private:
  /// Resource grid to map.
  resource_grid_writer& grid;
};

} // namespace srsran
