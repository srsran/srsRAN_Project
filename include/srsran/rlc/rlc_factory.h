/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rlc/rlc_config.h"
#include "srsran/rlc/rlc_entity.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers2.h"
#include <memory>

namespace srsran {

struct rlc_entity_creation_message {
  du_ue_index_t                        ue_index;
  rb_id_t                              rb_id;
  rlc_config                           config;
  rlc_rx_upper_layer_data_notifier*    rx_upper_dn;
  rlc_tx_upper_layer_data_notifier*    tx_upper_dn;
  rlc_tx_upper_layer_control_notifier* tx_upper_cn;
  rlc_tx_lower_layer_notifier*         tx_lower_dn;
  timer_manager2*                      timers;
  task_executor*                       pcell_executor;
  task_executor*                       ue_executor;
};

/// Creates an instance of a RLC bearer
std::unique_ptr<rlc_entity> create_rlc_entity(const rlc_entity_creation_message& msg);

} // namespace srsran
