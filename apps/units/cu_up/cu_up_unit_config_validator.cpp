/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_unit_config_validator.h"
#include "cu_up_unit_config.h"

using namespace srsran;

/// Validates the given AMF configuration. Returns true on success, otherwise false.
static bool validate_amf_appconfig(const cu_up_unit_amf_config& config)
{
  // only check for non-empty AMF address and default port
  if (config.ip_addr.empty() or config.port != 38412) {
    return false;
  }
  return true;
}

bool srsran::validate_cu_up_unit_config(const cu_up_unit_config& config)
{
  if (!validate_amf_appconfig(config.amf_cfg)) {
    return false;
  }

  return true;
}
