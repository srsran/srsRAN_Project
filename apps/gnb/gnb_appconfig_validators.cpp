/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gnb_appconfig_validators.h"
#include "srsran/adt/span.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/nr_cgi_helpers.h"
#include "srsran/ran/prach/prach_helper.h"
#include "srsran/rlc/rlc_config.h"

using namespace srsran;

/// Validates the given logging configuration. Returns true on success, otherwise false.
static bool validate_log_appconfig(const log_appconfig& config)
{
  if (config.filename.empty()) {
    return false;
  }

  return true;
}

static bool validate_hal_config(const std::optional<hal_appconfig>& config)
{
#ifdef DPDK_FOUND
  if (config && config->eal_args.empty()) {
    fmt::print("It is mandatory to fill the EAL configuration arguments to initialize DPDK correctly\n");
    return false;
  }
#else
  if (config) {
    fmt::print("Unable to use DPDK as the application was not compiled with DPDK support\n");
    return false;
  }
#endif
  return true;
}

bool srsran::validate_appconfig(const gnb_appconfig& config)
{
  if (!validate_log_appconfig(config.log_cfg)) {
    return false;
  }

  if (!validate_hal_config(config.hal_config)) {
    return false;
  }

  return true;
}
