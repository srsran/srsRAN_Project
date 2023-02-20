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

#include "adapters.h"
#include "f1ap_adapters.h"
#include "srsgnb/du_high/du_high_configuration.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1ap/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/scheduler/scheduler_metrics.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

class du_high
{
public:
  explicit du_high(const du_high_configuration& cfg_);
  ~du_high();

  void start();
  void stop();

  f1ap_message_handler& get_f1ap_message_handler();

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index);

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index);

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index);

  mac_cell_control_information_handler& get_control_information_handler(du_cell_index_t cell_index);

private:
  du_high_configuration cfg;

  timer_manager& timers;

  f1ap_du_configurator_adapter f1ap_du_cfg_handler;

  std::unique_ptr<scheduler_ue_metrics_notifier> metrics_notifier;

  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1ap_interface>       f1ap;
  std::unique_ptr<mac_interface>        mac;

  std::unique_ptr<mac_cell_slot_handler> main_cell_slot_handler;

  du_manager_mac_event_indicator mac_ev_notifier;
};

} // namespace srs_du
} // namespace srsgnb
