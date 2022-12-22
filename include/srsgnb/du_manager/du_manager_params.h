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
#include "srsgnb/du_high/du_high_cell_executor_mapper.h"
#include "srsgnb/du_high/du_high_ue_executor_mapper.h"
#include "srsgnb/f1ap/du/f1ap_du.h"
#include "srsgnb/f1u/du/f1u_gateway.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {
namespace srs_du {

struct du_manager_params {
  struct ran_params {
    span<du_cell_config> cells;
  };
  struct service_params {
    timer_manager&                timers;
    task_executor&                du_mng_exec;
    du_high_ue_executor_mapper&   ue_execs;
    du_high_cell_executor_mapper& cell_execs;
  };
  struct f1ap_config_params {
    f1ap_connection_manager& conn_mng;
    f1ap_ue_context_manager& ue_mng;
  };
  struct f1u_config_params {
    f1u_du_gateway& f1u_gw;
  };
  struct rlc_config_params {
    mac_ue_control_information_handler&          mac_ue_info_handler;
    f1c_message_handler&                         f1c_rx_msg_handler;
    f1ap_rrc_message_transfer_procedure_handler& f1c_rx_proc_handler;
  };
  struct mac_config_params {
    mac_cell_manager&    cell_mng;
    mac_ue_configurator& ue_cfg;
  };

  ran_params         ran;
  service_params     services;
  f1ap_config_params f1ap;
  f1u_config_params  f1u;
  rlc_config_params  rlc;
  mac_config_params  mac;
};

} // namespace srs_du
} // namespace srsgnb