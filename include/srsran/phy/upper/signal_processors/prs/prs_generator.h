/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

class resource_grid_writer;
struct prs_generator_configuration;

/// Positioning Reference Signals (PRS) generator interface.
class prs_generator
{
public:
  /// Default destructor.
  virtual ~prs_generator() = default;

  /// \brief Generates Positioning Reference Signals (PRS) according to TS38.211 Section 7.4.1.7.
  /// \param[out] grid   Resource grid.
  /// \param[in]  config PRS transmission configuration.
  virtual void generate(resource_grid_writer& grid, const prs_generator_configuration& config) = 0;
};

} // namespace srsran
