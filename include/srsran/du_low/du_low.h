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

#include "srsran/adt/span.h"

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

  /// Returns a span of the upper PHYs managed by this DU low.
  virtual span<upper_phy*> get_all_upper_phys() = 0;
};

} // namespace srsran
