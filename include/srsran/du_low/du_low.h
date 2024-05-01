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

class upper_phy;

/// DU low interface.
class du_low
{
public:
  /// Default destructor.
  virtual ~du_low() = default;

  /// Stops the DU low.
  virtual void stop() = 0;

  /// Returns the upper PHY for the given cell of this DU low.
  virtual upper_phy& get_upper_phy(unsigned cell_id) = 0;
};

} // namespace srsran
