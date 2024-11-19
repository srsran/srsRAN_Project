/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e2/e2ap_configuration.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline e2ap_configuration make_default_e2ap_config()
{
  e2ap_configuration cfg{};
  cfg.gnb_id            = {411, 22};
  cfg.plmn              = "00101";
  cfg.max_setup_retries = 5;
  cfg.e2sm_kpm_enabled  = false;
  cfg.e2sm_rc_enabled   = false;
  return cfg;
}

/// Returns true if the given E2 configuration is valid, otherwise false.
inline bool is_valid_configuration(const e2ap_configuration& config)
{
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
