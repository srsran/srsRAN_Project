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

namespace srsran {

/// \brief Resource grid base interface.
///
/// Provides getters.
class resource_grid_base
{
public:
  /// Default destructor.
  virtual ~resource_grid_base() = default;

  /// Gets the resource grid number of ports.
  virtual unsigned get_nof_ports() const = 0;

  /// Gets the resource grid number of subcarriers.
  virtual unsigned get_nof_subc() const = 0;

  /// Gets the resource grid number of symbols.
  virtual unsigned get_nof_symbols() const = 0;
};

} // namespace srsran
