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

struct srs_estimator_configuration;
struct srs_estimator_result;
class resource_grid_reader;

/// Propagation channel estimator interface based on Sounding Reference Signals (SRS).
class srs_estimator
{
public:
  /// Default destructor.
  virtual ~srs_estimator() = default;

  /// \brief Estimates the SRS propagation channel.
  ///
  /// \param[in] grid   Received resource grid containing the SRS.
  /// \param[in] config Sounding Reference Signal parameters.
  /// \return The propagation channel estimation results.
  virtual srs_estimator_result estimate(const resource_grid_reader&        grid,
                                        const srs_estimator_configuration& config) = 0;
};

} // namespace srsran