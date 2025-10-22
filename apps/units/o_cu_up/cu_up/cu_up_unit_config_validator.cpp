/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
      fmt::println("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}", qos.five_qi, qos.mode);
      return false;
    }
  }
  return true;
}

static bool validate_cu_up_expert_execution_appconfig(const cu_up_unit_execution_config& exec_cfg, bool tracing_enabled)
{
  if (exec_cfg.executor_tracing_enable && !tracing_enabled) {
    fmt::println("Tracing requested for CU-UP executors, but tracing is disabled");
    return false;
  }
  return true;
}

static bool validate_cu_up_test_mode_appconfig(const cu_up_unit_test_mode_config& config)
{
  if (config.attach_detach_period.count() != 0 && config.reestablish_period.count() != 0) {
    fmt::println("Cannot run attach-detach tests simultaniously with re-establish tests");
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

  if (!validate_cu_up_test_mode_appconfig(config.test_mode_cfg)) {
    return false;
  }

  return true;
}
