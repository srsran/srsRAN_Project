/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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
