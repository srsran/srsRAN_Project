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

#include "adapters.h"
#include "srsgnb/du_high/du_high_configuration.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/rlc/rlc.h"
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

  f1c_message_handler& get_f1c_message_handler();

  mac_pdu_handler& get_pdu_handler(du_cell_index_t cell_index);

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index);

private:
  du_high_configuration cfg;

  timer_manager timers;

  std::unique_ptr<du_manager_interface> du_manager;
  std::unique_ptr<f1_interface>         f1ap;
  std::unique_ptr<mac_interface>        mac;

  std::unique_ptr<mac_cell_slot_handler> main_cell_slot_handler;

  du_manager_mac_event_indicator mac_ev_notifier;
};

} // namespace srs_du
} // namespace srsgnb
