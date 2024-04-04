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

#include "../du_high/logger_registrator.h"
#include "../du_low//logger_registrator.h"
#include "../fapi//logger_registrator.h"

namespace srsran {
namespace modules {
namespace flexible_du {
namespace split_7_2 {

/// Registers the DU split 7.2 loggers in the logger service.
inline void register_logs(const log_appconfig& log_cfg)
{
  du_high::register_logs(log_cfg);
  du_low::register_logs(log_cfg);
  fapi::register_logs(log_cfg);

  srslog::basic_logger& ofh_logger = srslog::fetch_basic_logger("OFH", false);
  ofh_logger.set_level(srslog::str_to_basic_level(log_cfg.ofh_level));
}

} // namespace split_7_2
} // namespace flexible_du
} // namespace modules
} // namespace srsran
