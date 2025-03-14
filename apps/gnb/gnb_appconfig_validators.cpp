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

#include "gnb_appconfig_validators.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"
#include "apps/services/worker_manager/worker_manager_appconfig_validator.h"
#include "apps/units/flexible_o_du/o_du_high/du_high/du_high_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"

using namespace srsran;

static bool validate_hal_config(const std::optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
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

  if (!validate_expert_execution_appconfig(config.expert_execution_cfg)) {
    return false;
  }

  if (!validate_hal_config(config.hal_config)) {
    return false;
  }

  return true;
}

bool srsran::validate_plmn_and_tacs(const du_high_unit_config& du_hi_cfg, const cu_cp_unit_config& cu_cp_cfg)
{
  if (cu_cp_cfg.amf_config.no_core) {
    return true;
  }

  std::vector<cu_cp_unit_supported_ta_item> supported_tas;

  for (const auto& supported_ta : cu_cp_cfg.amf_config.amf.supported_tas) {
    supported_tas.push_back(supported_ta);
  }

  for (const auto& amf : cu_cp_cfg.extra_amfs) {
    for (const auto& supported_ta : amf.supported_tas) {
      supported_tas.push_back(supported_ta);
    }
  }

  bool ret_val = false;
  for (const auto& cell : du_hi_cfg.cells_cfg) {
    for (const auto& supported_ta : supported_tas) {
      for (const auto& plmn_item : supported_ta.plmn_list) {
        if (plmn_item.plmn_id == cell.cell.plmn && supported_ta.tac == cell.cell.tac) {
          ret_val = true;
        }
      }
    }

    if (!ret_val) {
      fmt::print("Could not find cell PLMN '{}' and cell TAC '{}' in the CU-CP supported tracking areas list\n",
                 cell.cell.plmn,
                 cell.cell.tac);
      return false;
    }
  }

  return ret_val;
}
