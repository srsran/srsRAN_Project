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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/constants.h"

namespace srsran {

class resource_grid_mapper;
class resource_grid_reader;
class resource_grid_writer;

/// Describes a resource grid class with writer and reader interfaces.
class resource_grid
{
public:
  virtual ~resource_grid() = default;

  /// Sets all resource elements in the grid to zero.
  virtual void set_all_zero() = 0;

  /// Gets the resource grid writer.
  virtual resource_grid_writer& get_writer() = 0;

  /// Gets the resource grid reader.
  virtual const resource_grid_reader& get_reader() const = 0;

  /// Gets the resource grid mapper.
  virtual resource_grid_mapper& get_mapper() = 0;
};

} // namespace srsran
