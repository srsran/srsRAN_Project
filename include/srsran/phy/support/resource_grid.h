/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/phy/constants.h"

namespace srsran {

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
};

} // namespace srsran
