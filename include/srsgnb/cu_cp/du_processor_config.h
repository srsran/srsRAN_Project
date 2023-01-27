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

#include "srsgnb/rrc/rrc_config.h"

namespace srsgnb {

namespace srs_cu_cp {

struct du_processor_config_t {
  std::string           name        = "srs_cu_cp";
  du_index_t            du_index    = INVALID_DU_INDEX;
  uint8_t               rrc_version = 2;
  srslog::basic_logger& logger      = srslog::fetch_basic_logger("CU-CP");
  rrc_cfg_t             rrc_cfg; // TODO: do we put subcomponent configs here?
};

} // namespace srs_cu_cp

} // namespace srsgnb
