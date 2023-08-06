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
#include "resource_grid_dimensions.h"
#include "srsran/phy/support/resource_grid_reader.h"

namespace srsran {

/// Implements the resource grid reader interface.
class resource_grid_reader_impl : public resource_grid_reader
{
public:
  using storage_type = tensor<static_cast<unsigned>(resource_grid_dimensions::all), cf_t, resource_grid_dimensions>;

  /// Constructs a resource grid reader implementation from a tensor.
  resource_grid_reader_impl(const storage_type& data_, span<const bool> empty_) : data(data_), empty(empty_) {}

  // See interface for documentation.
  unsigned get_nof_ports() const override;

  // See interface for documentation.
  unsigned get_nof_subc() const override;

  // See interface for documentation.
  unsigned get_nof_symbols() const override;

  // See interface for documentation.
  bool is_empty(unsigned port) const override;

  // See interface for documentation.
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override;

  // See interface for documentation.
  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override;

  // See interface for documentation.
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override;

  // See interface for documentation.
  span<const cf_t> get_view(unsigned port, unsigned l) const override;

private:
  const storage_type& data;
  span<const bool>    empty;
};

} // namespace srsran
