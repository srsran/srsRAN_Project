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
