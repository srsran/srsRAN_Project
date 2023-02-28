/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap_configuration.h"

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

  return cfg;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::ngap_configuration& config)
{
  if (config.ran_node_name.empty()) {
    fmt::print("RAN node name is empty");
    return false;
  }

  if (config.plmn.empty()) {
    fmt::print("PLMN id is empty");
    return false;
  }

  if (config.plmn.find("0x") != std::string::npos) {
    fmt::print("PLMN must not contain 0x");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsran
