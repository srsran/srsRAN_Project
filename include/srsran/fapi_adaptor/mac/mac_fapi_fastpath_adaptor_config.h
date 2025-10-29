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

#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor_config.h"

namespace srsran {
namespace fapi_adaptor {

/// MAC-FAPI sector fastpath adaptor configuration.
struct mac_fapi_sector_fastpath_adaptor_config {
  mac_fapi_p7_sector_fastpath_adaptor_config p7_config;
};

/// MAC-FAPI sector fastpath adaptor dependencies.
struct mac_fapi_sector_fastpath_adaptor_dependencies {
  mac_fapi_p7_sector_fastpath_adaptor_dependencies p7_dependencies;
};

/// MAC-FAPI fastpath adaptor configuration.
struct mac_fapi_fastpath_adaptor_config {
  std::vector<mac_fapi_sector_fastpath_adaptor_config> sectors;
};

/// MAC-FAPI fastpath adaptor dependencies.
struct mac_fapi_fastpath_adaptor_dependencies {
  std::vector<mac_fapi_sector_fastpath_adaptor_dependencies> sectors;
};

} // namespace fapi_adaptor
} // namespace srsran
