/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_appconfig_validators.h"
#include "apps/helpers/logger/logger_appconfig_validator.h"
#include "du_low_appconfig.h"

using namespace srsran;

bool srsran::validate_du_low_appconfig(const du_low_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  return true;
}
