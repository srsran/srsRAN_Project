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

#include "srsran/du/du_low/du_low_config.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_config.h"

namespace srsran {
namespace srs_du {

using cell_prach_ports_entry = std::vector<uint8_t>;

/// O-RAN DU low configuration.
struct o_du_low_config {
  du_low_config du_low_cfg;
  /// FAPI adaptor configuration.
  fapi_adaptor::phy_fapi_adaptor_config fapi_cfg;
  /// Metrics configuration. Set to \c true to enable the DU low metrics.
  bool enable_metrics;
};

/// O-RAN DU low dependencies.
struct o_du_low_dependencies {
  /// DU Low dependencies.
  du_low_dependencies du_low_deps;
};

} // namespace srs_du
} // namespace srsran
