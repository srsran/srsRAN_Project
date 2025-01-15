/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split6_o_du_unit_config_validator.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_validator.h"

using namespace srsran;

bool srsran::validate_split6_o_du_unit_config(const split6_o_du_unit_config&   config,
                                              const os_sched_affinity_bitmask& available_cpus)
{
  if (!validate_o_du_high_config(config.odu_high_cfg, available_cpus)) {
    return false;
  }

  return true;
}
