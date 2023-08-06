/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/ran/s_nssai.h"

namespace srsran {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline e2ap_configuration make_default_e2ap_config()
{
  e2ap_configuration cfg{};
  cfg.gnb_id            = 411;
  cfg.ran_node_name     = "srsgnb01";
  cfg.plmn              = "00101";
  cfg.max_setup_retries = 5;
  cfg.e2sm_kpm_enabled  = false;
  cfg.e2sm_rc_enabled   = false;
  return cfg;
}

/// Returns true if the given E2 configuration is valid, otherwise false.
inline bool is_valid_configuration(const e2ap_configuration& config)
{
  if (config.ran_node_name.empty()) {
    fmt::print("RAN node name is empty\n");
    return false;
  }

  if (config.plmn.empty()) {
    fmt::print("PLMN id is empty\n");
    return false;
  }

  if (config.plmn.find("0x") != std::string::npos) {
    fmt::print("PLMN must not contain 0x\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
