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

#include "srsran/phy/upper/upper_phy_factories.h"

namespace srsran {
namespace srs_du {

/// DU low cell configuration.
struct du_low_cell_config {
  /// Upper PHY configuration.
  upper_phy_configuration upper_phy_cfg;
};

/// DU low cell dependencies.
struct du_low_cell_dependencies {
  /// Upper PHY dependencies.
  upper_phy_dependencies upper_phy_deps;
};

/// Parameters used to instantiate the DU-low.
struct du_low_config {
  /// Upper physical layer common configuration for all cells.
  upper_phy_factory_configuration upper_phy_common_config;
  /// Specific cell configuration.
  std::vector<du_low_cell_config> cells;
};

/// Dependencies necessary to instantiate a DU-low.
struct du_low_dependencies {
  /// Logger used to report state of the DU-low.
  srslog::basic_logger* logger;
  /// Upper physical layer common dependencies for all cells.
  upper_phy_factory_dependencies upper_phy_common_deps;
  /// Specific cell dependencies.
  std::vector<du_low_cell_dependencies> cells;
};

} // namespace srs_du
} // namespace srsran
