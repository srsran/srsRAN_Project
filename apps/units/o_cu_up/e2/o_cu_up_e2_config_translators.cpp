/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "o_cu_up_e2_config_translators.h"
#include "srsran/e2/e2ap_configuration_helpers.h"

using namespace srsran;

e2ap_configuration srsran::generate_e2_config(const e2_config& config)
{
  e2ap_configuration out_cfg = srsran::config_helpers::make_default_e2ap_config();
  out_cfg.e2_type            = e2_type_t::CU_UP;
  out_cfg.e2sm_kpm_enabled   = config.e2sm_kpm_enabled;
  out_cfg.e2sm_rc_enabled    = config.e2sm_rc_enabled;

  return out_cfg;
}
