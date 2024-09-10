/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {
namespace srs_du {

/// Public DU interface.
class du
{
public:
  virtual ~du() = default;

  /// Starts the DU.
  virtual void start() = 0;

  /// Stops the DU.
  virtual void stop() = 0;
};

} // namespace srs_du
} // namespace srsran
