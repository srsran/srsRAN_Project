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

#include "du_appconfig_validators.h"
#include "apps/helpers/f1u/f1u_appconfig_validator.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"

using namespace srsran;

bool srsran::validate_appconfig(const du_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (config.f1ap_cfg.cu_cp_address.empty()) {
    fmt::print("CU-CP F1-C address is mandatory\n");
    return false;
  }

  if (!validate_f1u_sockets_appconfig(config.f1u_cfg.f1u_sockets)) {
    return false;
  }

  return true;
}
