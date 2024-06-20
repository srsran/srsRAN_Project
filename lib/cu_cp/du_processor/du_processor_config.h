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

#include "../cu_cp_controller/node_connection_notifier.h"
#include "du_configuration_handler.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/rrc/rrc_config.h"

namespace srsran {
namespace srs_cu_cp {

struct du_processor_config_t {
  std::string                               name           = "srs_cu_cp";
  du_index_t                                du_index       = du_index_t::invalid;
  uint8_t                                   rrc_version    = 2;
  srslog::basic_logger&                     logger         = srslog::fetch_basic_logger("CU-CP");
  du_connection_notifier*                   du_setup_notif = nullptr;
  rrc_cfg_t                                 rrc_cfg;                     // TODO: do we put subcomponent configs here?
  security_indication_t                     default_security_indication; // default if not signaled via NGAP
  f1ap_configuration                        f1ap_cfg;
  std::unique_ptr<du_configuration_handler> du_cfg_hdlr;
};

} // namespace srs_cu_cp

} // namespace srsran
