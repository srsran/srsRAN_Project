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
