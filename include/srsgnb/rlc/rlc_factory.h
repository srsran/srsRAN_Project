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

#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_entity.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {

struct rlc_entity_creation_message {
  du_ue_index_t                        ue_index;
  lcid_t                               lcid;
  rlc_config                           config;
  rlc_rx_upper_layer_data_notifier*    rx_upper_dn;
  rlc_tx_upper_layer_data_notifier*    tx_upper_dn;
  rlc_tx_upper_layer_control_notifier* tx_upper_cn;
  rlc_tx_lower_layer_notifier*         tx_lower_dn;
  timer_manager*                       timers;
  task_executor*                       pcell_executor;
};

/// Creates an instance of a RLC bearer
std::unique_ptr<rlc_entity> create_rlc_entity(const rlc_entity_creation_message& msg);

} // namespace srsgnb
