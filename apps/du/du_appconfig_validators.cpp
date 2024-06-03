/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_appconfig_validators.h"

using namespace srsran;

bool srsran::validate_appconfig(const du_appconfig& config)
{
  if (config.f1c_cfg.cu_cp_address.empty()) {
    fmt::print("CU-CP F1-C address is mandatory\n");
    return false;
  }

  return true;
}
