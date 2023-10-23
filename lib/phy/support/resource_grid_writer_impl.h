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
#include "srsran/phy/support/resource_grid_writer.h"

namespace srsran {

/// Implements the resource grid writer interface.
class resource_grid_writer_impl : public resource_grid_writer
{
public:
  using storage_type = tensor<static_cast<unsigned>(resource_grid_dimensions::all), cf_t, resource_grid_dimensions>;

  /// Constructs a resource grid writer implementation from a tensor.
  resource_grid_writer_impl(storage_type& data_, std::atomic<unsigned>& empty_) : data(data_), empty(empty_) {}

  // See interface for documentation.
  unsigned get_nof_ports() const override;

  // See interface for documentation.
  unsigned get_nof_subc() const override;

  // See interface for documentation.
  unsigned get_nof_symbols() const override;

  // See interface for documentation.
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override;

  // See interface for documentation.
  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override;

  // See interface for documentation.
  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override;

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  /// Helper function to mark port as not empty.
  inline void clear_empty(unsigned i_port) { empty &= ~(1U << i_port); }

private:
  storage_type&          data;
  std::atomic<unsigned>& empty;
};

} // namespace srsran
