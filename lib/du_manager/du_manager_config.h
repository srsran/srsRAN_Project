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

#include "srsgnb/du/du_cell_config.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

class timer_manager;

namespace srs_du {

struct du_manager_config_t {
  timer_manager*                               timers;
  mac_ue_configurator*                         mac_ue_mng;
  mac_cell_manager*                            mac_cell_mng;
  f1ap_connection_manager*                     f1ap_conn_mng;
  f1ap_ue_context_manager*                     f1ap_ue_ctx_mng;
  f1ap_rrc_message_transfer_procedure_handler* f1ap_rrc;
  f1c_message_handler*                         f1c_msg_handler;
  srslog::basic_logger&                        logger = srslog::fetch_basic_logger("DU-MNG");
  task_executor*                               du_mng_exec;
  du_setup_params                              setup_params; /// Will be merged with top-level config struct
  std::vector<du_cell_config>                  du_cells;
};

} // namespace srs_du
} // namespace srsgnb
