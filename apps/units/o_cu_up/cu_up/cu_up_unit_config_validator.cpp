/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "cu_up_unit_config_validator.h"
#include "cu_up_unit_config.h"
#include "srsran/adt/span.h"

using namespace srsran;

/// Validates the given QoS configuration. Returns true on success, otherwise false.
static bool validate_qos_appconfig(span<const cu_up_unit_qos_config> config)
{
  for (const auto& qos : config) {
    if (qos.mode != "am" && qos.mode != "um-bidir") {
      fmt::print("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}\n", qos.five_qi, qos.mode);
      return false;
    }
  }
  return true;
}

static bool validate_cu_up_expert_execution_appconfig(const cu_up_unit_execution_config& exec_cfg, bool tracing_enabled)
{
  if (exec_cfg.executor_tracing_enable && !tracing_enabled) {
    fmt::println("Tracing requested for CU-UP executors, but tracing is disabled\n");
    return false;
  }
  return true;
}

bool srsran::validate_cu_up_unit_config(const cu_up_unit_config& config, bool tracing_enabled)
{
  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }
  if (!validate_cu_up_expert_execution_appconfig(config.exec_cfg, tracing_enabled)) {
    return false;
  }

  return true;
}
