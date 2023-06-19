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

#include "ngap_configuration.h"
#include "srsran/ran/s_nssai.h"

namespace srsran {
namespace config_helpers {

std::map<uint8_t, srs_cu_cp::cu_cp_qos_config> make_default_ngap_qos_config_list();

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::ngap_configuration make_default_ngap_config()
{
  srs_cu_cp::ngap_configuration cfg{};
  cfg.gnb_id        = 411;
  cfg.ran_node_name = "srsgnb01";
  cfg.plmn          = "00101";
  cfg.tac           = 7;
  s_nssai_t slice_cfg;
  slice_cfg.sst = 1;
  cfg.slice_configurations.push_back(slice_cfg);

  return cfg;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::ngap_configuration& config)
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
