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

#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid.h"

namespace srsran {

/// Describes a generic resource grid implementation
class resource_grid_impl : public resource_grid
{
private:
  /// Dimension, i.e. number of coordinates, of each indexing level of the resource grid.
  enum dimensions : unsigned { dim_symbol = 1, dim_port = 2, dim_all = 3 };

  std::vector<bool> empty;
  unsigned          nof_ports;
  unsigned          nof_symb;
  unsigned          nof_subc;

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<dim_all, cf_t> rg_buffer;

public:
  resource_grid_impl(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc);
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override;
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override;
  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override;
  void             put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  bool       is_empty(unsigned port) const override;
  void       get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const override;
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override;
  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override;
  void       get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override;

  void set_all_zero() override;
};

} // namespace srsran
