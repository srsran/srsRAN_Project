/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_appconfig_validator.h"
#include "apps/services/logger/logger_appconfig_validator.h"
#include "cu_appconfig.h"

using namespace srsran;

bool srsran::validate_cu_appconfig(const cu_appconfig& config)
{
  return validate_logger_appconfig(config.log_cfg);
}