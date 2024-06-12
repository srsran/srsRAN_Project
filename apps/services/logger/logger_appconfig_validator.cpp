/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logger_appconfig_validator.h"
#include "logger_appconfig.h"

using namespace srsran;

bool srsran::validate_logger_appconfig(const logger_appconfig& config)
{
  return !config.filename.empty();
}
