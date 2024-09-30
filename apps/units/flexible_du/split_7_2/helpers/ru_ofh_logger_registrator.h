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

#include "ru_ofh_config.h"

namespace srsran {

/// Registers the Open Fronthaul Radio Unit loggers in the logger service.
inline void register_ru_ofh_loggers(const ru_ofh_unit_logger_config& log_cfg)
{
  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(log_cfg.ofh_level);
}

} // namespace srsran
