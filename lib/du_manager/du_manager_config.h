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
#include "srsgnb/du_manager/du_manager_params.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

namespace srs_du {

struct du_manager_config_t {
  srslog::basic_logger&                 logger;
  du_setup_params                       setup_params; /// Will be merged with top-level config struct
  std::vector<du_cell_config>           du_cells;
  du_manager_params::service_params     services;
  du_manager_params::f1ap_config_params f1ap;
  du_manager_params::rlc_config_params  rlc;
  du_manager_params::mac_config_params  mac;
};

} // namespace srs_du
} // namespace srsgnb
