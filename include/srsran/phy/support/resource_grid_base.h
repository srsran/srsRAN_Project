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
