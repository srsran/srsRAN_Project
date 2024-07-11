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
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"

namespace srsran {
namespace srs_cu_cp {

struct du_processor_config_t {
  du_index_t                                du_index = du_index_t::invalid;
  const cu_cp_configuration&                cu_cp_cfg;
  srslog::basic_logger&                     logger         = srslog::fetch_basic_logger("CU-CP");
  du_connection_notifier*                   du_setup_notif = nullptr;
  std::unique_ptr<du_configuration_handler> du_cfg_hdlr;
};

} // namespace srs_cu_cp
} // namespace srsran
