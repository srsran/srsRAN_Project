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

#include "apps/units/flexible_du/du_high/du_high_logger_registrator.h"
#include "apps/units/flexible_du/du_low//du_low_wrapper_config_helper.h.h"
#include "apps/units/flexible_du/fapi/fapi_logger_registrator.h"

namespace srsran {
namespace modules {
namespace flexible_du {
namespace split_6 {

/// Registers the DU split 6 loggers in the logger service.
inline void register_logs(const log_appconfig& log_cfg)
{
  du_high::register_logs(log_cfg);
  fapi::register_logs(log_cfg);
}

} // namespace split_6
} // namespace flexible_du
} // namespace modules
} // namespace srsran
