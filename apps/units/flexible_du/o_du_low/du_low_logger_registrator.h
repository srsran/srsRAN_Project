/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
