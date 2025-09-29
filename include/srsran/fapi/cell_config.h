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

#include "messages/config_request_tlvs.h"
#include <any>

namespace srsran {
namespace fapi {

/// FAPI cell configuration.
struct fapi_cell_config {
  phy_config     phy_cfg;
  carrier_config carrier_cfg;
  cell_config    cell_cfg;
  prach_config   prach_cfg;
  ssb_config     ssb_cfg;
  tdd_phy_config tdd_cfg;
  /// Vendor specific configuration..
  std::any vendor_cfg;
};

} // namespace fapi
} // namespace srsran
