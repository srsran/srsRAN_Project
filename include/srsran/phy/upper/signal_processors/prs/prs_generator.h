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
