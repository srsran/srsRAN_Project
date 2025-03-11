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
