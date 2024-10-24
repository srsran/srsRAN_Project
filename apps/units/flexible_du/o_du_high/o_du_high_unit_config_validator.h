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

#include "du_high/du_high_config_validator.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Validates the given O-DU high configuration. Returns true on success, false otherwise.
inline bool validate_o_du_high_config(const o_du_high_unit_config&     config,
                                      const os_sched_affinity_bitmask& available_cpus)
{
  return validate_du_high_config(config.du_high_cfg.config, available_cpus);
}

} // namespace srsran
