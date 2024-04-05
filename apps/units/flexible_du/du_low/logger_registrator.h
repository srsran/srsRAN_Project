/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../../gnb/gnb_appconfig.h"

namespace srsran {
namespace modules {
namespace flexible_du {
namespace du_low {

/// Registers the DU high loggers in the logger service.
inline void register_logs(const log_appconfig& log_cfg)
{
  // Set layer-specific logging options.
  auto& phy_logger = srslog::fetch_basic_logger("PHY", true);
  phy_logger.set_level(srslog::str_to_basic_level(log_cfg.phy_level));
  phy_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace du_low
} // namespace flexible_du
} // namespace modules
} // namespace srsran
