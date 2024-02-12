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

#pragma once

#include "../../phy/support/resource_grid_dimensions.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/complex_normal_random.h"

namespace srsran {

/// Implements a resource grid reader that contains always the same random data.
class ru_dummy_rx_resource_grid : public resource_grid_reader
{
public:
  ru_dummy_rx_resource_grid(unsigned seed, unsigned nof_subc, unsigned nof_symbols, unsigned nof_ports) :
    rg_buffer({nof_subc, nof_symbols, nof_ports})
  {
    std::mt19937                      rgen(seed);
    complex_normal_distribution<cf_t> dist(0, 1);

    span<cf_t> data = rg_buffer.get_data();
    std::generate(data.begin(), data.end(), [&rgen, &dist]() { return dist(rgen); });
  }

  // See resource_grid_base for documentation.
  unsigned get_nof_ports() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::port); }

  // See resource_grid_base for documentation.
  unsigned get_nof_subc() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::subc); }

  // See resource_grid_base for documentation.
  unsigned get_nof_symbols() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::symbol); }

  // See resource_grid_reader for documentation.
  bool is_empty(unsigned port) const override
  {
    // This grid is never empty.
    return false;
  }

  // See resource_grid_reader for documentation.
  bool is_empty() const override
  {
    // This grid is never empty.
    return false;
  }

  // See resource_grid_reader for documentation.
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override
  {
    // Count number of elements.
    unsigned nof_symbols = std::accumulate(
        mask.begin(), mask.end(), 0U, [](unsigned count, bool active_re) { return count + (active_re ? 1U : 0U); });

    return get(symbols, nof_symbols);
  }

  // See resource_grid_reader for documentation.
  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override
  {
    // Count number of elements.
    unsigned nof_symbols = mask.count();

    return get(symbols, nof_symbols);
  }

  // See resource_grid_reader for documentation.
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override
  {
    srsvec::copy(symbols, rg_buffer.get_data().first(symbols.size()));
  }

  // See resource_grid_reader for documentation.
  span<const cf_t> get_view(unsigned port, unsigned l) const override { return rg_buffer.get_view({l, port}); }

private:
  /// Gets a number of symbols.
  span<cf_t> get(span<cf_t> symbols, unsigned nof_symbols) const
  {
    srsvec::copy(symbols.first(nof_symbols), rg_buffer.get_data().first(nof_symbols));

    return symbols.last(symbols.size() - nof_symbols);
  }

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<static_cast<unsigned>(resource_grid_dimensions::all), cf_t, resource_grid_dimensions> rg_buffer;
};

} // namespace srsran
