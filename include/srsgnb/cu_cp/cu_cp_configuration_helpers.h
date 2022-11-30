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

// TODO: This file is temporary. Eventually we will receive cell configurations from the CU config file.

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
  // TODO: do we need to check the notitfiers?
#if 0
  if (config.cu_executor == nullptr) {
    fmt::print("CU executer not set.\n");
    return false;
  }

  if (config.f1c_notifier == nullptr) {
    fmt::print("F1C notifier not set.\n");
    return false;
  }

  if (config.e1_notifier == nullptr) {
    fmt::print("E1 notifier not set.\n");
    return false;
  }

  if (config.ngc_notifier == nullptr) {
    fmt::print("NGC notifier not set.\n");
    return false;
  }
#endif

  if (!is_valid_configuration(config.ngc_config)) {
    fmt::print("Invalid NGC configuration.\n");
    return false;
  }

  return true;
}

} // namespace config_helpers
} // namespace srsgnb
