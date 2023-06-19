/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"

namespace srsran {

/// Spy implementation of a CSI-RS processor.
class csi_rs_processor_spy : public nzp_csi_rs_generator
{
  bool map_method_called = false;

public:
  void map(resource_grid_mapper& grid, const config_t& config) override { map_method_called = true; }

  /// Returns true if the map method has been called, false otherwise.
  bool is_map_called() const { return map_method_called; }
};

} // namespace srsran
