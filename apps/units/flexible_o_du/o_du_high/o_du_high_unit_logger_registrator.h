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

#include "du_high/du_high_logger_registrator.h"
#include "fapi/fapi_logger_registrator.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Registers the O-RAN DU high loggers in the logger service.
inline void register_o_du_high_loggers(const o_du_high_unit_config& unit_config)
{
  register_du_high_loggers(unit_config.du_high_cfg.config.loggers);
  register_fapi_loggers(unit_config.fapi_cfg);
}

} // namespace srsran
