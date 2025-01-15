/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_appconfig_validator.h"
#include "apps/services/logger/logger_appconfig_validator.h"
#include "apps/services/worker_manager/worker_manager_appconfig_validator.h"
#include "cu_appconfig.h"

using namespace srsran;

bool srsran::validate_cu_appconfig(const cu_appconfig& config)
{
  if (!validate_logger_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_expert_execution_appconfig(config.expert_execution_cfg)) {
    return false;
  }

  return true;
}
