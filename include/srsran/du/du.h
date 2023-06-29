/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

class du
{
public:
  virtual ~du() = default;

  /// \brief Start the DU.
  virtual void start() = 0;

  /// \brief Stop the DU.
  virtual void stop() = 0;
};

} // namespace srsran
