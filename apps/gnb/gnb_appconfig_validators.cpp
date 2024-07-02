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

#include "gnb_appconfig_validators.h"
#include "apps/services/logger/logger_appconfig_validator.h"
#include "apps/units/cu_cp/cu_cp_unit_config.h"
#include "apps/units/flexible_du/du_high/du_high_config.h"

using namespace srsran;

static bool validate_hal_config(const std::optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
    return false;
  }
#else
  if (config) {
    fmt::print("Unable to use DPDK as the application was not compiled with DPDK support\n");
    return false;
  }
#endif
  return true;
}

bool srsran::validate_appconfig(const gnb_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_hal_config(config.hal_config)) {
    return false;
  }

  return true;
}

bool srsran::validate_plmn_and_tacs(const du_high_unit_config& du_hi_cfg, const cu_cp_unit_config& cu_cp_cfg)
{
  for (const auto& cell : du_hi_cfg.cells_cfg) {
    if (std::find(cu_cp_cfg.plmns.cbegin(), cu_cp_cfg.plmns.cend(), cell.cell.plmn) == cu_cp_cfg.plmns.cend()) {
      fmt::print("Could not find cell PLMN '{}' in the CU-CP PLMN list", cell.cell.plmn);

      return false;
    }

    if (std::find(cu_cp_cfg.tacs.cbegin(), cu_cp_cfg.tacs.cend(), cell.cell.tac) == cu_cp_cfg.tacs.cend()) {
      fmt::print("Could not find cell TAC '{}' in the CU-CP TAC list", cell.cell.tac);
      return false;
    }
  }

  return true;
}
