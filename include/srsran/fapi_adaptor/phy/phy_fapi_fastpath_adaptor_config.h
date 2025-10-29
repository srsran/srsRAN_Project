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

#include "srsran/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// PHY-FAPI sector fastpath adaptor configuration.
struct phy_fapi_sector_fastpath_adaptor_config {
  phy_fapi_p7_sector_fastpath_adaptor_config p7_config;
};

/// PHY-FAPI sector fastpath adaptor dependencies.
struct phy_fapi_sector_fastpath_adaptor_dependencies {
  phy_fapi_p7_sector_fastpath_adaptor_dependencies p7_dependencies;
};

/// PHY-FAPI fastpath adaptor configuration.
struct phy_fapi_fastpath_adaptor_config {
  std::vector<phy_fapi_sector_fastpath_adaptor_config> sectors;
};

/// PHY-FAPI fastpath adaptor dependencies.
struct phy_fapi_fastpath_adaptor_dependencies {
  std::vector<phy_fapi_sector_fastpath_adaptor_dependencies> sectors;
};

} // namespace fapi_adaptor
} // namespace srsran
