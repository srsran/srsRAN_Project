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

#include "../node_connection_manager/node_connection_notifier.h"
#include "srsran/rrc/rrc_config.h"

namespace srsran {

namespace srs_cu_cp {

struct du_processor_config_t {
  std::string             name         = "srs_cu_cp";
  du_index_t              du_index     = du_index_t::invalid;
  uint8_t                 rrc_version  = 2;
  srslog::basic_logger&   logger       = srslog::fetch_basic_logger("CU-CP");
  du_connection_notifier* conn_handler = nullptr;
  rrc_cfg_t               rrc_cfg;                     // TODO: do we put subcomponent configs here?
  security_indication_t   default_security_indication; // default if not signaled via NGAP
};

} // namespace srs_cu_cp

} // namespace srsran
