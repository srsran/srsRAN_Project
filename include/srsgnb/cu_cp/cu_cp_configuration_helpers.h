/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_configuration.h"
#include "srsgnb/ngap/ngc_configuration_helpers.h"

namespace srsgnb {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::cu_cp_configuration make_default_cu_cp_config()
{
  srs_cu_cp::cu_cp_configuration cfg{};
  cfg.ngc_config = make_default_ngc_config();
  return cfg;
}

/// Returns true if the given CU-CP configuration is valid, otherwise false.
inline bool is_valid_configuration(const srs_cu_cp::cu_cp_configuration& config)
{
  // Notifiers aren't checked here.
  if (!is_valid_configuration(config.ngc_config)) {
    fmt::print("Invalid NGC configuration.\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsgnb
