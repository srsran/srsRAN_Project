/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_du_unit_config_validator.h"
#include "apps/units/flexible_du/du_high/du_high_config_validator.h"

using namespace srsran;

bool srsran::validate_split6_du_unit_config(const split6_du_unit_config&     config,
                                            const os_sched_affinity_bitmask& available_cpus)
{
  if (!validate_du_high_config(config.du_high_cfg.config, available_cpus)) {
    return false;
  }

  return true;
}
