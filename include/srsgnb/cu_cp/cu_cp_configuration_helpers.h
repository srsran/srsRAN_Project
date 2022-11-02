/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "cu_cp_configuration.h"

// TODO: This file is temporary. Eventually we will receive cell configurations from the CU config file.

namespace srsgnb {
namespace config_helpers {

/// Generates default cell configuration used by gNB DU. The default configuration should be valid.
inline srs_cu_cp::cu_cp_configuration make_default_cu_cp_config()
{
  srs_cu_cp::cu_cp_configuration cfg{};

  cfg.ngc_config.gnb_id        = 411;
  cfg.ngc_config.ran_node_name = "srsgnb01";
  cfg.ngc_config.plmn_id       = "00f110";
  cfg.ngc_config.tac           = 7;

  return cfg;
}

} // namespace config_helpers
} // namespace srsgnb
