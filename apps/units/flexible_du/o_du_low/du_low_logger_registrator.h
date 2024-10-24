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

#include "du_low_config.h"

namespace srsran {

/// Registers the DU low loggers in the logger service.
inline void register_du_low_loggers(const du_low_unit_logger_config& log_cfg)
{
  // Set layer-specific logging options.
  auto& phy_logger = srslog::fetch_basic_logger("PHY", true);
  phy_logger.set_level(log_cfg.phy_level);
  phy_logger.set_hex_dump_max_size(log_cfg.hex_max_size);
}

} // namespace srsran
